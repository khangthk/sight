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

#include "data/helper/SeriesDB.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>

#include <data/SeriesDB.hpp>

namespace sight::data::helper
{

//-----------------------------------------------------------------------------

SeriesDB::SeriesDB(data::SeriesDB& seriesDB) :
    m_seriesDB(seriesDB)
{
}

//-----------------------------------------------------------------------------

SeriesDB::~SeriesDB()
= default;

//-----------------------------------------------------------------------------

void SeriesDB::add(data::Series::sptr newSeries)
{
    SIGHT_ASSERT(
        "The object " << newSeries->getID() << " must not exist in SeriesDB.",
        std::find(m_seriesDB.begin(), m_seriesDB.end(), newSeries) == m_seriesDB.end()
    );

    // Modify SeriesDB
    m_seriesDB.getContainer().push_back(newSeries);

    m_addedSeries.push_back(newSeries);
}

//-----------------------------------------------------------------------------

void SeriesDB::remove(data::Series::sptr oldSeries)
{
    auto iter = std::find(m_seriesDB.begin(), m_seriesDB.end(), oldSeries);
    SIGHT_ASSERT(
        "The object " << oldSeries->getID() << " must exist in SeriesDB.",
        iter != m_seriesDB.end()
    );

    // Modify SeriesDB
    m_seriesDB.getContainer().erase(iter);

    m_removedSeries.push_back(oldSeries);
}

//-----------------------------------------------------------------------------

void SeriesDB::clear()
{
    while(!m_seriesDB.empty())
    {
        this->remove(m_seriesDB.front());
    }
}

//-----------------------------------------------------------------------------

void SeriesDB::merge(data::SeriesDB::sptr seriesDBIn)
{
    auto& container = m_seriesDB.getContainer();

    for(const data::Series::sptr& series : seriesDBIn->getContainer())
    {
        // Only add the new series if not already here
        if(std::find(container.cbegin(), container.cend(), series) == container.cend())
        {
            container.push_back(series);
            m_addedSeries.push_back(series);
        }
    }
}

//-----------------------------------------------------------------------------

void SeriesDB::notify()
{
    if(!m_addedSeries.empty())
    {
        auto sig = m_seriesDB.signal<data::SeriesDB::AddedSeriesSignalType>(
            data::SeriesDB::s_ADDED_SERIES_SIG
        );
        sig->asyncEmit(m_addedSeries);
    }

    if(!m_removedSeries.empty())
    {
        auto sig = m_seriesDB.signal<data::SeriesDB::RemovedSeriesSignalType>(
            data::SeriesDB::s_REMOVED_SERIES_SIG
        );
        sig->asyncEmit(m_removedSeries);
    }

    SIGHT_INFO_IF(
        "No changes were found on the SeriesDB '" + m_seriesDB.getID() + "', nothing to notify.",
        m_addedSeries.empty() && m_removedSeries.empty()
    );
}

//-----------------------------------------------------------------------------

} // namespace sight::data::helper
