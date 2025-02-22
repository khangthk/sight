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

// cspell:ignore SEM_NOGPFAULTERRORBOX

#ifdef _WIN32
#include <windows.h>
#endif

#include <core/runtime/runtime.hpp>

#include <core/crypto/password_keeper.hpp>
#include <core/runtime/profile/profile.hpp>
#include <core/os/temp_path.hpp>
#include <core/tools/os.hpp>
#include <core/tools/system.hpp>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <csignal>
#include <filesystem>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>

//------------------------------------------------------------------------------

namespace po = boost::program_options;

using sight::core::crypto::secure_string;

//------------------------------------------------------------------------------

template<class A1, class A2>
inline static std::ostream& operator<<(std::ostream& _s, std::vector<A1, A2> const& _vec)
{
    copy(_vec.begin(), _vec.end(), std::ostream_iterator<A1>(_s, " "));
    return _s;
}

//-----------------------------------------------------------------------------

/// Wrapper for std::filesystem::absolute, needed by clang 3.0 in use with
/// std::transform
inline static std::filesystem::path absolute(const std::filesystem::path& _path)
{
    return std::filesystem::weakly_canonical(_path);
}

//-----------------------------------------------------------------------------

volatile sig_atomic_t g_signal_status = 0;
//------------------------------------------------------------------------------

void signal_handler(int _signal)
{
    g_signal_status = _signal;

    try
    {
        const auto& profile = sight::core::runtime::get_current_profile();

        if(profile != nullptr)
        {
            profile->stop();
        }
    }
    catch(const std::exception& e)
    {
        SIGHT_FATAL(e.what());
    }
    catch(...)
    {
        SIGHT_FATAL("An unrecoverable error has occurred.");
    }

    // We use brutal exit because when interrupted by a signal, we never get out from run,
    // even if the program is fully terminated
    exit(0);
}

//------------------------------------------------------------------------------

inline static std::filesystem::path build_log_file_path(
    const std::filesystem::path& _log_dir,
    bool _encrypted_log
)
{
    // Use the default log file name as base
    std::filesystem::path log_file_path = _encrypted_log
                                          ? sight::core::log::ENCRYPTED_LOG_FILE
                                          : sight::core::log::LOG_FILE;

    // Prepend the log directory
    if(!_log_dir.empty())
    {
        std::filesystem::create_directories(_log_dir);
        log_file_path = _log_dir / log_file_path;
    }

    // Test if the directory is writable. An exception will be thrown if not
    {
        std::ofstream ostream;
        ostream.open(log_file_path, std::ios::out | std::ios::binary | std::ios::trunc);
    }

    // Cleanup
    std::filesystem::remove_all(log_file_path);

    return log_file_path;
}

//------------------------------------------------------------------------------

inline static std::filesystem::path find_log_file_path(
    const std::filesystem::path& _log_file,
    const std::string& _profile_name,
    bool _encrypted_log
)
{
    if(_log_file.empty())
    {
        try
        {
            // Try the user cache directory
            return build_log_file_path(
                sight::core::tools::os::get_user_cache_dir(_profile_name),
                _encrypted_log
            );
        }
        catch(...)
        {
            // Fallback: take temporary directory
            return build_log_file_path(
                std::filesystem::temp_directory_path() / "sight" / _profile_name,
                _encrypted_log
            );
        }
    }

    // Take the user choice
    return _log_file;
}

//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    std::vector<std::filesystem::path> module_paths;
    std::filesystem::path profile_file;
    sight::core::runtime::profile::params_container profile_args;

    // Runner options
    po::options_description options("Sight application launcher options");
    options.add_options()
    ("help,h", "Show help message")
    ("module-path,B", po::value(&module_paths), "Adds a module path")
    ;

    // Log options
    bool encrypted_log = false;
    bool console_log   = false;
    bool file_log      = true;

    std::string log_file;

    using spy_logger = sight::core::log::spy_logger;
    using level_t    = spy_logger::level_t;
    auto log_level = std::underlying_type_t<level_t>(level_t::warning);

    po::options_description log_options("Log options");
    log_options.add_options()
    (
        "encrypted-log",
        po::value(&encrypted_log)->implicit_value(true)->zero_tokens(),
        "Enable log encryption. Implies compression."
    )
    (
        "no-encrypted-log",
        po::value(&encrypted_log)->implicit_value(false)->zero_tokens(),
        "Disable log encryption."
    )
    (
        "clog",
        po::value(&console_log)->implicit_value(true)->zero_tokens(),
        "Enable log output to console. Will disable file log."
    )
    (
        "no-clog",
        po::value(&console_log)->implicit_value(false)->zero_tokens(),
        "Disable log output to console"
    )
    (
        "flog",
        po::value(&file_log)->implicit_value(true)->zero_tokens(),
        "Enable log output to file. Will disable console log."
    )
    (
        "no-flog",
        po::value(&file_log)->implicit_value(false)->zero_tokens(),
        "Disable log output to file"
    )
    (
        "log-output",
        po::value(&log_file),
        "Log output filename"
    )
    (
        "log-trace",
        po::value(&log_level)->implicit_value(std::underlying_type_t<level_t>(level_t::trace))->zero_tokens(),
        "Set log_level to trace"
    )
    (
        "log-debug",
        po::value(&log_level)->implicit_value(std::underlying_type_t<level_t>(level_t::debug))->zero_tokens(),
        "Set log_level to debug"
    )
    (
        "log-info",
        po::value(&log_level)->implicit_value(std::underlying_type_t<level_t>(level_t::info))->zero_tokens(),
        "Set log_level to info"
    )
    (
        "log-warn",
        po::value(&log_level)->implicit_value(std::underlying_type_t<level_t>(level_t::warning))->zero_tokens(),
        "Set log_level to warn"
    )
    (
        "log-error",
        po::value(&log_level)->implicit_value(std::underlying_type_t<level_t>(level_t::error))->zero_tokens(),
        "Set log_level to error"
    )
    (
        "log-fatal",
        po::value(&log_level)->implicit_value(std::underlying_type_t<level_t>(level_t::fatal))->zero_tokens(),
        "Set log_level to fatal"
    );

    // Hidden options
    bool macro_mode = false;
#ifdef WIN32
    bool enableAbortDialog = true;
#endif

    po::options_description hidden("Hidden options");
    hidden.add_options()
    ("profile", po::value(&profile_file)->default_value("profile.xml"), "Profile file")
    ("profile-args", po::value(&profile_args)->multitoken(), "Profile args")
    ("macro", po::value(&macro_mode)->implicit_value(true)->zero_tokens(), "Enable macro mode")
    ("no-macro", po::value(&macro_mode)->implicit_value(false)->zero_tokens(), "Disable macro mode")
#ifdef WIN32
    ("abort-dialog", po::value(&enableAbortDialog)->implicit_value(true)->zero_tokens(), "Enable abort dialog")
    ("no-abort-dialog", po::value(&enableAbortDialog)->implicit_value(false)->zero_tokens(), "Disable abort dialog")
#endif
    ;

    // Set options
    po::options_description cmdline_options;
    cmdline_options.add(options).add(log_options).add(hidden);

    po::positional_options_description p;
    p.add("profile", 1).add("profile-args", -1);

    // Get options
    po::variables_map vm;

    try
    {
        po::store(
            po::command_line_parser(argc, argv)
            .options(cmdline_options)
            .positional(p)
            .run(),
            vm
        );
        po::notify(vm);

        if(vm.count("flog") > 0 && vm.count("clog") > 0)
        {
            throw po::error("Cannot enable both file and console log");
            return 1;
        }
    }
    catch(const po::error& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // If help
    if(vm.count("help") != 0U)
    {
        std::cout << "usage: " << argv[0] << " [options] [profile(=profile.xml)] [profile-args ...]" << std::endl;
        std::cout << "  use '--' to stop processing args for sightrun" << std::endl << std::endl;
        std::cout << options << std::endl << log_options << std::endl;
        return 0;
    }

    // If we enable console log, but not file log, we disable file log (which is enabled by default)
    if(console_log)
    {
        file_log = false;
    }

#ifdef WIN32
    if(!enableAbortDialog)
    {
        _set_error_mode(_OUT_TO_STDERR);
        SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
        _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
        _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
        _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    }
#endif

    SIGHT_INFO_IF("Profile path: " << profile_file << " => " << ::absolute(profile_file), vm.count("profile"));
    SIGHT_INFO_IF("Profile-args: " << profile_args, vm.count("profile-args"));

    // Check if profile path exist
    profile_file = ::absolute(profile_file);

    SIGHT_FATAL_IF(
        "Profile file " << profile_file << " do not exists or is not a regular file.",
        !std::filesystem::is_regular_file(profile_file)
    );

    // Check if the same application is already running and if we are in "UNIQUE" mode
    // Parse the profile.xml
    boost::property_tree::ptree profile_tree;
    boost::property_tree::read_xml(profile_file.string(), profile_tree);
    const auto& profile_name = profile_tree.get<std::string>("profile.<xmlattr>.name");

    // Log file
    if(console_log)
    {
        spy_logger::add_global_console_log(std::clog, level_t(log_level));
    }

    if(file_log)
    {
        const auto& log_file_path = find_log_file_path(log_file, profile_name, encrypted_log);

        if(encrypted_log)
        {
            // Use an empty password
            secure_string password;

#ifdef _DEBUG
            sight::core::log::g_logger.start(log_file_path, level_t(log_level), password, false);
#else
            // Use asynchronous mode in release
            sight::core::log::g_logger.start(log_file_path, level_t(log_level), password, true);
#endif
        }
        else
        {
            sight::core::log::g_logger.start(log_file_path, level_t(log_level));
        }
    }

    // This will hold the pid file if required
    std::unique_ptr<sight::core::os::temp_file> temp_pid_file;

    // check-single-instance
    if(profile_tree.get<bool>("profile.<xmlattr>.check-single-instance", false))
    {
        // Check if we already have a .pid file
        const auto& cache_dir    = sight::core::tools::os::get_user_cache_dir(profile_name);
        const auto& pid_filename = cache_dir / (profile_name + ".pid");

        // If we have a .pid file, check if the process is still running
        if(std::filesystem::exists(pid_filename) && std::filesystem::is_regular_file(pid_filename))
        {
            try
            {
                std::ifstream existing_pid_stream(pid_filename);
                std::string existing_pid_str;
                std::getline(existing_pid_stream, existing_pid_str);
                const int existing_pid = std::stoi(existing_pid_str);

                // If the process is still running, we exit
                if(sight::core::tools::system::is_process_running(existing_pid))
                {
                    SIGHT_ERROR("An instance of " << profile_name << " is already running.");
                    return 5;
                }
            }
            catch(...)
            {
                // If we cannot read the pid, we continue
                SIGHT_WARN("Cannot read pid file " << pid_filename);
            }
        }

        // If we are here, there is no other instance running or the pid file is invalid
        // We create a new pid file - It should be automatically deleted when the program exit
        temp_pid_file = std::make_unique<sight::core::os::temp_file>(std::ios::out | std::ios::trunc, pid_filename);
        (*temp_pid_file) << sight::core::tools::system::get_pid() << std::endl;
        temp_pid_file->stream().close();

        SIGHT_DEBUG("Created pid file " << temp_pid_file->string());
    }

    // Automatically adds the module folders where the profile.xml is located if it was not already there
    const auto profile_module_path = profile_file.parent_path().parent_path();
    bool find_profile_module_path  = false;
    for(const std::filesystem::path& module_path : module_paths)
    {
        if(profile_module_path == module_path)
        {
            find_profile_module_path = true;
        }
    }

    if(!find_profile_module_path)
    {
        module_paths.push_back(profile_module_path);
    }

#if SIGHT_INFO_ENABLED
    for(const std::filesystem::path& modulePath : modulePaths)
    {
        SIGHT_INFO_IF(
            "Module paths are: " << modulePath.string() << " => " << ::absolute(modulePath),
            vm.count("module-path")
        );
    }
#endif

    sight::core::runtime::init();

    for(const std::filesystem::path& module_path : module_paths)
    {
        sight::core::runtime::add_modules(module_path);
    }

    sight::core::runtime::profile::sptr profile;

    try
    {
        profile = sight::core::runtime::io::profile_reader::create_profile(profile_file);

        // Install a signal handler
        if(std::signal(SIGINT, signal_handler) == SIG_ERR)
        {
            perror("std::signal(SIGINT)");
        }

        if(std::signal(SIGTERM, signal_handler) == SIG_ERR)
        {
            perror("std::signal(SIGTERM)");
        }

#ifndef WIN32
        if(std::signal(SIGHUP, signal_handler) == SIG_ERR)
        {
            perror("std::signal(SIGHUP)");
        }

        if(std::signal(SIGQUIT, signal_handler) == SIG_ERR)
        {
            perror("std::signal(SIGQUIT)");
        }
#endif
        profile->set_params(profile_args);

        profile->start();
        if(macro_mode)
        {
            sight::core::runtime::load_module("sight::module::ui::test");
        }

        profile->run();
        if(g_signal_status == 0)
        {
            profile->stop();
        }

        if(macro_mode)
        {
            sight::core::runtime::unload_module("sight::module::ui::test");
        }
    }
    catch(const std::exception& e)
    {
        SIGHT_FATAL(e.what());
        return 3;
    }
    catch(...)
    {
        SIGHT_FATAL("An unrecoverable error has occurred.");
        return 4;
    }

    return 0;
}

//-----------------------------------------------------------------------------

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return main(__argc, __argv);
}

#endif // _WIN32
