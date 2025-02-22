/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

#include "sha256.hpp"

#include <openssl/sha.h>

#include <array>
#include <iomanip>
#include <sstream>

namespace sight::core::crypto
{

//------------------------------------------------------------------------------

void hash(const secure_string& _message, std::array<std::uint8_t, HASH_SIZE>& _output)
{
    // Compute SHA256 using openssl
    SHA256(reinterpret_cast<const std::uint8_t*>(_message.data()), _message.size(), _output.data());
}

//------------------------------------------------------------------------------

secure_string hash(const secure_string& _message)
{
    // The hash array
    std::array<std::uint8_t, HASH_SIZE> output {};

    // Compute SHA256 using openssl
    hash(_message, output);

    // Convert the hash to an hexadecimal string
    std::ostringstream stream;

    stream << std::hex << std::setfill('0');
    for(int byte : output)
    {
        stream << std::setw(2) << byte;
    }

    return secure_string(stream.str());
}

} // namespace sight::core::crypto
