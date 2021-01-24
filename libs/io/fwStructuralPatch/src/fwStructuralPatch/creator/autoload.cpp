/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "fwStructuralPatch/creator/data/ActivitySeries1.hpp"
#include "fwStructuralPatch/creator/data/Equipment1.hpp"
#include "fwStructuralPatch/creator/data/ImageSeries1.hpp"
#include "fwStructuralPatch/creator/data/Landmarks1.hpp"
#include "fwStructuralPatch/creator/data/ModelSeries1.hpp"
#include "fwStructuralPatch/creator/data/Patient1.hpp"
#include "fwStructuralPatch/creator/data/PointList1.hpp"
#include "fwStructuralPatch/creator/data/Study1.hpp"

#include <fwAtomsPatch/StructuralCreatorDB.hpp>

namespace fwStructuralPatch
{
namespace creator
{

/// Registers structural creators dedicated to data creation.
struct runner
{
    runner()
    {
        ::fwAtomsPatch::StructuralCreatorDB::sptr creators = ::fwAtomsPatch::StructuralCreatorDB::getDefault();
        creators->registerCreator(::fwStructuralPatch::creatordata::Equipment1::New());
        creators->registerCreator(::fwStructuralPatch::creatordata::Patient1::New());
        creators->registerCreator(::fwStructuralPatch::creatordata::Study1::New());
        creators->registerCreator(::fwStructuralPatch::creatordata::ModelSeries1::New());
        creators->registerCreator(::fwStructuralPatch::creatordata::ImageSeries1::New());
        creators->registerCreator(::fwStructuralPatch::creatordata::ActivitySeries1::New());
        creators->registerCreator(::fwStructuralPatch::creatordata::ActivitySeries1::New());
        creators->registerCreator(::fwStructuralPatch::creatordata::Landmarks1::New());
        creators->registerCreator(::fwStructuralPatch::creatordata::PointList1::New());
    }

    static runner r;
};

runner runner::r;

} // namespace creator
} // namespace fwStructuralPatch
