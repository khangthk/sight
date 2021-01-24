/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "fwDicomIOFilter/config.hpp"
#include "fwDicomIOFilter/IFilter.hpp"

#include <data/DicomSeries.hpp>

namespace fwDicomIOFilter
{
namespace splitter
{

/**
 * @brief Base class for Dicom instance splitter.
 */
class FWDICOMIOFILTER_CLASS_API ISplitter : public ::fwDicomIOFilter::IFilter
{
public:
    fwCoreClassMacro(ISplitter, ::fwDicomIOFilter::IFilter)

    /// Destructor
    FWDICOMIOFILTER_API virtual ~ISplitter();

    /// Return filter type
    FWDICOMIOFILTER_API FilterType getFilterType() const override;

protected:
    /// Constructor
    FWDICOMIOFILTER_API ISplitter();
};

} // namespace splitter
} // namespace fwDicomIOFilter
