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

#include "SGetSeries.hpp"

namespace sight::module::data
{

//------------------------------------------------------------------------------

void SGetSeries::configuring()
{
    service::IService::ConfigType config = this->getConfigTree();
    const ConfigType configOut           = config.get_child("out");

    const auto keyCfg = configOut.equal_range("key");
    for(auto itCfg = keyCfg.first ; itCfg != keyCfg.second ; ++itCfg)
    {
        const service::IService::ConfigType& attr = itCfg->second.get_child("<xmlattr>.index");
        const size_t index                        = attr.get_value<size_t>();
        m_indexNumbers.push_back(index);
    }
}

//------------------------------------------------------------------------------

void SGetSeries::starting()
{
}

//-----------------------------------------------------------------------------

void SGetSeries::updating()
{
    const auto seriesDB = m_seriesDB.lock();
    if(seriesDB == nullptr)
    {
        SIGHT_THROW_EXCEPTION(sight::data::Exception("Missing input database series"));
    }

    auto series = seriesDB->getContainer();
    if(series.empty())
    {
        SIGHT_THROW_EXCEPTION(sight::data::Exception("The database series is empty, nothing can be extracted."));
    }

    size_t i = 0;
    for(const auto& index : m_indexNumbers)
    {
        m_series[i] = series[index];
        i++;
    }
}

//-----------------------------------------------------------------------------

void SGetSeries::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data
