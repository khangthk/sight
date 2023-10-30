/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "io/dicom/container/sr/DicomSRNode.hpp"
#include "io/dicom/writer/tid/TemplateID.hpp"

#include <data/image.hpp>

namespace sight::data
{

class point;

} // namespace sight::data

namespace sight::io::dicom::writer::tid
{

/**
 * @brief Fiducial TID
 * @see TID t1tt3 FIXME: Set the final TID
 */
class IO_DICOM_CLASS_API fiducial : public io::dicom::writer::tid::template_id<data::image>
{
public:

    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] image Image data
     */
    IO_DICOM_API fiducial(
        const SPTR(gdcm::Writer)& _writer,
        const SPTR(io::dicom::container::dicom_instance)& _instance,
        const data::image::csptr& _image
    );

    /// Destructor
    IO_DICOM_API ~fiducial() override;

    /**
     * @brief Create nodes according to the template
     * @param[in] parent Parent node
     * @param[in] useSCoord3D True if we must write SCOORD3D, false if we must write SCOORD
     */
    IO_DICOM_API virtual void create_nodes(
        const SPTR(io::dicom::container::sr::dicom_sr_node)& _parent,
        bool _use_s_coord_3d = true
    );

protected:

    /**
     * @brief Create a fiducial node
     * @param[in] parent Parent node
     * @param[in] point Fiducial (landmark) point
     * @param[in] id ID of the fiducial
     * @param[in] useSCoord3D True if we must use 3D coordinates
     */
    void create_fiducial(
        const SPTR(io::dicom::container::sr::dicom_sr_node)& _parent,
        const CSPTR(data::point)& _point,
        unsigned int _id,
        bool _use_s_coord_3d
    );
};

} // namespace sight::io::dicom::writer::tid
