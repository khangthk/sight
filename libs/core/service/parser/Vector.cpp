/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

// cspell:ignore NOLINTNEXTLINE

#include "service/parser/Vector.hpp"

#include <data/Vector.hpp>

#include <boost/foreach.hpp>

namespace sight::service::parser
{

//------------------------------------------------------------------------------

void Vector::updating()
{
    SIGHT_FATAL("This method is deprecated, and thus shouldn't be used.");
}

//------------------------------------------------------------------------------

void Vector::createConfig(core::tools::Object::sptr _obj)
{
    // Declaration of attributes values
    const std::string OBJECT_BUILD_MODE = "src";
    const std::string BUILD_OBJECT      = "new";
    const std::string GET_OBJECT        = "ref";

    auto vector = data::Vector::dynamicCast(_obj);
    SIGHT_ASSERT("The passed object must be a data::Vector", vector);

    for(const auto& elem : m_cfg.get_child("object"))
    {
        if(elem.first == "item")
        {
            const auto buildMode = elem.second.get<std::string>(OBJECT_BUILD_MODE, BUILD_OBJECT);
            SIGHT_ASSERT(
                "The buildMode \"" << buildMode << "\" is not supported, it should be either BUILD_OBJECT"
                                                   "or GET_OBJECT.",
                buildMode == BUILD_OBJECT || buildMode == GET_OBJECT
            );

            if(buildMode == BUILD_OBJECT)
            {
                // Create and manage object config
                auto ctm = service::AppConfigManager::New();
                ctm->service::IAppConfigManager::setConfig(elem.second);

                m_ctmContainer.push_back(ctm);
                ctm->create();
                data::Object::sptr localObj = ctm->getConfigRoot();

                // Add object
                SIGHT_ASSERT("A data::Vector can contain only data::Object", localObj);
                vector->push_back(localObj);
            }
            else // if( buildMode == GET_OBJECT )
            {
                SIGHT_FATAL("ACH => Todo");
            }
        }
    }
}

//------------------------------------------------------------------------------

void Vector::startConfig()
{
    for(const service::IAppConfigManager::sptr& ctm : m_ctmContainer)
    {
        ctm->start();
    }
}

//------------------------------------------------------------------------------

void Vector::updateConfig()
{
    for(const service::IAppConfigManager::sptr& ctm : m_ctmContainer)
    {
        ctm->update();
    }
}

//------------------------------------------------------------------------------

void Vector::stopConfig()
{
    // NOLINTNEXTLINE(bugprone-branch-clone)
    BOOST_REVERSE_FOREACH(service::IAppConfigManager::sptr ctm, m_ctmContainer)
    {
        ctm->stop();
    }
}

//------------------------------------------------------------------------------

void Vector::destroyConfig()
{
    // NOLINTNEXTLINE(bugprone-branch-clone)
    BOOST_REVERSE_FOREACH(service::IAppConfigManager::sptr ctm, m_ctmContainer)
    {
        ctm->destroy();
    }
    m_ctmContainer.clear();
}

//------------------------------------------------------------------------------

} // namespace sight::service::parser
