/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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
#include "io/__/reader/factory/new.hpp"
#include "io/__/reader/registry/detail.hpp"

#include <core/base.hpp>
#include <core/jobs/base.hpp>
#include <core/tools/object.hpp>

#include <cstdint>
#include <filesystem>
#include <functional>

namespace sight::io::reader
{

/**
 * @brief   Base class for all object readers.
 *
 * This class defines the API to use basic object readers. This reader is not a
 * service. Their equivalent exist as services. See io::service::reader. To read an
 * object with this class, use setLocation and setOject method before executing
 * the method read. Reading modifies the object given in parameter with the
 * method set_object.
 */
class IO_CLASS_API object_reader : public core::base_object
{
public:

    SIGHT_DECLARE_CLASS(object_reader, core::base_object);

    using progress_callback = std::function<void (std::uint64_t)>;
    using cancel_callback   = std::function<void ()>;

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T factory product type
     */
    template<typename T>
    class registry
    {
    public:

        registry()
        {
            sight::io::reader::registry::get()->add_factory(T::classname(), &sight::io::reader::factory::make<T>);
        }
    };

    /**
     * @brief Defines an reader interface.
     *
     * This method (re)inits the object given in parameter of set_object method.
     */
    IO_API virtual void read() = 0;

    /**
     * @brief m_object setter.
     * @param[out] _pObject replaces weakptr m_object of the instance reader
     * @note m_object is saved in class with a weakptr
     * @note This object can be get with the method get_object()
     */
    IO_API virtual void set_object(core::tools::object::sptr _p_object);

    /**
     * @brief m_object getter.
     *
     * @return m_object
     * @note m_object is saved in reader with a weakptr
     */
    IO_API virtual core::tools::object::sptr get_object() const;

    IO_API virtual std::string extension() const = 0;

    /**
     * @brief Requests reader abortion.
     */
    IO_API void cancel() const;

    /// Returns the internal job
    IO_API virtual SPTR(core::jobs::base) get_job() const
    {
        return nullptr;
    }

protected:

    /// Destructor. Does nothing.
    IO_API ~object_reader() override = default;

    /**
     * @brief Object result of reading process.
     *
     * This object is given in parameter of set_object method but it is conserved with a weakptr.
     */
    core::tools::object::wptr m_object;
};

} // namespace sight::io::reader
