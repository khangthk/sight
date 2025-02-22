/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "spy_log_test.hpp"

#include <core/mt/types.hpp>
#include <core/spy_log.hpp>

#include <utest/exception.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/regex_find_format.hpp>
#include <boost/log/core.hpp>

#include <exception>
#include <iostream>
#include <regex>
#include <streambuf>
#include <string>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::log::ut::spy_log_test);

namespace sight::core::log::ut
{

static utest::exception e("");

//------------------------------------------------------------------------------

void spy_log_test::setUp()
{
    m_sink = core::log::spy_logger::add_global_console_log(m_ostream);
}

//-----------------------------------------------------------------------------

void spy_log_test::tearDown()
{
    boost::log::core::get()->remove_sink(m_sink);
    m_sink.reset();
    m_ostream.clear();
}

//-----------------------------------------------------------------------------

void spy_log_test::log_message_test()
{
    std::vector<std::string> logs;

    logs.emplace_back("trace message");
    sight::core::log::g_logger.trace(logs.back(), __FILE__, __LINE__);
    sight::core::log::ut::spy_log_test::check_log(logs, sight::core::log::ut::spy_log_test::log_to_vector(m_ostream));

    logs.emplace_back("debug message");
    sight::core::log::g_logger.debug(logs.back(), __FILE__, __LINE__);
    sight::core::log::ut::spy_log_test::check_log(logs, sight::core::log::ut::spy_log_test::log_to_vector(m_ostream));

    logs.emplace_back("info message");
    sight::core::log::g_logger.info(logs.back(), __FILE__, __LINE__);
    sight::core::log::ut::spy_log_test::check_log(logs, sight::core::log::ut::spy_log_test::log_to_vector(m_ostream));

    logs.emplace_back("warn message");
    sight::core::log::g_logger.warn(logs.back(), __FILE__, __LINE__);
    sight::core::log::ut::spy_log_test::check_log(logs, sight::core::log::ut::spy_log_test::log_to_vector(m_ostream));

    logs.emplace_back("error message");
    sight::core::log::g_logger.error(logs.back(), __FILE__, __LINE__);
    sight::core::log::ut::spy_log_test::check_log(logs, sight::core::log::ut::spy_log_test::log_to_vector(m_ostream));

    logs.emplace_back("fatal message");
    sight::core::log::g_logger.fatal(logs.back(), __FILE__, __LINE__);
    sight::core::log::ut::spy_log_test::check_log(logs, sight::core::log::ut::spy_log_test::log_to_vector(m_ostream));
}

//-----------------------------------------------------------------------------

struct log_producer_thread
{
    using sptr            = std::shared_ptr<log_producer_thread>;
    using log_container_t = std::vector<std::string>;

    log_producer_thread()
    = default;

    //------------------------------------------------------------------------------

    static void run(log_container_t& _logs, std::size_t _nb_logs, std::size_t _offset)
    {
        for(std::size_t i = _offset ; i < _nb_logs + _offset ; ++i)
        {
            std::stringstream ss;
            ss << "msg n ";
            ss.width(10);
            ss.fill('0');
            ss << i;
            _logs[i] = ss.str();
            sight::core::log::g_logger.fatal(_logs[i], __FILE__, __LINE__);
        }
    }
};

//-----------------------------------------------------------------------------

struct regex_log_compare
{
    //------------------------------------------------------------------------------

    bool operator()(std::string _a, std::string _b)
    {
        std::regex re(".*(msg n [\\d]+)$");
        std::smatch match_a;
        std::smatch match_b;
        bool do_match_a = std::regex_match(_a, match_a, re);
        bool do_match_b = std::regex_match(_b, match_b, re);
        CPPUNIT_ASSERT_MESSAGE(std::string("Regex do not match ") + _a, do_match_a);
        CPPUNIT_ASSERT_MESSAGE(std::string("Regex do not match ") + _b, do_match_b);

        std::string str_a(match_a[1].first, match_a[1].second);
        std::string str_b(match_b[1].first, match_b[1].second);

        return str_a < str_b;
    }
} regex_compare;

//------------------------------------------------------------------------------

void spy_log_test::thread_safety_test()
{
    const std::size_t nb_thread(20);
    const std::size_t nb_log(20);
    log_producer_thread::log_container_t logs(nb_thread * nb_log, "test");
    std::vector<std::thread> tg;
    for(std::size_t i = 0 ; i < nb_thread ; ++i)
    {
        log_producer_thread::sptr ct = std::make_shared<log_producer_thread>();
        std::size_t offset           = i * nb_log;
        tg.emplace_back([&, offset](auto&& ...){log_producer_thread::run(logs, nb_log, offset);});
    }

    for(auto& t : tg)
    {
        t.join();
    }

    log_producer_thread::log_container_t log_messages = sight::core::log::ut::spy_log_test::log_to_vector(m_ostream);

    std::sort(log_messages.begin(), log_messages.end(), regex_compare);
    sight::core::log::ut::spy_log_test::check_log(logs, log_messages);
}

//-----------------------------------------------------------------------------

std::vector<std::string> spy_log_test::log_to_vector(const std::stringstream& _logs_stream)
{
    std::vector<std::string> lines;
    std::string line;
    std::istringstream input;
    input.str(_logs_stream.str());
    while(std::getline(input, line))
    {
        lines.push_back(line);
    }

    return lines;
}

//-----------------------------------------------------------------------------

void spy_log_test::check_log(
    const std::vector<std::string>& _log_messages_ref,
    const std::vector<std::string>& _log_messages
)
{
    CPPUNIT_ASSERT_EQUAL(_log_messages_ref.size(), _log_messages.size());

    const std::string line_pattern("(\\[[0-9]+\\])");
    const std::string time_pattern("(\\[.+\\])");
    const std::string level_pattern("( *\\[[a-z]+\\])");
    const std::string file_pattern("( \\[.*:)");
    const std::string file_line_pattern("([0-9]+\\] )");
    const std::string message_pattern("(.*)$");

    std::regex re(std::string(line_pattern) + time_pattern + level_pattern + file_pattern
                  + file_line_pattern + message_pattern);

    std::smatch match;
    std::string regex_message;
    std::size_t i = 0;

    for(const std::string& log : _log_messages)
    {
        const bool do_match = std::regex_match(log, match, re);
        CPPUNIT_ASSERT_MESSAGE(log + " doesn't match regex.", do_match);

        regex_message.assign(match[6].first, match[6].second);
        CPPUNIT_ASSERT_EQUAL(_log_messages_ref[i], regex_message);
        ++i;
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::core::log::ut
