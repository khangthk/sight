/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "geometry/data/Mesh.hpp"

#include "data/thread/RegionThreader.hpp"

#include "geometry/data/Matrix4.hpp"

#include <core/com/Signal.hxx>
#include <core/tools/NumericRoundCast.hxx>
#include <core/tools/random/Generator.hpp>

#include <geometry/data/MeshFunctions.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <boost/multi_array/multi_array_ref.hpp>

#include <cstdlib>
#include <ctime>
#include <functional>

namespace sight::geometry::data
{

using core::tools::random::safeRand;
using namespace sight::data::iterator;

struct RandFloat
{
    //------------------------------------------------------------------------------

    float operator()()
    {
        return ((static_cast<float>(safeRand() % 101) - 50.f)) / 500.f;
    }
};

//------------------------------------------------------------------------------

Vector<float> computeTriangleNormal(const Point& p1, const Point& p2, const Point& p3)
{
    Vector<float> n(p1, p2);
    Vector<float> v(p1, p3);
    n.crossWith(v);
    n.normalize();
    return n;
}

//------------------------------------------------------------------------------

void generateRegionCellNormals(
    const sight::data::Mesh::sptr& mesh,
    const sight::data::Mesh::cell_t regionMin,
    const sight::data::Mesh::cell_t regionMax
)
{
    switch(mesh->getCellType())
    {
        case sight::data::Mesh::CellType::POINT:
        case sight::data::Mesh::CellType::LINE:
        {
            auto cellRange = mesh->range<cell::nxyz>();
            std::fill(cellRange.begin(), cellRange.end(), cell::nxyz({0.f, 0.f, 0.f}));
            break;
        }

        case sight::data::Mesh::CellType::TRIANGLE:
        {
            const auto pointBegin = mesh->cbegin<point::xyz>();

            auto cellRange     = mesh->zip_range<cell::triangle, cell::nxyz>();
            auto cellBegin     = cellRange.begin() + regionMin;
            const auto cellEnd = cellRange.begin() + regionMax;

            int i = 0;
            std::for_each(
                cellBegin,
                cellEnd,
                [&](auto&& it)
                {
                    i++;
                    auto&& [cell, normal] = it;
                    auto pItr             = pointBegin + cell.pt[0];
                    const Point p1(pItr->x, pItr->y, pItr->z);
                    pItr = pointBegin + cell.pt[1];
                    const Point p2(pItr->x, pItr->y, pItr->z);
                    pItr = pointBegin + cell.pt[2];
                    const Point p3(pItr->x, pItr->y, pItr->z);
                    auto n = computeTriangleNormal(p1, p2, p3);

                    normal.nx = n.x;
                    normal.ny = n.y;
                    normal.nz = n.z;
                });
            break;
        }

        case sight::data::Mesh::CellType::QUAD:
        case sight::data::Mesh::CellType::TETRA:
        {
            const auto pointBegin = mesh->cbegin<point::xyz>();

            auto cellRange     = mesh->zip_range<cell::quad, cell::nxyz>();
            auto cellBegin     = cellRange.begin() + regionMin;
            const auto cellEnd = cellRange.begin() + regionMax;

            std::for_each(
                cellBegin,
                cellEnd,
                [&](auto&& it)
                {
                    Vector<float> n;
                    auto&& [cell, normal] = it;

                    for(size_t i = 0 ; i < 4 ; ++i)
                    {
                        Vector<float> v;
                        auto pItr = pointBegin + cell.pt[i];
                        const Point p1(pItr->x, pItr->y, pItr->z);
                        pItr = pointBegin + cell.pt[(i + 1) % 4];
                        const Point p2(pItr->x, pItr->y, pItr->z);
                        pItr = pointBegin + cell.pt[(i + 2) % 4];
                        const Point p3(pItr->x, pItr->y, pItr->z);

                        v = computeTriangleNormal(p1, p2, p3);

                        n += v;
                    }

                    n /= core::tools::numericRoundCast<float>(4);
                    n.normalize();
                    normal.nx = n.x;
                    normal.ny = n.y;
                    normal.nz = n.z;
                });
        }
    }
}

//------------------------------------------------------------------------------

template<typename T>
void vectorSum(std::vector<std::vector<T> >& vectors, size_t regionMin, size_t regionMax)
{
    if(vectors.empty())
    {
        return;
    }

    typename std::vector<std::vector<T> >::iterator vIter = vectors.begin();

    std::vector<T>& res = vectors[0];

    for(++vIter ; vIter != vectors.end() ; ++vIter)
    {
        for(size_t i = regionMin ; i < regionMax ; ++i)
        {
            res[i] += (*vIter)[i];
        }
    }
}

//------------------------------------------------------------------------------

void Mesh::generateCellNormals(sight::data::Mesh::sptr mesh)
{
    const sight::data::Mesh::size_t numberOfCells = mesh->numCells();
    if(numberOfCells > 0)
    {
        if(!mesh->has<sight::data::Mesh::Attributes::CELL_NORMALS>())
        {
            mesh->resize(
                mesh->numPoints(),
                mesh->numCells(),
                mesh->getCellType(),
                sight::data::Mesh::Attributes::CELL_NORMALS
            );
        }

        const auto dumpLock = mesh->lock();

        sight::data::thread::RegionThreader rt((numberOfCells >= 200000) ? 4 : 1);
        rt(
            std::bind(&generateRegionCellNormals, mesh, std::placeholders::_1, std::placeholders::_2),
            numberOfCells
        );
    }
}

//------------------------------------------------------------------------------

typedef std::vector<std::vector<float> > FloatVectors;

//------------------------------------------------------------------------------

void generateRegionCellNormalsByPoints(
    FloatVectors& normalsData,
    size_t dataId,
    const sight::data::Mesh::sptr& mesh,
    const sight::data::Mesh::cell_t regionMin,
    const sight::data::Mesh::cell_t regionMax
)
{
    FloatVectors::value_type& normalsResults = normalsData[dataId];

    const sight::data::Mesh::size_t nbOfPoints = mesh->numPoints();
    normalsResults.resize(3 * nbOfPoints, 0.f);

    auto accumNormal = [&](const auto& cell, const auto& normal)
                       {
                           normalsData[dataId][3 * cell]     += normal.nx;
                           normalsData[dataId][3 * cell + 1] += normal.ny;
                           normalsData[dataId][3 * cell + 2] += normal.nz;
                       };

    switch(mesh->getCellType())
    {
        case sight::data::Mesh::CellType::LINE:
        {
            const auto range = mesh->czip_range<cell::line, cell::nxyz>();
            auto begin       = range.begin() + regionMin;
            const auto end   = range.begin() + regionMax;
            std::for_each(
                begin,
                end,
                [&](const auto& it)
                {
                    const auto& [line, n] = it;
                    accumNormal(line.pt[0], n);
                    accumNormal(line.pt[1], n);
                });
            break;
        }

        case sight::data::Mesh::CellType::TRIANGLE:
        {
            const auto range = mesh->czip_range<cell::triangle, cell::nxyz>();
            auto begin       = range.begin() + regionMin;
            const auto end   = range.begin() + regionMax;
            std::for_each(
                begin,
                end,
                [&](const auto& it)
                {
                    const auto& [cell, n] = it;
                    accumNormal(cell.pt[0], n);
                    accumNormal(cell.pt[1], n);
                    accumNormal(cell.pt[2], n);
                });
            break;
        }

        case sight::data::Mesh::CellType::QUAD:
        case sight::data::Mesh::CellType::TETRA:
        {
            const auto range = mesh->czip_range<cell::quad, cell::nxyz>();
            auto begin       = range.begin() + regionMin;
            const auto end   = range.begin() + regionMax;
            std::for_each(
                begin,
                end,
                [&](const auto& it)
                {
                    const auto& [cell, n] = it;
                    accumNormal(cell.pt[0], n);
                    accumNormal(cell.pt[1], n);
                    accumNormal(cell.pt[2], n);
                    accumNormal(cell.pt[3], n);
                });
            break;
        }
    }
}

//------------------------------------------------------------------------------

void normalizeRegionCellNormalsByPoints(
    FloatVectors::value_type& normalsData,
    sight::data::Mesh::sptr mesh,
    const sight::data::Mesh::cell_t regionMin,
    const sight::data::Mesh::cell_t regionMax
)
{
    Vector<sight::data::Mesh::normal_t>* normalSum =
        reinterpret_cast<Vector<sight::data::Mesh::normal_t>*>(&(*normalsData.begin()));

    auto pointItr = mesh->begin<point::nxyz>() + regionMin;

    for(sight::data::Mesh::cell_t i = regionMin ; i < regionMax ; ++i, ++pointItr)
    {
        Vector<sight::data::Mesh::normal_t> normal = normalSum[i];

        normal.normalize();
        pointItr->nx = normal.x;
        pointItr->ny = normal.y;
        pointItr->nz = normal.z;
    }
}

//------------------------------------------------------------------------------

void Mesh::generatePointNormals(sight::data::Mesh::sptr mesh)
{
    const sight::data::Mesh::size_t nbOfPoints = mesh->numPoints();
    if(nbOfPoints > 0)
    {
        const sight::data::Mesh::size_t numberOfCells = mesh->numCells();

        // To generate point normals, we need to use the cell normals
        if(!mesh->has<sight::data::Mesh::Attributes::CELL_NORMALS>())
        {
            generateCellNormals(mesh);
        }

        if(!mesh->has<sight::data::Mesh::Attributes::POINT_NORMALS>())
        {
            mesh->resize(
                mesh->numPoints(),
                mesh->numCells(),
                mesh->getCellType(),
                sight::data::Mesh::Attributes::POINT_NORMALS
            );
        }

        const auto dumpLock = mesh->lock();

        sight::data::thread::RegionThreader rt((nbOfPoints >= 100000) ? 4 : 1);

        FloatVectors normalsData(rt.numberOfThread());

        rt(
            std::bind(
                &generateRegionCellNormalsByPoints,
                std::ref(normalsData),
                std::placeholders::_3,
                mesh,
                std::placeholders::_1,
                std::placeholders::_2
            ),
            numberOfCells
        );

        rt(
            std::bind(
                &vectorSum<FloatVectors::value_type::value_type>,
                std::ref(normalsData),
                std::placeholders::_1,
                std::placeholders::_2
            ),
            nbOfPoints * 3
        );

        rt(
            std::bind(
                &normalizeRegionCellNormalsByPoints,
                std::ref(normalsData[0]),
                mesh,
                std::placeholders::_1,
                std::placeholders::_2
            ),
            nbOfPoints
        );
    }
}

//------------------------------------------------------------------------------

template<typename T>
void regionShakeNormals(T normals, const sight::data::Mesh::cell_t regionMin, const sight::data::Mesh::cell_t regionMax)
{
    RandFloat randFloat;
    for(sight::data::Mesh::cell_t i = regionMin ; i < regionMax ; ++i)
    {
        Vector<float> v(randFloat(), randFloat(), randFloat());
        normals[i] += v;
        normals[i].normalize();
    }
}

//------------------------------------------------------------------------------

void Mesh::shakeNormals(sight::data::Array::sptr array)
{
    if(array
       && array->getType() == core::tools::Type::create<float>()
       && !array->empty()
       && array->numDimensions() == 2)
    {
        const auto dumpLock = array->lock();
        void* buf;
        buf = array->getBuffer();
        const auto nbOfNormals = array->getSize().at(0);
        typedef boost::multi_array_ref<Vector<float>, 1> NormalsMultiArrayType;
        NormalsMultiArrayType normals = NormalsMultiArrayType(
            static_cast<NormalsMultiArrayType::element*>(buf),
            ::boost::extents[nbOfNormals]
        );

        sight::data::thread::RegionThreader rt((nbOfNormals >= 150000) ? 4 : 1);
        rt(
            std::bind(
                &regionShakeNormals<NormalsMultiArrayType>,
                std::ref(normals),
                std::placeholders::_1,
                std::placeholders::_2
            ),
            nbOfNormals
        );
    }
}

//------------------------------------------------------------------------------

void Mesh::shakePointNormals(sight::data::Mesh::sptr mesh)
{
    const auto dumpLock = mesh->lock();

    RandFloat randFloat;

    for(auto& n : mesh->range<point::nxyz>())
    {
        Vector<float> v(randFloat(), randFloat(), randFloat());
        Vector<float> normal(n.nx, n.ny, n.nz);
        normal += v;
        normal.normalize();
        n.nx = normal.x;
        n.ny = normal.y;
        n.nz = normal.z;
    }
}

//------------------------------------------------------------------------------

void Mesh::shakeCellNormals(sight::data::Mesh::sptr mesh)
{
    const auto dumpLock = mesh->lock();

    RandFloat randFloat;

    for(auto& n : mesh->range<cell::nxyz>())
    {
        Vector<float> v(randFloat(), randFloat(), randFloat());
        Vector<float> normal(n.nx, n.ny, n.nz);
        normal += v;
        normal.normalize();
        n.nx = normal.x;
        n.ny = normal.y;
        n.nz = normal.z;
    }
}

//------------------------------------------------------------------------------

void Mesh::colorizeMeshPoints(sight::data::Mesh::sptr mesh)
{
    if(!mesh->has<sight::data::Mesh::Attributes::POINT_COLORS>())
    {
        mesh->resize(
            mesh->numPoints(),
            mesh->numCells(),
            mesh->getCellType(),
            sight::data::Mesh::Attributes::POINT_COLORS
        );
    }

    const auto dumpLock = mesh->lock();

    for(auto& c : mesh->range<point::rgba>())
    {
        c.r = static_cast<std::uint8_t>(safeRand() % 256);
        c.g = static_cast<std::uint8_t>(safeRand() % 256);
        c.b = static_cast<std::uint8_t>(safeRand() % 256);
        c.a = static_cast<std::uint8_t>(safeRand() % 256);
    }
}

//------------------------------------------------------------------------------

void Mesh::colorizeMeshCells(sight::data::Mesh::sptr mesh)
{
    if(!mesh->has<sight::data::Mesh::Attributes::CELL_COLORS>())
    {
        mesh->resize(
            mesh->numPoints(),
            mesh->numCells(),
            mesh->getCellType(),
            sight::data::Mesh::Attributes::CELL_COLORS
        );
    }

    const auto dumpLock = mesh->lock();

    for(auto& c : mesh->range<cell::rgba>())
    {
        c.r = static_cast<std::uint8_t>(safeRand() % 256);
        c.g = static_cast<std::uint8_t>(safeRand() % 256);
        c.b = static_cast<std::uint8_t>(safeRand() % 256);
        c.a = static_cast<std::uint8_t>(safeRand() % 256);
    }
}

//------------------------------------------------------------------------------

void Mesh::shakePoint(sight::data::Mesh::sptr mesh)
{
    RandFloat randFloat;

    const auto dumpLock = mesh->lock();

    for(auto& p : mesh->range<point::xyz>())
    {
        p.x += randFloat() * 5;
        p.y += randFloat() * 5;
        p.z += randFloat() * 5;
    }
}

//------------------------------------------------------------------------------

void Mesh::transform(
    sight::data::Mesh::csptr inMesh,
    sight::data::Mesh::sptr outMesh,
    const sight::data::Matrix4& t
)
{
    const auto inDumpLock  = inMesh->lock();
    const auto outDumpLock = outMesh->lock();

    const glm::dmat4x4 matrix = sight::geometry::data::getMatrixFromTF3D(t);

    [[maybe_unused]] const size_t numPts = inMesh->numPoints();
    SIGHT_ASSERT("In and out meshes should have the same number of points", numPts == outMesh->numPoints());

    SIGHT_ASSERT(
        "in and out meshes must have the same point normals attribute",
        (inMesh->has<sight::data::Mesh::Attributes::POINT_NORMALS>()
         && outMesh->has<sight::data::Mesh::Attributes::POINT_NORMALS>())
        || (!inMesh->has<sight::data::Mesh::Attributes::POINT_NORMALS>()
            && !outMesh->has<sight::data::Mesh::Attributes::POINT_NORMALS>())
    );

    for(auto&& [in, out] : boost::combine(inMesh->crange<point::xyz>(), outMesh->range<point::xyz>()))
    {
        const glm::vec4 pt(in.x, in.y, in.z, 1.);
        const glm::vec4 transformedPt = matrix * pt;

        out.x = transformedPt.x;
        out.y = transformedPt.y;
        out.z = transformedPt.z;
    }

    if(inMesh->has<sight::data::Mesh::Attributes::POINT_NORMALS>())
    {
        for(auto&& [in, out] : boost::combine(inMesh->crange<point::nxyz>(), outMesh->range<point::nxyz>()))
        {
            const glm::vec4 normal(in.nx, in.ny, in.nz, 0.);
            const glm::vec4 transformedNormal = glm::normalize(matrix * normal);

            out.nx = transformedNormal.x;
            out.ny = transformedNormal.y;
            out.nz = transformedNormal.z;
        }
    }

    if(inMesh->has<sight::data::Mesh::Attributes::CELL_NORMALS>())
    {
        SIGHT_ASSERT("out mesh must have normals", outMesh->has<sight::data::Mesh::Attributes::CELL_NORMALS>());
        for(auto&& [in, out] : boost::combine(inMesh->crange<cell::nxyz>(), outMesh->range<cell::nxyz>()))
        {
            const glm::vec4 normal(in.nx, in.ny, in.nz, 0.);
            const glm::vec4 transformedNormal = glm::normalize(matrix * normal);

            out.nx = transformedNormal.x;
            out.ny = transformedNormal.y;
            out.nz = transformedNormal.z;
        }
    }
}

//------------------------------------------------------------------------------

void Mesh::transform(sight::data::Mesh::sptr mesh, const sight::data::Matrix4& t)
{
    Mesh::transform(mesh, mesh, t);
}

//------------------------------------------------------------------------------

void Mesh::colorizeMeshPoints(
    const sight::data::Mesh::sptr& mesh,
    const std::uint8_t colorR,
    const std::uint8_t colorG,
    const std::uint8_t colorB,
    const std::uint8_t colorA
)
{
    const auto dumpLock = mesh->lock();

    SIGHT_ASSERT("color array must be allocated", mesh->has<sight::data::Mesh::Attributes::POINT_COLORS>());

    for(auto& c : mesh->range<point::rgba>())
    {
        c.r = colorR;
        c.g = colorG;
        c.b = colorB;
        c.a = colorA;
    }

    auto sig = mesh->signal<sight::data::Mesh::signal_t>(sight::data::Mesh::s_POINT_COLORS_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void Mesh::colorizeMeshPoints(
    const sight::data::Mesh::sptr& _mesh,
    const std::vector<size_t>& _vectorNumTriangle,
    const std::uint8_t _colorR,
    const std::uint8_t _colorG,
    const std::uint8_t _colorB,
    const std::uint8_t _colorA
)
{
    const auto dumpLock = _mesh->lock();

    auto itrCell = _mesh->begin<cell::triangle>();

    auto itrPoint = _mesh->begin<point::rgba>();

    for(size_t index : _vectorNumTriangle)
    {
        auto cell                        = itrCell + static_cast<std::ptrdiff_t>(index);
        const std::ptrdiff_t indexPoint0 = static_cast<std::ptrdiff_t>(cell->pt[0]);
        const std::ptrdiff_t indexPoint1 = static_cast<std::ptrdiff_t>(cell->pt[1]);
        const std::ptrdiff_t indexPoint2 = static_cast<std::ptrdiff_t>(cell->pt[2]);

        auto point1 = itrPoint + indexPoint0;
        auto point2 = itrPoint + indexPoint1;
        auto point3 = itrPoint + indexPoint2;

        point1->r = _colorR;
        point1->g = _colorG;
        point1->b = _colorB;
        point1->a = _colorA;

        point2->r = _colorR;
        point2->g = _colorG;
        point2->b = _colorB;
        point2->a = _colorA;

        point3->r = _colorR;
        point3->g = _colorG;
        point3->b = _colorB;
        point3->a = _colorA;
    }

    auto sig = _mesh->signal<sight::data::Mesh::signal_t>(sight::data::Mesh::s_POINT_COLORS_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void Mesh::colorizeMeshCells(
    const sight::data::Mesh::sptr& mesh,
    const std::uint8_t colorR,
    const std::uint8_t colorG,
    const std::uint8_t colorB,
    const std::uint8_t colorA
)
{
    const auto dumpLock = mesh->lock();

    SIGHT_ASSERT("color array must be allocated", mesh->has<sight::data::Mesh::Attributes::CELL_COLORS>());

    for(auto& c : mesh->range<cell::rgba>())
    {
        c.r = colorR;
        c.g = colorG;
        c.b = colorB;
        c.a = colorA;
    }

    auto sig = mesh->signal<sight::data::Mesh::signal_t>(sight::data::Mesh::s_CELL_COLORS_MODIFIED_SIG);
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

void Mesh::colorizeMeshCells(
    const sight::data::Mesh::sptr& mesh,
    const std::vector<size_t>& triangleIndexVector,
    const std::uint8_t colorR,
    const std::uint8_t colorG,
    const std::uint8_t colorB,
    const std::uint8_t colorA
)
{
    const auto dumpLock = mesh->lock();

    auto itrCell = mesh->begin<cell::rgba>();

    for(size_t index : triangleIndexVector)
    {
        auto cell = itrCell + static_cast<std::ptrdiff_t>(index);

        cell->r = colorR;
        cell->g = colorG;
        cell->b = colorB;
        cell->a = colorA;
    }

    auto sig = mesh->signal<sight::data::Mesh::signal_t>(sight::data::Mesh::s_CELL_COLORS_MODIFIED_SIG);
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

bool Mesh::isClosed(const sight::data::Mesh::csptr& mesh)
{
    bool isClosed = true;

    typedef std::pair<sight::data::Mesh::cell_t, sight::data::Mesh::cell_t> Edge;
    typedef std::map<Edge, int> EdgeHistogram;
    EdgeHistogram edgesHistogram;

    const auto dumpLock = mesh->lock();

    auto addEdge = [&edgesHistogram](const sight::data::Mesh::point_t& p1, const sight::data::Mesh::point_t& p2)
                   {
                       const auto edge = geometry::data::makeOrderedPair(p1, p2);

                       if(edgesHistogram.find(edge) == edgesHistogram.end())
                       {
                           edgesHistogram[edge] = 1;
                       }
                       else
                       {
                           ++edgesHistogram[edge];
                       }
                   };

    switch(mesh->getCellType())
    {
        case sight::data::Mesh::CellType::LINE:
        {
            for(auto& line : mesh->crange<cell::line>())
            {
                addEdge(line.pt[0], line.pt[1]);
            }

            break;
        }

        case sight::data::Mesh::CellType::TRIANGLE:
        {
            for(auto& cell : mesh->crange<cell::triangle>())
            {
                addEdge(cell.pt[0], cell.pt[1]);
                addEdge(cell.pt[1], cell.pt[2]);
                addEdge(cell.pt[2], cell.pt[0]);
            }

            break;
        }

        case sight::data::Mesh::CellType::QUAD:
        case sight::data::Mesh::CellType::TETRA:
        {
            for(auto& cell : mesh->crange<cell::quad>())
            {
                addEdge(cell.pt[0], cell.pt[1]);
                addEdge(cell.pt[1], cell.pt[2]);
                addEdge(cell.pt[2], cell.pt[3]);
                addEdge(cell.pt[3], cell.pt[0]);
            }

            break;
        }
    }

    for(const EdgeHistogram::value_type& histo : edgesHistogram)
    {
        if(histo.second != 2)
        {
            isClosed = false;
            break;
        }
    }

    return isClosed;
}

//------------------------------------------------------------------------------

} // namespace sight::geometry::data
