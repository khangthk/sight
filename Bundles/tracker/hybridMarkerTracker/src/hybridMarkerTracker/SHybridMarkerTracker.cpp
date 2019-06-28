/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "hybridMarkerTracker/SHybridMarkerTracker.hpp"

#include <arData/Camera.hpp>
#include <arData/FrameTL.hpp>
#include <arData/MarkerMap.hpp>
#include <arData/MarkerTL.hpp>

#include <cvIO/Camera.hpp>
#include <cvIO/FrameTL.hpp>
#include <cvIO/Image.hpp>
#include <cvIO/Matrix.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwRuntime/operations.hpp>

#include <cmath>
namespace hybridMarkerTracker
{

fwServicesRegisterMacro(::arServices::ITracker, ::hybridMarkerTracker::SHybridMarkerTracker);

const ::fwCom::Slots::SlotKeyType SHybridMarkerTracker::s_SET_INT_PARAMETER_SLOT   = "setIntParameter";
const ::fwCom::Slots::SlotKeyType SHybridMarkerTracker::s_SET_FLOAT_PARAMETER_SLOT = "setDoubleParameter";
const ::fwCom::Slots::SlotKeyType SHybridMarkerTracker::s_SET_BOOL_PARAMETER_SLOT  = "setBoolParameter";

static const ::fwServices::IService::KeyType s_FRAME_INPUT  = "frameIn";
static const ::fwServices::IService::KeyType s_POSE_INOUT   = "pose";
static const ::fwServices::IService::KeyType s_CAMERA_INPUT = "camera";

SHybridMarkerTracker::SHybridMarkerTracker() noexcept :
    m_tracker(NULL),
    m_showDrawings(true)
{
    m_currentcHp = ::cv::Mat::eye(4, 4, CV_64F);

    newSlot(s_SET_INT_PARAMETER_SLOT, &SHybridMarkerTracker::setIntParameter, this);
    newSlot(s_SET_FLOAT_PARAMETER_SLOT, &SHybridMarkerTracker::setDoubleParameter, this);
    newSlot(s_SET_BOOL_PARAMETER_SLOT, &SHybridMarkerTracker::setBoolParameter, this);

}

SHybridMarkerTracker::~SHybridMarkerTracker()
{
    if(m_tracker)
    {
        delete m_tracker;
        m_tracker = nullptr;
    }
}

//------------------------------------------------------------------------------

void SHybridMarkerTracker::readSettings(std::string filename)
{
    ::cv::FileStorage fs;
    SLM_DEBUG("Initializing...");
    fs.open(filename, ::cv::FileStorage::READ);
    // Read settings & configuration
    fs["Sym_BoardSize_Width" ] >> m_symboardSize.width;
    fs["Sym_BoardSize_Height"] >> m_symboardSize.height;
    fs["Asym_Square_Size"]  >> m_asymSquareSize;
    fs["Sym_Square_Size_X" ] >> m_symSquareSize.x;
    fs["Sym_Square_Size_Y"] >> m_symSquareSize.y;
    fs["Radius"]  >> m_radius;
    fs["Chess_Dist_Center"]  >> m_chessDistCenter;
    fs["Chess_Interval"]  >> m_chessInterval;

    fs.release();

    // --- Tracker parameters---
    ::cv::Size roiSize(300, 300);
    // Global blob detector setting
    ::cv::SimpleBlobDetector::Params params;
    params.minRepeatability    = 2;
    params.minDistBetweenBlobs = 10;
    params.minThreshold        = 80;
    params.maxThreshold        = 160;
    params.thresholdStep       = 20;
    params.filterByArea        = true;
    params.minArea             = 50;
    params.maxArea             = 1000;
    params.filterByConvexity   = true;
    params.minConvexity        = 0.85f;
    params.maxConvexity        = 1.0f;
    params.filterByCircularity = true;
    params.minCircularity      = 0.7f;
    params.maxCircularity      = 1.0f;
    params.filterByInertia     = false;
    params.minInertiaRatio     = 0.01;

    // Local blob detector setting
    ::cv::SimpleBlobDetector::Params paramsRoi;
    paramsRoi.minRepeatability    = 2;
    paramsRoi.minDistBetweenBlobs = 10;
    paramsRoi.minThreshold        = 50;
    paramsRoi.maxThreshold        = 220;
    paramsRoi.thresholdStep       = 10;
    paramsRoi.filterByArea        = true;
    paramsRoi.minArea             = 50;
    paramsRoi.maxArea             = 2000;

    /**
     *  Calculate model points for marker
     */
    const float arcInner      = m_symSquareSize.y;
    const float arcOutter     = arcInner + 2 * m_asymSquareSize;
    const float chordInner_2  = m_radius * sin(arcInner / (2*m_radius));
    const float chordOutter_2 = m_radius * sin(arcOutter / (2*m_radius));
    const float sagittaInner  = m_radius - sqrt(m_radius * m_radius - chordInner_2 * chordInner_2);
    const float sagittaOutter = m_radius - sqrt(m_radius * m_radius - chordOutter_2 * chordOutter_2);
    // MID
    for ( int i = 0; i < m_symboardSize.height; i++ )
    {
        for ( int j = 0; j < m_symboardSize.width; j++)
        {
            ::cv::Point3f pt;
            pt.x = i * m_symSquareSize.x;
            pt.y = chordInner_2;
            pt.y = (j % 2) == 0 ? pt.y : -pt.y;
            pt.z = sagittaInner;
            m_trackMidPatternPoints.push_back(pt);
        }
    }
    // TOP
    const int asymPointsNum = m_symboardSize.height + m_symboardSize.height - 1;
    for ( int i = 0; i < asymPointsNum; i++ )
    {
        ::cv::Point3f pt;
        pt.x = (m_symboardSize.height-1) * m_symSquareSize.x
               - i * m_asymSquareSize;
        pt.y = (i % 2) == 0 ? chordInner_2 : chordOutter_2;
        pt.z = (i % 2) == 0 ? sagittaInner : sagittaOutter;
        m_trackTopPatternPoints.push_back(pt);
    }
    // BOT
    for ( int i = 0; i < asymPointsNum; i++ )
    {
        ::cv::Point3f pt;
        pt.x = i * m_asymSquareSize;
        pt.y = (i % 2) == 0 ? -chordInner_2 : -chordOutter_2;
        pt.z = (i % 2) == 0 ? sagittaInner : sagittaOutter;
        m_trackBotPatternPoints.push_back(pt);
    }

    // Chess pattern points
    // MID
    for (auto i = -1; i < 5; i++)
    {
        ::cv::Point3f pt(0.0f, 0.0f, 0.0f);
        pt.x = m_chessInterval/2 + i * m_chessInterval;
        m_trackChessMidPatternPoint.push_back(pt);
    }

    const float arcChess     = m_chessDistCenter * 2;
    const float chordChess_2 = m_radius * sin(arcChess / (2*m_radius));
    float sagittaChess;
    if (arcChess < CV_PI * m_radius)
    {
        sagittaChess = m_radius - sqrt(m_radius * m_radius - chordChess_2 * chordChess_2);
    }
    else
    {
        sagittaChess = m_radius + sqrt(m_radius * m_radius - chordChess_2 * chordChess_2);
    }
    // TOP
    for (auto i = 0; i < 5; i++)
    {
        ::cv::Point3f pt;
        pt.x = i * m_chessInterval;
        pt.y = chordChess_2;
        pt.z = sagittaChess;

        m_trackChessTopPatternPoint.push_back(pt);
    }
    // BOT
    for (auto i = 0; i < 5; i++)
    {
        ::cv::Point3f pt;
        pt.x = i * m_chessInterval;
        pt.y = -chordChess_2;
        pt.z = sagittaChess;

        m_trackChessBotPatternPoint.push_back(pt);
    }
    m_tracker = new TrackerCurvedot(m_symboardSize, roiSize, params, paramsRoi);
}

//------------------------------------------------------------------------------

void SHybridMarkerTracker::process()
{
    bool useIppe = true;

    // Tracking
    if (m_tracker->track(m_imgTrack))
    {
        const std::vector< ::cv::Point2f > imgPoints = m_tracker->getP_img();

        // Determine which pattern is detected
        std::vector< ::cv::Point3f > visiblePatternPoints;
        const int currDetectState = m_tracker->CurrDetectState();
        if ((currDetectState& TrackerCurvedot::TOP_CIR) && !(currDetectState & TrackerCurvedot::MID_CIR))
        {
            visiblePatternPoints = m_trackTopPatternPoints;
        }
        else if ((currDetectState& TrackerCurvedot::BOT_CIR) && !(currDetectState & TrackerCurvedot::MID_CIR))
        {
            visiblePatternPoints = m_trackBotPatternPoints;
        }
        else if ((currDetectState& TrackerCurvedot::TOP_CIR) && (currDetectState & TrackerCurvedot::MID_CIR))
        {
            const int numSymDot = m_trackMidPatternPoints.size();
            visiblePatternPoints.reserve(m_trackTopPatternPoints.size() + numSymDot/2);
            visiblePatternPoints.insert(visiblePatternPoints.end(),
                                        m_trackTopPatternPoints.begin(), m_trackTopPatternPoints.end());
            for (int i = 0; i < numSymDot; i++)
            {
                if (i%2 != 0)
                {
                    visiblePatternPoints.push_back(m_trackMidPatternPoints[i]);
                }
            }
            useIppe = false;       // IPPE is only for planar model
        }
        else if ((currDetectState& TrackerCurvedot::BOT_CIR) && (currDetectState & TrackerCurvedot::MID_CIR))
        {
            const int numSymDot = m_trackMidPatternPoints.size();
            visiblePatternPoints.reserve(m_trackBotPatternPoints.size() + numSymDot/2);
            visiblePatternPoints.insert(visiblePatternPoints.end(),
                                        m_trackBotPatternPoints.begin(), m_trackBotPatternPoints.end());
            for (int i = 0; i < numSymDot; i++)
            {
                if (i%2 == 0)
                {
                    visiblePatternPoints.push_back(m_trackMidPatternPoints[i]);
                }
            }
            useIppe = false;
        }
        else if (currDetectState & TrackerCurvedot::MID_CIR)
        {
            visiblePatternPoints = m_trackMidPatternPoints;
        }

        // Calculate pattern pose in camera coordinate
        ::cv::Mat rvec, tvec, cRp;
        ::cv::Mat rvec1, tvec1;     // 1st solution
        ::cv::Mat rvec2, tvec2;     // 2nd solution
        float error1, error2;

        m_currentcHp = ::cv::Mat::eye(4, 4, CV_64F);

        if (useIppe)
        {
            std::vector< ::cv::Point3f > pts_3d;
            bool isChessDetect = true;

            if (currDetectState & TrackerCurvedot::TOP_CHESS)
            {
                pts_3d = m_trackChessTopPatternPoint;
            }
            else if (currDetectState & TrackerCurvedot::MID_CHESS)
            {
                pts_3d = m_trackChessMidPatternPoint;
            }
            else if (currDetectState & TrackerCurvedot::BOT_CHESS)
            {
                pts_3d = m_trackChessBotPatternPoint;
            }
            else
            {
                isChessDetect = false;        // Chess line not found

            }
            ippeSolver.solveGeneric(visiblePatternPoints, imgPoints,
                                    m_cameraMatrix, m_distCoeffs, rvec1, tvec1, error1, rvec2, tvec2, error2);

            // Use chessboard features to disambiguate if two solutions are similar
            if (isChessDetect && abs(error1 - error2) < 0.1 && error1 < 0.2 && error2 < 0.2)
            {
                calculateCorrectPose(rvec1, tvec1, rvec2, tvec2, pts_3d, rvec, tvec);
            }
            else
            {
                rvec = rvec1;
                tvec = tvec1;
            }

            ::cv::Mat cHp_1 = ::cv::Mat::eye(4, 4, CV_64F);
            ::cv::Rodrigues(rvec1, cRp);
            ::cv::Mat aux = cHp_1.colRange(0, 3).rowRange(0, 3);
            cRp.copyTo(aux);
            aux = cHp_1.colRange(3, 4).rowRange(0, 3);
            tvec1.copyTo(aux);

            ::cv::Mat cHp_2 = ::cv::Mat::eye(4, 4, CV_64F);
            ::cv::Rodrigues(rvec2, cRp);
            aux = cHp_2.colRange(0, 3).rowRange(0, 3);
            cRp.copyTo(aux);
            aux = cHp_2.colRange(3, 4).rowRange(0, 3);
            tvec2.copyTo(aux);

            if(m_showDrawings)
            {
                drawRect(cHp_1, m_imgTrack, ::cv::Scalar(0, 0, 255));
                drawRect(cHp_2, m_imgTrack, ::cv::Scalar(255, 0, 0));
            }

            m_currentcHp = cHp_1;
        }
        else
        {
            ::cv::solvePnP(visiblePatternPoints, imgPoints, m_cameraMatrix, m_distCoeffs, rvec, tvec);
            ::cv::Rodrigues(rvec, cRp);
            ::cv::Mat aux = m_currentcHp.colRange(0, 3).rowRange(0, 3);
            cRp.copyTo(aux);
            aux = m_currentcHp.colRange(3, 4).rowRange(0, 3);
            tvec.copyTo(aux);

            if(m_showDrawings)
            {
                drawRect(m_currentcHp, m_imgTrack);

            }
        }

        if(m_showDrawings)
        {
            m_tracker->drawKeydots(m_imgTrack);
        }
    }
    else
    {
        SLM_WARN("Cannot find the pattern");
    }

    const std::string str_1 = "Blue rectangle shows current estimated pose";
    const std::string str_2 = "Red rectangle shows the ambiguous pose provided by IPPE";
    const std::string str_3 = "Green shows detection of pattern";
    const std::string str_4 = "Yellow shows tracking of pattern";

    if(m_showDrawings)
    {
        ::cv::putText(m_imgTrack, str_1, ::cv::Point(10, 20), ::cv::FONT_HERSHEY_COMPLEX, 0.5, ::cv::Scalar(0, 0,
                                                                                                            255), 1);
        ::cv::putText(m_imgTrack, str_2, ::cv::Point(10, 40), ::cv::FONT_HERSHEY_COMPLEX, 0.5, ::cv::Scalar(255, 0,
                                                                                                            0), 1);
        ::cv::putText(m_imgTrack, str_3, ::cv::Point(10, 60), ::cv::FONT_HERSHEY_COMPLEX, 0.5, ::cv::Scalar(0, 255,
                                                                                                            0), 1);
        ::cv::putText(m_imgTrack, str_4, ::cv::Point(10, 80), ::cv::FONT_HERSHEY_COMPLEX, 0.5, ::cv::Scalar(255, 255,
                                                                                                            0),
                      1);
    }
}

//------------------------------------------------------------------------------

::cv::Vec2f SHybridMarkerTracker::errorDistPoints(const std::vector< ::cv::Point2f >& pts_d,
                                                  const std::vector< ::cv::Point2f >& pts_1,
                                                  const std::vector< ::cv::Point2f >& pts_2,
                                                  const double max_dist_sq)
{
    ::cv::Vec2f sumError(0, 0);
    auto distSq_1 = 0.0, distSq_2 = 0.0;
    ::cv::Point2f ptsDiff;
    for (auto i = 0; i < pts_d.size(); i++)
    {
        int validInd = -1;
        for (auto j = 0; j < pts_1.size(); j++)
        {
            ptsDiff  = pts_d[i] - pts_1[j];
            distSq_1 = ptsDiff.x * ptsDiff.x + ptsDiff.y * ptsDiff.y;
            if (distSq_1 > max_dist_sq)
            {
                continue;
            }
            else
            {
                validInd = j;
                break;
            }
        }

        for (auto j = 0; j < pts_2.size(); j++)
        {
            ptsDiff  = pts_d[i] - pts_2[j];
            distSq_2 = ptsDiff.x * ptsDiff.x + ptsDiff.y * ptsDiff.y;
            if (distSq_2 > max_dist_sq)
            {
                continue;
            }

            if (validInd == j) // assume pts_1 and pts_2 have same order
            {
                sumError[0] += distSq_1;
                sumError[1] += distSq_2;

                break;
            }
        }
        distSq_1 = distSq_2 = 0.0;
    }

    // recursive in case threshold max_dist_sq is too small
    if (sumError[0] == 0.0)
    {
        sumError = errorDistPoints(pts_d, pts_1, pts_2, max_dist_sq * 2);
    }

    return sumError;
}

//------------------------------------------------------------------------------

void SHybridMarkerTracker::calculateCorrectPose(
    ::cv::InputArray rvec1, ::cv::InputArray tvec1,
    ::cv::InputArray rvec2, ::cv::InputArray tvec2,
    const std::vector< ::cv::Point3f >& pts_3d,
    ::cv::OutputArray rvec, ::cv::OutputArray tvec)
{
    std::vector<cv::Point2f> projPoints_1, projPoints_2;

    ::cv::projectPoints(pts_3d, rvec1, tvec1,
                        m_cameraMatrix, m_distCoeffs, projPoints_1);
    ::cv::projectPoints(pts_3d, rvec2, tvec2,
                        m_cameraMatrix, m_distCoeffs, projPoints_2);

    std::vector< ::cv::Point2f > detect_pts = m_tracker->get_chess_pts();

    // Calculate a threshold to determine correspondence
    ::cv::Point2f diff_temp = (projPoints_1[0] - projPoints_1[1]) * 0.7;
    double max_dist_sq = diff_temp.x*diff_temp.x + diff_temp.y*diff_temp.y;
    diff_temp   = (projPoints_2[0] - projPoints_2[1]) * 0.7;
    max_dist_sq = (max_dist_sq + diff_temp.x*diff_temp.x + diff_temp.y*diff_temp.y)/2;

    const ::cv::Vec2f errors = errorDistPoints(detect_pts, projPoints_1, projPoints_2, max_dist_sq);

    rvec.create(3, 1, CV_64FC1);
    tvec.create(3, 1, CV_64FC1);
    if ( errors[0] < errors[1] )
    {
        rvec1.getMat().copyTo(rvec);
        tvec1.getMat().copyTo(tvec);
    }
    else
    {
        rvec2.getMat().copyTo(rvec);
        tvec2.getMat().copyTo(tvec);
    }
}

//------------------------------------------------------------------------------

void SHybridMarkerTracker::drawRect(const ::cv::Mat& cHp, ::cv::Mat& img, ::cv::Scalar color)
{
    ::cv::Mat rectCorners        = ::cv::Mat::ones(4, 4, CV_64F);
    rectCorners.at<double>(0, 0) = 0.0;
    rectCorners.at<double>(1, 0) = 0.0;
    rectCorners.at<double>(2, 0) = 0.0;
    rectCorners.at<double>(0, 1) = 20.0;
    rectCorners.at<double>(1, 1) = 0.0;
    rectCorners.at<double>(2, 1) = 0.0;
    rectCorners.at<double>(0, 2) = 20.0;
    rectCorners.at<double>(1, 2) = 0.0;
    rectCorners.at<double>(2, 2) = 30.0;
    rectCorners.at<double>(0, 3) = 0.0;
    rectCorners.at<double>(1, 3) = 0.0;
    rectCorners.at<double>(2, 3) = 30.0;

    const ::cv::Mat cornersHm = cHp * rectCorners;

    std::vector < ::cv::Point3f > corners(4);    // 0-3: US image endpoints, 4-7: shadow pts

    for (int i = 0; i < corners.size(); i++)
    {
        corners[i].x = cornersHm.at<double>(0, i) / cornersHm.at<double>(3, i);
        corners[i].y = cornersHm.at<double>(1, i) / cornersHm.at<double>(3, i);
        corners[i].z = cornersHm.at<double>(2, i) / cornersHm.at<double>(3, i);
    }

    ::cv::Mat rVec, tVec, m_distCoeffs;
    rVec = ::cv::Mat::zeros(3, 1, CV_32FC1);
    tVec = ::cv::Mat::zeros(3, 1, CV_32FC1);
    std::vector < ::cv::Point2f > corners_2d;

    ::cv::projectPoints(corners, rVec, tVec, m_cameraMatrix,
                        m_distCoeffs, corners_2d);

    ::cv::Rodrigues(cHp(::cv::Rect(0, 0, 3, 3)), rVec);
    cHp.colRange(3, 4).rowRange(0, 3).copyTo(tVec);

    ::cv::line(img, corners_2d[0], corners_2d[1], color, 2);
    ::cv::line(img, corners_2d[1], corners_2d[2], color, 2);
    ::cv::line(img, corners_2d[2], corners_2d[3], color, 2);
    ::cv::line(img, corners_2d[0], corners_2d[3], color, 2);

    ::cv::circle(img, corners_2d[0], 4, color, -1);

}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SHybridMarkerTracker::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_FRAME_INPUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( s_FRAME_INPUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT );
    return connections;
}

//------------------------------------------------------------------------------

void SHybridMarkerTracker::starting()
{
    auto filePath = ::fwRuntime::getBundleResourceFilePath("hybridMarkerTracker", "settings.xml");
    readSettings(filePath.string());
    this->startTracking();
}

//------------------------------------------------------------------------------

void SHybridMarkerTracker::tracking(::fwCore::HiResClock::HiResClockType& timestamp)
{
    auto frameIn = this->getInput< ::fwData::Image >(s_FRAME_INPUT);
    SLM_ASSERT("The In key '" + s_FRAME_INPUT + "' is not defined", frameIn);

    auto frameOut = this->getInOut< ::fwData::Image >(s_FRAME_INOUT);
    SLM_ASSERT("The InOut key '" + s_FRAME_INOUT + "' is not defined", frameOut);

    auto matrixOut = this->getInOut< ::fwData::TransformationMatrix3D >(s_POSE_INOUT);
    SLM_ASSERT("The InOut key '" + s_POSE_INOUT + "' is not defined", matrixOut);

    auto arCamera = this->getInput< ::arData::Camera >(s_CAMERA_INPUT);
    SLM_ASSERT("The InPut key '" + s_CAMERA_INPUT + "' is not defined", arCamera);

    std::tie(m_cameraMatrix, m_camImgSize, m_distCoeffs) = ::cvIO::Camera::copyToCv(arCamera);

    if (frameIn)
    {
        // check if image dimension have changed
        ::fwData::Image::SizeType size;
        size = frameIn->getSize();
        // if so, we need to initialize the output image
        if(size != frameOut->getSize())
        {
            const ::fwData::Image::SpacingType::value_type voxelSize = 1;
            frameOut->allocate(size, frameIn->getType(), frameIn->getNumberOfComponents());
            ::fwData::Image::OriginType origin(2, 0);

            frameOut->setOrigin(origin);
            ::fwData::Image::SpacingType spacing(2, voxelSize);
            frameOut->setSpacing(spacing);
            frameOut->setWindowWidth(1);
            frameOut->setWindowCenter(0);
        }

        {
            ::fwData::mt::ObjectWriteLock outputLock(frameOut);
            ::cv::Mat img_track = ::cvIO::Image::moveToCv(frameOut);
            m_imgTrack          = ::cvIO::Image::copyToCv(frameIn);

            process();

            m_imgTrack.copyTo(img_track);

            auto sig = frameOut->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Image::s_BUFFER_MODIFIED_SIG);
            sig->asyncEmit();
        }

        {
            ::fwData::mt::ObjectWriteLock matrixLock(matrixOut);

            ::cvIO::Matrix::copyFromCv(m_currentcHp, matrixOut);

            auto sig = matrixOut->signal<  ::fwData::TransformationMatrix3D::ModifiedSignalType >
                           ( ::fwData::TransformationMatrix3D::s_MODIFIED_SIG);
            sig->asyncEmit();
        }
    }
}

//------------------------------------------------------------------------------

void SHybridMarkerTracker::setIntParameter(int _val, std::string _key)
{
    int val = _val;
    if(_key == "symboardSizeWidth")
    {
        m_symboardSize.width = val;
    }
    else if(_key == "symboardSizeHeight")
    {
        m_symboardSize.height = val;
    }
    else
    {
        SLM_ERROR("The slot key : '"+ _key + "' is not handled");
    }
}

//------------------------------------------------------------------------------

void SHybridMarkerTracker::setDoubleParameter(double _val, std::string _key)
{
    float val = _val;
    if(_key == "asymSquareSize")
    {
        m_asymSquareSize = val;
    }
    else if(_key == "symSquareSizeX")
    {
        m_symSquareSize.x = val;
    }
    else if(_key == "symSquareSizeY")
    {
        m_symSquareSize.y = val;
    }
    else if(_key == "radius")
    {
        m_radius = val;
    }
    else if(_key == "chessDistCenter")
    {
        m_chessDistCenter = val;
    }
    else if(_key == "chessInterval")
    {
        m_chessInterval = val;
    }
    else
    {
        SLM_ERROR("The slot key : '"+ _key + "' is not handled");
    }
}

//------------------------------------------------------------------------------

void SHybridMarkerTracker::setBoolParameter(bool _val, std::string _key)
{
    bool val = _val;
    if(_key == "showDrawings")
    {
        m_showDrawings = val;
    }
    else
    {
        SLM_ERROR("The slot key : '"+ _key + "' is not handled");
    }

}

//------------------------------------------------------------------------------

void SHybridMarkerTracker::configuring()
{
}

//------------------------------------------------------------------------------

void SHybridMarkerTracker::updating()
{
    auto timestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    this->tracking(timestamp);
}

//------------------------------------------------------------------------------

void SHybridMarkerTracker::stopping()
{
    this->stopTracking();
}

//------------------------------------------------------------------------------

} // namespace hybridMarkerTracker
