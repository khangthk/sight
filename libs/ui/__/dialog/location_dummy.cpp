/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "location_dummy.hpp"

#include <core/location/multiple_files.hpp>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

namespace sight::ui::dialog
{

std::queue<std::vector<std::filesystem::path> > location_dummy::s_paths_list;

//------------------------------------------------------------------------------

void location_dummy::set_type(types _type)
{
    m_type = _type;
}

//------------------------------------------------------------------------------

void location_dummy::set_option(options /*option*/)
{
}

//------------------------------------------------------------------------------

void location_dummy::add_filter(const std::string& /*filterName*/, const std::string& /*wildcardList*/)
{
}

//------------------------------------------------------------------------------

std::string location_dummy::get_current_selection() const
{
    return "";
}

//------------------------------------------------------------------------------

void location_dummy::set_paths(const std::vector<std::filesystem::path>& _files)
{
    push_paths(_files);
}

//------------------------------------------------------------------------------

void location_dummy::push_paths(const std::vector<std::filesystem::path>& _files)
{
    s_paths_list.push(_files);
}

//------------------------------------------------------------------------------

bool location_dummy::clear()
{
    if(s_paths_list.empty())
    {
        return true;
    }

    while(!s_paths_list.empty())
    {
        s_paths_list.pop();
    }

    return false;
}

//------------------------------------------------------------------------------

sight::core::location::base::sptr location_dummy::show()
{
    std::vector<std::filesystem::path> paths = s_paths_list.front();
    s_paths_list.pop();
    if(m_type == single_file)
    {
        auto single_file = std::make_shared<sight::core::location::single_file>();
        single_file->set_file(paths[0]);
        return single_file;
    }

    if(m_type == folder)
    {
        auto single_folder = std::make_shared<sight::core::location::single_folder>();
        single_folder->set_folder(paths[0]);
        return single_folder;
    }

    auto multiple_files = std::make_shared<sight::core::location::multiple_files>();
    multiple_files->set_files(paths);
    return multiple_files;
}

} // namespace sight::ui::dialog
