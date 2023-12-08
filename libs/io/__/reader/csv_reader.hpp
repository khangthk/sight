/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#pragma once

#include "io/__/config.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace sight::io::reader
{

/**
 * @brief Read CSV file and returns parsed tokens.
 * The input file is supposed to use comma separator, but another separator can be used when reading file.
 */
class IO_CLASS_API csv_reader
{
public:

    /**
     * @brief Constructor
     *
     * @param csvStream the stream to read
     */
    IO_API csv_reader(const std::filesystem::path& _csv_path);

    /// Destructor
    IO_API virtual ~csv_reader();

    /// Containers to store parsed tokens.
    using token_container_t = std::vector<std::string>;

    /**
     * @brief Returns tokens on next line to read, using comma separator
     *
     * Other separators can be used
     * @param separator token separator
     *
     * @return tokens found on next line separated with given characters
     */
    IO_API token_container_t get_line(const std::string& _separator = ",");

private:

    /// Associated stream
    std::ifstream m_stream;
};

} // namespace sight::io::reader