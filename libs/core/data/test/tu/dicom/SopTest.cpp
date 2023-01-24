// This file was automatically generated by the '/libs/core/data/dicom/class_generator.py' script. DO NOT EDIT !
// Used arguments: 'Namespace(verbose=False, source_url='http://dicom.nema.org/medical/dicom/current/source/docbook', sop_list=None, mandatory_tags=[Tag(group='7FE0', element='0010'), Tag(group='0062', element='0002'), Tag(group='0066', element='0002'), Tag(group='0066', element='0011')])'

#include "SopTest.hpp"

#include <data/dicom/Sop.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::dicom::ut::SopTest);

// cspell: ignore Wundefined afterloader angio autorefraction autosequence boli bscan cadsr ciexy ctdi ctdivol decf
// cspell: ignore deidentification dermoscopic diconde dicos dvhroi exif fiducials fluence fnumber frameof gpsdop hicpp
// cspell: ignore hpgl imager inms inua inus iods isop ivol ivus keratoconus keratometer keratometric kerma lensometry
// cspell: ignore lookat meterset metersets mrfov multiway nmpet nmtomo nolintbegin nolintend octb octz oecf olink
// cspell: ignore postamble powerline prcs presaturation radiofluoroscopic relaxivity reprojection rgblut rtdvh rtroi
// cspell: ignore sddn sdhn sdvn softcopy soundpath stereometric stowrs tlhc tomo tomosynthesis tomotherapeutic toric
// cspell: ignore tractography xaxrf

// NOLINTBEGIN

namespace sight::data::dicom::ut
{

void SopTest::setUp()
{
}

//------------------------------------------------------------------------------

void SopTest::tearDown()
{
}

//------------------------------------------------------------------------------

void SopTest::sopTest()
{
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::ComputedRadiographyImageStorage>::s_keyword, sop::get(sop::Keyword::ComputedRadiographyImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::ComputedRadiographyImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::DigitalXRayImageStorageForPresentation>::s_keyword, sop::get(sop::Keyword::DigitalXRayImageStorageForPresentation).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::DigitalXRayImageStorageForPresentation>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.1.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::DigitalXRayImageStorageForProcessing>::s_keyword, sop::get(sop::Keyword::DigitalXRayImageStorageForProcessing).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::DigitalXRayImageStorageForProcessing>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.1.1.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::DigitalMammographyXRayImageStorageForPresentation>::s_keyword, sop::get(sop::Keyword::DigitalMammographyXRayImageStorageForPresentation).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::DigitalMammographyXRayImageStorageForPresentation>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.1.2").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::DigitalMammographyXRayImageStorageForProcessing>::s_keyword, sop::get(sop::Keyword::DigitalMammographyXRayImageStorageForProcessing).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::DigitalMammographyXRayImageStorageForProcessing>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.1.2.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::DigitalIntraOralXRayImageStorageForPresentation>::s_keyword, sop::get(sop::Keyword::DigitalIntraOralXRayImageStorageForPresentation).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::DigitalIntraOralXRayImageStorageForPresentation>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.1.3").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::DigitalIntraOralXRayImageStorageForProcessing>::s_keyword, sop::get(sop::Keyword::DigitalIntraOralXRayImageStorageForProcessing).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::DigitalIntraOralXRayImageStorageForProcessing>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.1.3.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::XRayAngiographicImageStorage>::s_keyword, sop::get(sop::Keyword::XRayAngiographicImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::XRayAngiographicImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.12.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::EnhancedXAImageStorage>::s_keyword, sop::get(sop::Keyword::EnhancedXAImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::EnhancedXAImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.12.1.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::XRayRadiofluoroscopicImageStorage>::s_keyword, sop::get(sop::Keyword::XRayRadiofluoroscopicImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::XRayRadiofluoroscopicImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.12.2").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::EnhancedXRFImageStorage>::s_keyword, sop::get(sop::Keyword::EnhancedXRFImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::EnhancedXRFImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.12.2.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::PositronEmissionTomographyImageStorage>::s_keyword, sop::get(sop::Keyword::PositronEmissionTomographyImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::PositronEmissionTomographyImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.128").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::LegacyConvertedEnhancedPETImageStorage>::s_keyword, sop::get(sop::Keyword::LegacyConvertedEnhancedPETImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::LegacyConvertedEnhancedPETImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.128.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::XRay3DAngiographicImageStorage>::s_keyword, sop::get(sop::Keyword::XRay3DAngiographicImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::XRay3DAngiographicImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.13.1.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::XRay3DCraniofacialImageStorage>::s_keyword, sop::get(sop::Keyword::XRay3DCraniofacialImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::XRay3DCraniofacialImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.13.1.2").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::BreastTomosynthesisImageStorage>::s_keyword, sop::get(sop::Keyword::BreastTomosynthesisImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::BreastTomosynthesisImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.13.1.3").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::BreastProjectionXRayImageStorageForPresentation>::s_keyword, sop::get(sop::Keyword::BreastProjectionXRayImageStorageForPresentation).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::BreastProjectionXRayImageStorageForPresentation>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.13.1.4").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::BreastProjectionXRayImageStorageForProcessing>::s_keyword, sop::get(sop::Keyword::BreastProjectionXRayImageStorageForProcessing).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::BreastProjectionXRayImageStorageForProcessing>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.13.1.5").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::EnhancedPETImageStorage>::s_keyword, sop::get(sop::Keyword::EnhancedPETImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::EnhancedPETImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.130").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::IntravascularOpticalCoherenceTomographyImageStorageForPresentation>::s_keyword, sop::get(sop::Keyword::IntravascularOpticalCoherenceTomographyImageStorageForPresentation).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::IntravascularOpticalCoherenceTomographyImageStorageForPresentation>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.14.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::IntravascularOpticalCoherenceTomographyImageStorageForProcessing>::s_keyword, sop::get(sop::Keyword::IntravascularOpticalCoherenceTomographyImageStorageForProcessing).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::IntravascularOpticalCoherenceTomographyImageStorageForProcessing>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.14.2").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::CTImageStorage>::s_keyword, sop::get(sop::Keyword::CTImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::CTImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.2").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::EnhancedCTImageStorage>::s_keyword, sop::get(sop::Keyword::EnhancedCTImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::EnhancedCTImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.2.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::LegacyConvertedEnhancedCTImageStorage>::s_keyword, sop::get(sop::Keyword::LegacyConvertedEnhancedCTImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::LegacyConvertedEnhancedCTImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.2.2").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::NuclearMedicineImageStorage>::s_keyword, sop::get(sop::Keyword::NuclearMedicineImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::NuclearMedicineImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.20").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::UltrasoundMultiFrameImageStorage>::s_keyword, sop::get(sop::Keyword::UltrasoundMultiFrameImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::UltrasoundMultiFrameImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.3.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::ParametricMapStorage>::s_keyword, sop::get(sop::Keyword::ParametricMapStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::ParametricMapStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.30").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::MRImageStorage>::s_keyword, sop::get(sop::Keyword::MRImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::MRImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.4").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::EnhancedMRImageStorage>::s_keyword, sop::get(sop::Keyword::EnhancedMRImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::EnhancedMRImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.4.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::EnhancedMRColorImageStorage>::s_keyword, sop::get(sop::Keyword::EnhancedMRColorImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::EnhancedMRColorImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.4.3").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::LegacyConvertedEnhancedMRImageStorage>::s_keyword, sop::get(sop::Keyword::LegacyConvertedEnhancedMRImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::LegacyConvertedEnhancedMRImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.4.4").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::RTImageStorage>::s_keyword, sop::get(sop::Keyword::RTImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::RTImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.481.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::RTDoseStorage>::s_keyword, sop::get(sop::Keyword::RTDoseStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::RTDoseStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.481.2").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::UltrasoundImageStorage>::s_keyword, sop::get(sop::Keyword::UltrasoundImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::UltrasoundImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.6.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::EnhancedUSVolumeStorage>::s_keyword, sop::get(sop::Keyword::EnhancedUSVolumeStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::EnhancedUSVolumeStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.6.2").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::SegmentationStorage>::s_keyword, sop::get(sop::Keyword::SegmentationStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::SegmentationStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.66.4").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::SurfaceSegmentationStorage>::s_keyword, sop::get(sop::Keyword::SurfaceSegmentationStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::SurfaceSegmentationStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.66.5").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::SurfaceScanMeshStorage>::s_keyword, sop::get(sop::Keyword::SurfaceScanMeshStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::SurfaceScanMeshStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.68.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::SurfaceScanPointCloudStorage>::s_keyword, sop::get(sop::Keyword::SurfaceScanPointCloudStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::SurfaceScanPointCloudStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.68.2").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::SecondaryCaptureImageStorage>::s_keyword, sop::get(sop::Keyword::SecondaryCaptureImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::SecondaryCaptureImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.7").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::MultiFrameSingleBitSecondaryCaptureImageStorage>::s_keyword, sop::get(sop::Keyword::MultiFrameSingleBitSecondaryCaptureImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::MultiFrameSingleBitSecondaryCaptureImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.7.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::MultiFrameGrayscaleByteSecondaryCaptureImageStorage>::s_keyword, sop::get(sop::Keyword::MultiFrameGrayscaleByteSecondaryCaptureImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::MultiFrameGrayscaleByteSecondaryCaptureImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.7.2").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::MultiFrameGrayscaleWordSecondaryCaptureImageStorage>::s_keyword, sop::get(sop::Keyword::MultiFrameGrayscaleWordSecondaryCaptureImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::MultiFrameGrayscaleWordSecondaryCaptureImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.7.3").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::MultiFrameTrueColorSecondaryCaptureImageStorage>::s_keyword, sop::get(sop::Keyword::MultiFrameTrueColorSecondaryCaptureImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::MultiFrameTrueColorSecondaryCaptureImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.7.4").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::VLEndoscopicImageStorage>::s_keyword, sop::get(sop::Keyword::VLEndoscopicImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::VLEndoscopicImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.77.1.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::VideoEndoscopicImageStorage>::s_keyword, sop::get(sop::Keyword::VideoEndoscopicImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::VideoEndoscopicImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.77.1.1.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::VLMicroscopicImageStorage>::s_keyword, sop::get(sop::Keyword::VLMicroscopicImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::VLMicroscopicImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.77.1.2").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::VideoMicroscopicImageStorage>::s_keyword, sop::get(sop::Keyword::VideoMicroscopicImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::VideoMicroscopicImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.77.1.2.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::VLSlideCoordinatesMicroscopicImageStorage>::s_keyword, sop::get(sop::Keyword::VLSlideCoordinatesMicroscopicImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::VLSlideCoordinatesMicroscopicImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.77.1.3").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::VLPhotographicImageStorage>::s_keyword, sop::get(sop::Keyword::VLPhotographicImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::VLPhotographicImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.77.1.4").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::VideoPhotographicImageStorage>::s_keyword, sop::get(sop::Keyword::VideoPhotographicImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::VideoPhotographicImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.77.1.4.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::OphthalmicPhotography8BitImageStorage>::s_keyword, sop::get(sop::Keyword::OphthalmicPhotography8BitImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::OphthalmicPhotography8BitImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.77.1.5.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::OphthalmicPhotography16BitImageStorage>::s_keyword, sop::get(sop::Keyword::OphthalmicPhotography16BitImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::OphthalmicPhotography16BitImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.77.1.5.2").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::OphthalmicTomographyImageStorage>::s_keyword, sop::get(sop::Keyword::OphthalmicTomographyImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::OphthalmicTomographyImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.77.1.5.4").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::WideFieldOphthalmicPhotographyStereographicProjectionImageStorage>::s_keyword, sop::get(sop::Keyword::WideFieldOphthalmicPhotographyStereographicProjectionImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::WideFieldOphthalmicPhotographyStereographicProjectionImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.77.1.5.5").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::WideFieldOphthalmicPhotography3DCoordinatesImageStorage>::s_keyword, sop::get(sop::Keyword::WideFieldOphthalmicPhotography3DCoordinatesImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::WideFieldOphthalmicPhotography3DCoordinatesImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.77.1.5.6").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::OphthalmicOpticalCoherenceTomographyEnFaceImageStorage>::s_keyword, sop::get(sop::Keyword::OphthalmicOpticalCoherenceTomographyEnFaceImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::OphthalmicOpticalCoherenceTomographyEnFaceImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.77.1.5.7").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage>::s_keyword, sop::get(sop::Keyword::OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.77.1.5.8").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::VLWholeSlideMicroscopyImageStorage>::s_keyword, sop::get(sop::Keyword::VLWholeSlideMicroscopyImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::VLWholeSlideMicroscopyImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.77.1.6").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::DermoscopicPhotographyImageStorage>::s_keyword, sop::get(sop::Keyword::DermoscopicPhotographyImageStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::DermoscopicPhotographyImageStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.77.1.7").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::OphthalmicThicknessMapStorage>::s_keyword, sop::get(sop::Keyword::OphthalmicThicknessMapStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::OphthalmicThicknessMapStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.81.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::CornealTopographyMapStorage>::s_keyword, sop::get(sop::Keyword::CornealTopographyMapStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::CornealTopographyMapStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.1.1.82.1").m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::GenericImplantTemplateStorage>::s_keyword, sop::get(sop::Keyword::GenericImplantTemplateStorage).m_keyword);
    CPPUNIT_ASSERT_EQUAL(sop::Sop<sop::Keyword::GenericImplantTemplateStorage>::s_keyword, sop::get("1.2.840.10008.5.1.4.43.1").m_keyword);
}

//------------------------------------------------------------------------------

} // namespace sight::data::dicom::ut

// NOLINTEND
