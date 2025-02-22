/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "container.hpp"

#include <core/compare.hpp>

#include <glm/glm.hpp>

#include <array>
#include <iostream>

namespace sight::data
{

/**
 * @brief This class represents a 3D transformation matrix (4x4).
 *
 * Our convention is a row-major representation.
 */
class SIGHT_DATA_CLASS_API matrix4 final : public container<std::array<double,
                                                                       16> >
{
public:

    /// This will enable common collection constructors / assignment operators
    using container<matrix4::container_t>::container;
    using container<matrix4::container_t>::operator=;

    SIGHT_DECLARE_CLASS(matrix4, container<matrix4::container_t>);

    /// Constructors
    /// @{
    constexpr matrix4() noexcept;
    inline matrix4(std::initializer_list<value_type> _init_list);

    template<
        typename T,
        typename = std::enable_if_t<
            core::is_container_ordered<T>::value
            || std::is_base_of_v<std::initializer_list<value_type>, T>
        >
    >
    inline matrix4(const T& _data);
    /// @}

    //! @brief destructor
    SIGHT_DATA_API ~matrix4() noexcept final = default;

    /// Assignment operator
    /// @{
    inline matrix4& operator=(std::initializer_list<value_type> _init_list);

    template<
        typename T,
        typename = std::enable_if_t<
            core::is_container_ordered<T>::value
            || std::is_base_of_v<std::initializer_list<value_type>, T>
        >
    >
    inline matrix4& operator=(const T& _data);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the coefficient in the given position (matrix[l][c])
     */
    [[nodiscard]] constexpr matrix4::value_type& operator()(std::size_t _l, std::size_t _c) noexcept;
    [[nodiscard]] constexpr const matrix4::value_type& operator()(std::size_t _l, std::size_t _c) const noexcept;
    /// @}

    /// maximum size of the matrix (MATRIX_SIZE x MATRIX_SIZE)
    static constexpr std::size_t MATRIX_SIZE = 4;

    /// Print the coefficients of the matrix
    friend std::ostream& operator<<(std::ostream& _s, const matrix4& _mat)
    {
        for(std::size_t l = 0 ; l < MATRIX_SIZE ; l++)
        {
            for(std::size_t c = 0 ; c < MATRIX_SIZE ; c++)
            {
                _s << _mat(l, c) << "\t";
            }

            _s << std::endl;
        }

        return _s;
    }

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    SIGHT_DATA_API void shallow_copy(const object::csptr& _source) final;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    SIGHT_DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) final;

    /// Equality comparison operators
    /// @{
    constexpr bool operator==(const matrix4& _other) const noexcept;
    constexpr bool operator!=(const matrix4& _other) const noexcept;

    template<
        typename T,
        typename = std::enable_if_t<
            core::is_container_ordered<T>::value
            || std::is_base_of_v<std::initializer_list<value_type>, T>
        >
    >
    constexpr bool operator==(const T& _other) const noexcept;

    template<
        typename T,
        typename = std::enable_if_t<
            core::is_container_ordered<T>::value
            || std::is_base_of_v<std::initializer_list<value_type>, T>
        >
    >
    constexpr bool operator!=(const T& _other) const noexcept;
    /// @}

    [[nodiscard]] static constexpr container_t identity() noexcept;
    [[nodiscard]] constexpr bool is_identity() noexcept;

    /// Convert the matrix to any "ordered" container type.
    template<
        typename T = container_t,
        typename = std::enable_if_t<core::is_container_ordered<T>::value>
    >
    [[nodiscard]] constexpr T values() const noexcept;

    /// Helper function to set the orientation (6-9 cosines) from a container
    /// @param [in] _orientation in a container
    template<typename T, typename = std::enable_if_t<core::is_container_ordered<T>::value> >
    inline void set_orientation(const T& _orientation);

    /// Helper function to return the orientation part of the matrix (6-9 cosines) to a container
    /// @param [in] _full (default=true) to return the full orientation (9 cosines), false for the 6 first, which is
    ///                   common when dealing with DICOM
    template<
        typename T = container_t,
        typename = std::enable_if_t<core::is_container_ordered<T>::value>
    >
    [[nodiscard]] constexpr T orientation(bool _full = true) const noexcept;

    /// Helper function to set the position from a container
    /// @param [in] _position in DICOM format
    template<typename T, typename = std::enable_if_t<core::is_container_ordered<T>::value> >
    inline void set_position(const T& _position);

    /// Helper function to return the position part of the matrix (3 doubles) to a container
    template<
        typename T = container_t,
        typename = std::enable_if_t<core::is_container_ordered<T>::value>
    >
    [[nodiscard]] constexpr T position() const noexcept;

protected:

    static constexpr container_t IDENTITY = {
        1., 0., 0., 0.,
        0., 1., 0., 0.,
        0., 0., 1., 0.,
        0., 0., 0., 1.
    };
};

constexpr matrix4::matrix4() noexcept
{
    *this = IDENTITY;
}

//------------------------------------------------------------------------------

inline matrix4::matrix4(std::initializer_list<value_type> _init_list)
{
    this->operator=<std::initializer_list<value_type> >(_init_list);
}

//------------------------------------------------------------------------------

inline matrix4& matrix4::operator=(std::initializer_list<value_type> _init_list)
{
    this->operator=<std::initializer_list<value_type> >(_init_list);
    return *this;
}

//------------------------------------------------------------------------------

template<typename T, typename enable>
inline matrix4::matrix4(const T& _data)
{
    ///@todo make this functions constexpr once we support C++23 which will support SIGHT_ASSERT
    SIGHT_ASSERT(
        std::string(__func__) + " need 16 elements: " + std::to_string(_data.size()) + " given.",
        _data.size() == this->size()
    );

    std::copy(_data.begin(), _data.end(), this->begin());
}

//------------------------------------------------------------------------------

template<typename T, typename enable>
inline matrix4& matrix4::operator=(const T& _data)
{
    ///@todo make this functions constexpr once we support C++23 which will support SIGHT_ASSERT
    SIGHT_ASSERT(
        std::string(__func__) + " need 16 elements: " + std::to_string(_data.size()) + " given.",
        _data.size() == this->size()
    );

    std::copy(_data.begin(), _data.end(), this->begin());
    return *this;
}

//------------------------------------------------------------------------------

template<typename T, typename enable>
constexpr bool matrix4::operator==(const T& _other) const noexcept
{
    return core::is_equal(*this, _other);
}

//------------------------------------------------------------------------------

template<typename T, typename enable>
constexpr bool matrix4::operator!=(const T& _other) const noexcept
{
    return !(*this == _other);
}

//------------------------------------------------------------------------------

template<typename T, typename enable>
[[nodiscard]] constexpr T matrix4::values() const noexcept
{
    T values;

    if constexpr(!core::is_container_dynamic<T>::value)
    {
        std::copy(this->cbegin(), this->cend(), values.begin());
    }
    else
    {
        values.reserve(this->size());
        std::copy(this->cbegin(), this->cend(), std::back_inserter(values));
    }

    return values;
}

//------------------------------------------------------------------------------

template<typename T, typename enable>
inline void matrix4::set_orientation(const T& _orientation)
{
    ///@todo make this functions constexpr once we support C++23 which will support SIGHT_ASSERT
    SIGHT_ASSERT(
        std::string(__func__) + " need either 6 or 9 elements: " + std::to_string(_orientation.size()) + " given.",
        _orientation.size() == 6 || _orientation.size() == 9
    );

    if(_orientation.size() == 6)
    {
        const glm::dvec3 u(_orientation[0], _orientation[1], _orientation[2]);
        const glm::dvec3 v(_orientation[3], _orientation[4], _orientation[5]);

        // Cross product to get the missing part
        const glm::dvec3 w = glm::cross(u, v);

        *this = {
            _orientation[0], _orientation[3], w.x, (*this)(0, 3),
            _orientation[1], _orientation[4], w.y, (*this)(1, 3),
            _orientation[2], _orientation[5], w.z, (*this)(2, 3),
            0.0, 0.0, 0.0, 1.0
        };
    }
    else
    {
        *this = {
            _orientation[0], _orientation[3], _orientation[6], (*this)(0, 3),
            _orientation[1], _orientation[4], _orientation[7], (*this)(1, 3),
            _orientation[2], _orientation[5], _orientation[8], (*this)(2, 3),
            0.0, 0.0, 0.0, 1.0
        };
    }
}

//------------------------------------------------------------------------------

template<typename T, typename enable>
[[nodiscard]] constexpr T matrix4::orientation(bool _full) const noexcept
{
    if(_full)
    {
        return T {
            (*this)(0, 0), (*this)(1, 0), (*this)(2, 0),
            (*this)(0, 1), (*this)(1, 1), (*this)(2, 1),
            (*this)(0, 2), (*this)(1, 2), (*this)(2, 2)
        };
    }

    return T {
        (*this)(0, 0), (*this)(1, 0), (*this)(2, 0),
        (*this)(0, 1), (*this)(1, 1), (*this)(2, 1)
    };
}

//------------------------------------------------------------------------------

template<typename T, typename enable>
inline void matrix4::set_position(const T& _position)
{
    ///@todo make this functions constexpr once we support C++23 which will support SIGHT_ASSERT
    SIGHT_ASSERT(
        std::string(__func__) + " need 3 elements: " + std::to_string(_position.size()) + " given.",
        _position.size() == 3
    );

    (*this)(0, 3) = _position[0];
    (*this)(1, 3) = _position[1];
    (*this)(2, 3) = _position[2];
}

//------------------------------------------------------------------------------

template<typename T, typename enable>
[[nodiscard]] constexpr T matrix4::position() const noexcept
{
    return T {(*this)(0, 3), (*this)(1, 3), (*this)(2, 3)};
}

//------------------------------------------------------------------------------

constexpr matrix4::value_type& matrix4::operator()(std::size_t _l, std::size_t _c) noexcept
{
    const std::size_t pos = _l * MATRIX_SIZE + _c;
    return this->at(pos);
}

//------------------------------------------------------------------------------

constexpr const matrix4::value_type& matrix4::operator()(std::size_t _l, std::size_t _c) const noexcept
{
    const std::size_t pos = _l * MATRIX_SIZE + _c;
    return this->at(pos);
}

//-----------------------------------------------------------------------------

constexpr matrix4::container_t matrix4::identity() noexcept
{
    return matrix4::IDENTITY;
}

//------------------------------------------------------------------------------

constexpr bool matrix4::is_identity() noexcept
{
    return this->operator==(matrix4::IDENTITY);
}

//------------------------------------------------------------------------------

constexpr bool matrix4::operator==(const matrix4& _other) const noexcept
{
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

constexpr bool matrix4::operator!=(const matrix4& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
