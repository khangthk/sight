/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "modules/filter/image/SMatrixRegressor.hpp"

#include <core/com/Signal.hxx>

#include <data/PointList.hpp>
#include <data/TransformationMatrix3D.hpp>
#include <data/Vector.hpp>

#include <services/macros.hpp>

#include <filter/image/MatrixRegressor.hpp>

namespace sight::modules::filter::image
{

fwServicesRegisterMacro(::sight::services::IOperator, ::sight::modules::filter::image::SMatrixRegressor, data::Vector)

static const services::IService::KeyType s_MATRIX_LIST_IN = "matrixList";
static const services::IService::KeyType s_POINT_LIST_IN        = "pointList";
static const services::IService::KeyType s_OPTIMAL_MATRIX_INOUT = "optimalMatrix";

//-----------------------------------------------------------------------------

SMatrixRegressor::SMatrixRegressor()
{

}

//-----------------------------------------------------------------------------

SMatrixRegressor::~SMatrixRegressor()
{

}

//-----------------------------------------------------------------------------

void SMatrixRegressor::configuring()
{

}

//-----------------------------------------------------------------------------

void SMatrixRegressor::starting()
{

}

//-----------------------------------------------------------------------------

void SMatrixRegressor::updating()
{
    const auto matrixList = this->getLockedInput< data::Vector >(s_MATRIX_LIST_IN);
    const auto pointList  = this->getLockedInput< data::PointList >(s_POINT_LIST_IN);

    SLM_ASSERT("'matrixList' does not exist", matrixList);
    SLM_ASSERT("'pointList' does not exist", pointList);

    const auto optimalMatrix = this->getLockedInOut< data::TransformationMatrix3D >(s_OPTIMAL_MATRIX_INOUT);

    SLM_ASSERT("'optimalMatrix' does not exist", optimalMatrix);

    std::vector< sight::filter::image::MatrixRegressor::PointType > ptList;

    // Convert the point list.
    for(const auto& pt : pointList->getPoints())
    {
        const auto& ptCoords = pt->getCoord();
        ptList.push_back( sight::filter::image::MatrixRegressor::PointType(ptCoords[0], ptCoords[1], ptCoords[2], 1.));
    }

    if(!matrixList->empty() && !ptList.empty())
    {
        sight::filter::image::MatrixRegressor regressor(matrixList.get_shared(), ptList);

        data::TransformationMatrix3D::csptr initVal =
            data::TransformationMatrix3D::dynamicCast((*matrixList)[0]);

        data::TransformationMatrix3D::sptr res = regressor.minimize(initVal, 1., 1e-4, 1e-4);
        optimalMatrix->deepCopy(res);

        m_sigComputed->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void SMatrixRegressor::stopping()
{

}

//-----------------------------------------------------------------------------

services::IService::KeyConnectionsMap SMatrixRegressor::getAutoConnections() const
{
    services::IService::KeyConnectionsMap connections;
    connections.push(s_MATRIX_LIST_IN, data::Vector::s_ADDED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push(s_MATRIX_LIST_IN, data::Vector::s_REMOVED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push(s_MATRIX_LIST_IN, data::Vector::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_POINT_LIST_IN, data::PointList::s_POINT_ADDED_SIG, s_UPDATE_SLOT);
    connections.push(s_POINT_LIST_IN, data::PointList::s_POINT_REMOVED_SIG, s_UPDATE_SLOT);
    connections.push(s_POINT_LIST_IN, data::PointList::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

} // namespace sight::modules::filter::image.
