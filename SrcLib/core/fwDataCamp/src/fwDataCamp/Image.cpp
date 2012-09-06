
#include "fwDataCamp/Factory.hpp"
#include <fwTools/camp/mapper.hpp>
#include "fwDataCamp/Image.hpp"


fwCampImplementDataMacro((fwData)(Image))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("size", &::fwData::Image::m_size)
        .property("type", &::fwData::Image::m_type)
        .property("spacing", &::fwData::Image::m_spacing)
        .property("origin", &::fwData::Image::m_origin)
        .property("array", &::fwData::Image::m_dataArray)
        ;
}