/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "io/itk/NiftiImageReader.hpp"

#include "factory/InrImageIOFactory.hpp"

#include "io/itk/helper/ProgressItkToFw.hpp"
#include "io/itk/itk.hpp"

#include <core/base.hpp>
#include <core/tools/Dispatcher.hpp>

#include <data/Image.hpp>

#include <io/base/reader/registry/macros.hpp>

#include <itkImageFileReader.h>
#include <itkNiftiImageIO.h>

#include <filesystem>

SIGHT_REGISTER_IO_READER(sight::io::itk::NiftiImageReader);

namespace sight::io::itk
{

//------------------------------------------------------------------------------

NiftiImageReader::NiftiImageReader(io::base::reader::IObjectReader::Key)
{
}

//------------------------------------------------------------------------------

NiftiImageReader::~NiftiImageReader()
{
}

//------------------------------------------------------------------------------

struct NiftiLoaderFunctor
{
    struct Parameter
    {
        data::Image::sptr dataImage;
        std::string filename;
        io::itk::NiftiImageReader::sptr reader;
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(Parameter& param)
    {
        SIGHT_INFO(
            "::io::itk::NiftiImageReader::NiftiLoaderFunctor with PIXELTYPE "
            << core::Type::get<PIXELTYPE>().name()
        );

        // Reader IO (*1*)
        typename ::itk::NiftiImageIO::Pointer imageIORead = ::itk::NiftiImageIO::New();
        imageIORead->SetFileName(param.filename.c_str());
        imageIORead->ReadImageInformation();

        // set observation (*2*)
        Progressor progress(imageIORead, param.reader, param.filename);

        // the reader
        typedef ::itk::Image<PIXELTYPE, 3> ImageType;
        typedef ::itk::ImageFileReader<ImageType> ReaderType;
        typename ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName(param.filename.c_str());

        // attach its IO (*3*)
        reader->SetImageIO(imageIORead);

        reader->Update();
        typename ImageType::Pointer itkimage = reader->GetOutput();

        io::itk::moveFromItk<ImageType>(itkimage, param.dataImage);
    }

    //// get pixel type from Header
    static const core::Type& getImageType(const std::string& imageFileName)
    {
        typename ::itk::NiftiImageIO::Pointer imageIO = ::itk::NiftiImageIO::New();
        imageIO->SetFileName(imageFileName.c_str());
        imageIO->ReadImageInformation();
        auto itkType = imageIO->GetComponentType();
        return sight::io::itk::ITK_TYPE_CONVERTER.at(itkType);
    }
};

//------------------------------------------------------------------------------

void NiftiImageReader::read()
{
    std::filesystem::path file = getFile();
    SIGHT_ASSERT("File: " << file << " doesn't exist", std::filesystem::exists(file));
    assert(!m_object.expired());
    assert(m_object.lock());

    const core::Type& type = NiftiLoaderFunctor::getImageType(file.string());

    NiftiLoaderFunctor::Parameter param;
    param.filename  = file.string();
    param.dataImage = this->getConcreteObject();
    param.reader    = this->getSptr();

    core::tools::Dispatcher<core::tools::IntrinsicTypes, NiftiLoaderFunctor>::invoke(type, param);

    SIGHT_ASSERT("sight::data::Image is not well produced", m_object.lock()); // verify that data::Image is well
    // produced
    // Post Condition image with a pixel type
    SIGHT_ASSERT(
        "Image has an unspecified type",
        getConcreteObject()->getType() != core::Type::NONE
    );
}

//------------------------------------------------------------------------------

} // namespace sight::io::itk