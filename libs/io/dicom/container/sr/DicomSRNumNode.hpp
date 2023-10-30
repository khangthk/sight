/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "io/dicom/config.hpp"
#include "io/dicom/container/DicomCodedAttribute.hpp"
#include "io/dicom/container/sr/DicomSRNode.hpp"

#include <gdcmDataSet.h>

namespace sight::io::dicom::container::sr
{

/**
 * @brief This class defines a NUM node in a SR document
 */
class IO_DICOM_CLASS_API dicom_sr_num_node : public io::dicom::container::sr::dicom_sr_node
{
public:

    /// Constructor
    IO_DICOM_API dicom_sr_num_node(
        const dicom_coded_attribute& _coded_attribute,
        const std::string& _relationship,
        double _num_value,
        dicom_coded_attribute _measurement_units
    );

    /// Destructor
    IO_DICOM_API ~dicom_sr_num_node() override;

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    IO_DICOM_API void write(gdcm::DataSet& _dataset) const override;

    /// Get Numeric value
    [[nodiscard]] const double& get_num_value() const
    {
        return m_num_value;
    }

    ///Set Numeric value
    void set_num_value(const double& _num_value)
    {
        m_num_value = _num_value;
    }

    /// Get Measurement units
    [[nodiscard]] const dicom_coded_attribute& get_measurement_units() const
    {
        return m_measurement_units;
    }

    ///Set Measurement units
    void set_measurement_units(const dicom_coded_attribute& _measurement_units)
    {
        m_measurement_units = _measurement_units;
    }

protected:

    /// Dump function
    void print(std::ostream& _os) const override;

    /**
     * @brief Write a Measured Value Sequence
     * @see PS 3.3  Table C.18.1
     * @param[in] dataset Destination dataset
     */
    void write_measured_value_sequence(gdcm::DataSet& _dataset) const;

    /// Numeric Value
    double m_num_value;

    /// Measurement Units
    dicom_coded_attribute m_measurement_units;
};

} // namespace sight::io::dicom::container::sr
