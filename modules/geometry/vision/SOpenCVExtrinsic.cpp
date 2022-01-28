/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#include "SOpenCVExtrinsic.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/tools/fwID.hpp>
#include <core/tools/Object.hpp>

#include <data/CalibrationInfo.hpp>
#include <data/Camera.hpp>
#include <data/CameraSeries.hpp>
#include <data/Matrix4.hpp>
#include <data/PointList.hpp>

#include <io/opencv/Matrix.hpp>

#include <service/IService.hpp>
#include <service/macros.hpp>

#include <ui/base/Preferences.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>

namespace sight::module::geometry::vision
{

static const core::com::Slots::SlotKeyType s_UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";
static const core::com::Signals::SignalKeyType s_ERROR_COMPUTED_SIG      = "errorComputed";

// ----------------------------------------------------------------------------

SOpenCVExtrinsic::SOpenCVExtrinsic() noexcept :
    m_width(11),
    m_height(8),
    m_squareSize(20.0),
    m_camIndex(1)
{
    newSignal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG);
    newSlot(s_UPDATE_CHESSBOARD_SIZE_SLOT, &SOpenCVExtrinsic::updateChessboardSize, this);
}

// ----------------------------------------------------------------------------

SOpenCVExtrinsic::~SOpenCVExtrinsic() noexcept
{
}

//------------------------------------------------------------------------------

void SOpenCVExtrinsic::configuring()
{
    core::runtime::ConfigurationElement::sptr cfgIdx = m_configuration->findConfigurationElement("camIndex");
    if(cfgIdx)
    {
        std::string idxStr = cfgIdx->getValue();
        SIGHT_ASSERT("'camIndex' is empty.", !idxStr.empty());
        m_camIndex = boost::lexical_cast<std::size_t>(idxStr);
    }

    core::runtime::ConfigurationElement::sptr cfgBoard = m_configuration->findConfigurationElement("board");
    SIGHT_ASSERT("Tag 'board' not found.", cfgBoard);

    SIGHT_ASSERT("Attribute 'width' is missing.", cfgBoard->hasAttribute("width"));
    m_widthKey = cfgBoard->getAttributeValue("width");
    SIGHT_ASSERT("Attribute 'width' is empty", !m_widthKey.empty());

    SIGHT_ASSERT("Attribute 'height' is missing.", cfgBoard->hasAttribute("height"));
    m_heightKey = cfgBoard->getAttributeValue("height");
    SIGHT_ASSERT("Attribute 'height' is empty", !m_heightKey.empty());

    if(cfgBoard->hasAttribute("squareSize"))
    {
        m_squareSizeKey = cfgBoard->getAttributeValue("squareSize");
        SIGHT_ASSERT("Attribute 'squareSize' is empty", !m_squareSizeKey.empty());
    }
}

// ----------------------------------------------------------------------------

void SOpenCVExtrinsic::starting()
{
    this->updateChessboardSize();
}

// ----------------------------------------------------------------------------

void SOpenCVExtrinsic::stopping()
{
}

//------------------------------------------------------------------------------

void SOpenCVExtrinsic::updating()
{
    const auto calInfo1 = m_calibrationInfo1.lock();
    const auto calInfo2 = m_calibrationInfo2.lock();

    SIGHT_ASSERT("Object with 'calibrationInfo1' is not found", calInfo1);
    SIGHT_ASSERT("Object with 'calibrationInfo2' is not found", calInfo2);

    SIGHT_WARN_IF("Calibration info is empty.", calInfo1->getPointListContainer().empty());
    if(!calInfo1->getPointListContainer().empty())
    {
        std::vector<std::vector<cv::Point3f> > objectPoints;

        std::vector<cv::Point3f> points;
        for(unsigned int y = 0 ; y < m_height - 1 ; ++y)
        {
            for(unsigned int x = 0 ; x < m_width - 1 ; ++x)
            {
                points.push_back(
                    cv::Point3f(
                        static_cast<float>(x) * m_squareSize,
                        static_cast<float>(y) * m_squareSize,
                        0
                    )
                );
            }
        }

        std::vector<std::vector<cv::Point2f> > imagePoints1;
        std::vector<std::vector<cv::Point2f> > imagePoints2;
        {
            data::CalibrationInfo::PointListContainerType pt_lists1 = calInfo1->getPointListContainer();
            data::CalibrationInfo::PointListContainerType pt_lists2 = calInfo2->getPointListContainer();

            SIGHT_ERROR_IF("The two calibrationInfo do not have the same size", pt_lists1.size() != pt_lists2.size());

            data::CalibrationInfo::PointListContainerType::iterator itr1    = pt_lists1.begin();
            data::CalibrationInfo::PointListContainerType::iterator itr2    = pt_lists2.begin();
            data::CalibrationInfo::PointListContainerType::iterator itrEnd1 = pt_lists1.end();
            data::CalibrationInfo::PointListContainerType::iterator itrEnd2 = pt_lists2.end();

            for( ; itr1 != itrEnd1 && itr2 != itrEnd2 ; ++itr1, ++itr2)
            {
                data::PointList::sptr ptList1 = *itr1;
                data::PointList::sptr ptList2 = *itr2;
                std::vector<cv::Point2f> imgPoint1;
                std::vector<cv::Point2f> imgPoint2;

                for(data::Point::csptr point : ptList1->getPoints())
                {
                    SIGHT_ASSERT("point is null", point);
                    imgPoint1.push_back(
                        cv::Point2f(
                            static_cast<float>(point->getCoord()[0]),
                            static_cast<float>(point->getCoord()[1])
                        )
                    );
                }

                for(data::Point::csptr point : ptList2->getPoints())
                {
                    SIGHT_ASSERT("point is null", point);
                    imgPoint2.push_back(
                        cv::Point2f(
                            static_cast<float>(point->getCoord()[0]),
                            static_cast<float>(point->getCoord()[1])
                        )
                    );
                }

                imagePoints1.push_back(imgPoint1);
                imagePoints2.push_back(imgPoint2);
                objectPoints.push_back(points);
            }
        }

        // Set the cameras
        cv::Mat cameraMatrix1 = cv::Mat::eye(3, 3, CV_64F);
        cv::Mat cameraMatrix2 = cv::Mat::eye(3, 3, CV_64F);

        std::vector<float> distortionCoefficients1(5);
        std::vector<float> distortionCoefficients2(5);
        cv::Mat rotationMatrix;
        cv::Mat translationVector;
        cv::Mat essentialMatrix;
        cv::Mat fundamentalMatrix;

        const auto camSeries = m_cameraSeries.lock();

        SIGHT_ASSERT(
            "camera index must be > 0 and < camSeries->numCameras()",
            m_camIndex > 0 && m_camIndex < camSeries->numCameras()
        );

        data::Image::sptr img = calInfo1->getImageContainer().front();
        cv::Size2i imgsize(static_cast<int>(img->getSize()[0]), static_cast<int>(img->getSize()[1]));
        {
            data::Camera::csptr cam1 = camSeries->getCamera(0);
            data::Camera::csptr cam2 = camSeries->getCamera(m_camIndex);

            data::mt::locked_ptr cam1Lock(cam1);
            data::mt::locked_ptr cam2Lock(cam2);

            cameraMatrix1.at<double>(0, 0) = cam1->getFx();
            cameraMatrix1.at<double>(1, 1) = cam1->getFy();
            cameraMatrix1.at<double>(0, 2) = cam1->getCx();
            cameraMatrix1.at<double>(1, 2) = cam1->getCy();

            cameraMatrix2.at<double>(0, 0) = cam2->getFx();
            cameraMatrix2.at<double>(1, 1) = cam2->getFy();
            cameraMatrix2.at<double>(0, 2) = cam2->getCx();
            cameraMatrix2.at<double>(1, 2) = cam2->getCy();
            for(std::size_t i = 0 ; i < 5 ; ++i)
            {
                distortionCoefficients1[i] = static_cast<float>(cam1->getDistortionCoefficient()[i]);
                distortionCoefficients2[i] = static_cast<float>(cam2->getDistortionCoefficient()[i]);
            }
        }
        double err = cv::stereoCalibrate(
            objectPoints,
            imagePoints1,
            imagePoints2,
            cameraMatrix1,
            distortionCoefficients1,
            cameraMatrix2,
            distortionCoefficients2,
            imgsize,
            rotationMatrix,
            translationVector,
            essentialMatrix,
            fundamentalMatrix,
            cv::CALIB_FIX_INTRINSIC,
            cv::TermCriteria(
                cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS,
                100,
                1e-5
            )
        );

        this->signal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG)->asyncEmit(err);

        data::Matrix4::sptr matrix = data::Matrix4::New();
        cv::Mat cv4x4              = cv::Mat::eye(4, 4, CV_64F);
        rotationMatrix.copyTo(cv4x4(cv::Rect(0, 0, 3, 3)));
        translationVector.copyTo(cv4x4(cv::Rect(3, 0, 1, 3)));

        io::opencv::Matrix::copyFromCv(cv4x4, matrix);

        {
            camSeries->setExtrinsicMatrix(m_camIndex, matrix);
        }

        data::CameraSeries::ExtrinsicCalibratedSignalType::sptr sig;
        sig = camSeries->signal<data::CameraSeries::ExtrinsicCalibratedSignalType>(
            data::CameraSeries::s_EXTRINSIC_CALIBRATED_SIG
        );

        sig->asyncEmit();

        // Export matrix if needed.
        m_matrix = matrix;
    }
}

//------------------------------------------------------------------------------

void SOpenCVExtrinsic::updateChessboardSize()
{
    try
    {
        ui::base::Preferences preferences;
        m_width      = preferences.get(m_widthKey, m_width);
        m_height     = preferences.get(m_heightKey, m_height);
        m_squareSize = preferences.get(m_squareSizeKey, m_squareSize);
    }
    catch(const ui::base::PreferencesDisabled&)
    {
        // Nothing to do..
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::geometry::vision
