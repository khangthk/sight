/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "fwAtomsFilter/functions.hpp"

#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/String.hpp>

#include <data/registry/detail.hpp>

#include <fwActivities/registry/Activities.hpp>

#include <fwAtomsPatch/helper/functions.hpp>

#include <algorithm>
#include <string>

namespace fwAtomsFilter
{

//------------------------------------------------------------------------------

bool isSeriesKnown(const SPTR(atoms::Object)& series)
{
    bool isKnown = false;

    namespace reg = ::fwActivities::registry;
    reg::Activities::sptr registry                = reg::Activities::getDefault();
    const std::vector< reg::ActivityInfo >& infos = registry->getInfos();

    const std::string& classname = ::fwAtomsPatch::helper::getClassname(series);

    SPTR(data::registry::Type) objectRegistry = data::registry::get();
    data::registry::Type::KeyVectorType keys            = objectRegistry->getFactoryKeys();
    data::registry::Type::KeyVectorType::iterator itKey = std::find(keys.begin(), keys.end(), classname);

    if(itKey != keys.end())
    {
        if(classname == "::sight::data::ActivitySeries")
        {
            // Check if activity config id is known
            atoms::String::sptr asId = series->getAttribute< atoms::String >("activity_config_id");

            for(reg::ActivityInfo info :  infos)
            {
                if(info.id == asId->getString())
                {
                    isKnown = true;
                    break;
                }
            }
        }
        else
        {
            isKnown = true;
        }
    }

    return isKnown;
}

} // namespace fwAtomsFilter
