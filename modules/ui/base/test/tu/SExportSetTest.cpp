/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "SExportSetTest.hpp"

#include <core/runtime/runtime.hpp>

#include <data/Set.hpp>
#include <data/String.hpp>

#include <service/base.hpp>

#include <ui/base/dialog/DummyInputDialog.hpp>
#include <ui/base/dialog/IInputDialog.hpp>
#include <ui/base/registry/macros.hpp>

#include <utest/wait.hpp>

#include <boost/dll.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::base::ut::SExportSetTest);

SIGHT_REGISTER_GUI(sight::ui::base::dialog::DummyInputDialog, sight::ui::base::dialog::IInputDialog::REGISTRY_KEY);

namespace sight::module::ui::base::ut
{

//------------------------------------------------------------------------------

void SExportSetTest::setUp()
{
    m_exportSet = service::add("sight::module::ui::base::SExportSet");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::ui::base::SExportSet'", m_exportSet);
}

//------------------------------------------------------------------------------

void SExportSetTest::tearDown()
{
    if(!m_exportSet->isStopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_exportSet->stop().get());
    }

    service::remove(m_exportSet);
}

//------------------------------------------------------------------------------

void SExportSetTest::basicTest()
{
    data::String::sptr helloWorld = data::String::New("Hello world!");
    data::Set::sptr set           = data::Set::New();
    m_exportSet->setInOut(helloWorld, "data");
    m_exportSet->setInOut(set, "container");
    CPPUNIT_ASSERT(set->empty());
    CPPUNIT_ASSERT_NO_THROW(m_exportSet->configure());
    CPPUNIT_ASSERT_NO_THROW(m_exportSet->start().get());
    sight::ui::base::dialog::DummyInputDialog::pushInput("I don't care");
    CPPUNIT_ASSERT_NO_THROW(m_exportSet->update().get());
    CPPUNIT_ASSERT_NO_THROW(m_exportSet->stop().get());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), set->size());
    CPPUNIT_ASSERT((*set)[0] == helloWorld);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::base::ut
