/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ViewportTest.hpp"

#include <service/op.hpp>

#include <viz/scene2d/data/parser/Viewport.hpp>
#include <viz/scene2d/data/Viewport.hpp>

#include <boost/property_tree/xml_parser.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::scene2d::ut::ViewportTest);

//------------------------------------------------------------------------------

namespace sight::viz::scene2d::ut
{

//------------------------------------------------------------------------------

void ViewportTest::setUp()
{
}

//------------------------------------------------------------------------------

void ViewportTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ViewportTest::testParser()
{
    service::config_t config;

    std::stringstream config_string(R"(<config x="-700" y="0.1" width="100" height="1.3"/>)");
    boost::property_tree::read_xml(config_string, config);

    auto parser = sight::service::add<data::parser::Viewport>("sight::viz::scene2d::data::parser::Viewport");
    parser->setObjectConfig(config);

    auto viewport = std::make_shared<sight::viz::scene2d::data::Viewport>();
    parser->createConfig(viewport);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(-700., viewport->x(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(.1, viewport->y(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(100., viewport->width(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.3, viewport->height(), 0.00001);
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene2d::ut
