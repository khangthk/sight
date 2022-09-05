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

#if defined(__unix__)

#include "core/runtime/detail/dl/Posix.hpp"

#include "core/runtime/Module.hpp"

namespace sight::core::runtime::detail::dl
{

//------------------------------------------------------------------------------

Posix::Posix(const std::filesystem::path& modulePath) noexcept :
    Native(modulePath)
{
}

//------------------------------------------------------------------------------

Posix::~Posix() noexcept =
    default;

//------------------------------------------------------------------------------

bool Posix::isLoaded() const noexcept
{
    return m_handle != nullptr;
}

//------------------------------------------------------------------------------

void* Posix::getSymbol(const std::string& name) const
{
    void* result = nullptr;
    if(isLoaded())
    {
        dlerror(); /* Clear existing error */
        result = dlsym(m_handle, name.c_str());
        if(result == nullptr) /* Check for possible errors */
        {
            std::string message(dlerror());
            if(!message.empty())
            {
                throw RuntimeException("Symbol retrieval failed. " + message);
            }
        }
    }

    return result;
}

//------------------------------------------------------------------------------

void Posix::load()
{
    if(m_handle == nullptr)
    {
        // Opens the dynamic library.
        m_handle = dlopen(getFullPath().string().c_str(), RTLD_LAZY | RTLD_GLOBAL);
        if(m_handle == nullptr)
        {
            std::string message(dlerror());
            throw RuntimeException("Module load failed. " + message);
        }
    }
}

//------------------------------------------------------------------------------

void Posix::unload()
{
    if(m_handle != nullptr)
    {
        int result = 0;
        result = dlclose(m_handle);
        if(result != 0)
        {
            std::string message(dlerror());
            throw RuntimeException("Module unload failed. " + message);
        }

        m_handle = nullptr;
    }
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail::dl

#endif // #if defined(__unix__)
