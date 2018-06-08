/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "videoCharucoCalibration/config.hpp"

#include <arData/FrameTL.hpp>

#include <fwData/Image.hpp>
#include <fwData/PointList.hpp>

#include <fwServices/IController.hpp>

namespace videoCharucoCalibration
{

/**
 * @brief This service updates CalibrationInfo objects with the points detected from charucoBoard.
 *
 * This service is used by calling 'detectPoints' slot. It checks on each timeline if points are visible in each
 * frame. Then it adds the detected points and the associated image in the CalibrationInfo.
 *
 * @section Signals Signals
 * - \b charucoBoardDetected(): Emitted when the charucoBoard is detected on the current image.
 * - \b charucoBoardNotDetected(): Emitted when the charucoBoard is not detected on the current image.
 *
 * @section Slots Slots
 * - \b checkPoints(::fwCore::HiResClock::HiResClockType): Try to detect the charucoBoard in the image(s) from the
 * timeline(s) at the given timestamp.
 * - \b detectPoints(): Request to store the current image in the calibration data, if the charucoBoard is detected.
 * - \b updateChessboardSize(): update the parameters of the charucoBoard from preferences.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." impl="::videoCalibration::SChessBoardDetector" >
            <in group="timeline">
                <key uid="..." />
                <key uid="..." />
            </in>
            <inout group="calInfo">
                <key uid="..." />
                <key uid="..." />
            </inout>
            <inout group="detection">
                <key uid="..." />
                <key uid="..." />
            </inout>
           <board width="CHESSBOARD_WIDTH" height="CHESSBOARD_HEIGHT" squareSize="CHESSBOARD_SQUARE_SIZE"
 * markerSize="CHESSBOARD_MARKER_SIZE"/>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b timeline [::arData::FrameTL]: timelines containing the images to detect the charucoBoard.
 * @subsection In-Out In-Out:
 * - \b calInfo [::arData::CalibrationInfo]: calibration object where to store the detected images.
 * - \b detection [::fwData::PointList] (optional): detected charucoBoard points in image coordinates.
 * @subsection Configuration Configuration:
 * - \b board : preference key to retrieve the number of squares of the board in width and height.
 */
class VIDEOCHARUCOCALIBRATION_CLASS_API SCharucoBoardDetector : public ::fwServices::IController
{
public:
    fwCoreServiceClassDefinitionsMacro((SCharucoBoardDetector)(::fwServices::IController));

    /**
     * @name Signals API
     * @{
     */
    VIDEOCHARUCOCALIBRATION_API static const ::fwCom::Signals::SignalKeyType s_CHARUCOBOARD_DETECTED_SIG;
    typedef ::fwCom::Signal<void ()> CharucoBoardDetectedSignalType;

    VIDEOCHARUCOCALIBRATION_API static const ::fwCom::Signals::SignalKeyType s_CHARUCOBOARD_NOT_DETECTED_SIG;
    typedef ::fwCom::Signal<void ()> CharucoBoardNotDetectedSignalType;
    /// @}

    /**
     * @name Slots API
     * @{
     */
    VIDEOCHARUCOCALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_CHECK_POINTS_SLOT;
    VIDEOCHARUCOCALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_DETECT_POINTS_SLOT;
    VIDEOCHARUCOCALIBRATION_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CHARUCOBOARD_SIZE_SLOT;
    ///@}

    /// Constructor
    VIDEOCHARUCOCALIBRATION_API SCharucoBoardDetector() noexcept;

    /// Destructor
    VIDEOCHARUCOCALIBRATION_API ~SCharucoBoardDetector() noexcept;

    /**
     * @brief Detect charucoBoard points
     * @param tl the timeline containing frames displaying the charucoBoard
     * @param timestamp time corresponding to the frame to process in the timeline
     * @param xDim the number of charucoBoard squares horizontally
     * @param yDim the number of charucoBoard squares vertically
     * @param squareSize the size of the charucoBoard'square used for calibration
     * @param markerSize the dize of the aruco's markers used for calibration
     * @return The list of charucoBoard points or NULL if no points are detected
     */
    static SPTR(::fwData::PointList) detectCharucoBoard(::arData::FrameTL::csptr tl,
                                                        ::fwCore::HiResClock::HiResClockType timestamp,
                                                        size_t xDim, size_t yDim,
                                                        double squareSize, double markerSize,
                                                        ::arData::FrameTL::sptr tlDetection = 0);

protected:

    /// Configure the service.
    VIDEOCHARUCOCALIBRATION_API void configuring() override;

    /// Does nothing.
    VIDEOCHARUCOCALIBRATION_API void starting() override;

    /// Does nothing.
    VIDEOCHARUCOCALIBRATION_API void updating() override;

    /// Does nothing.
    VIDEOCHARUCOCALIBRATION_API void stopping() override;

private:

    /**
     * @brief SLOT : check if charucoBoard is visible and send corresponding signal
     * @param timestamp timestamp used to gets image frame
     */
    VIDEOCHARUCOCALIBRATION_API void checkPoints(::fwCore::HiResClock::HiResClockType timestamp);

    /**
     * @brief SLOT: Checks on each timeline if points are visible in each frame. Then it add the detected points and the
     * associated image in the CalibrationInfo.
     */
    VIDEOCHARUCOCALIBRATION_API void detectPoints();

    /**
     * @brief SLOT: update the charucoBoard size.
     */
    VIDEOCHARUCOCALIBRATION_API void updateCharucoBoardSize();

    /**
     * @brief Creates an image from frame timeline
     */
    ::fwData::Image::sptr createImage(arData::FrameTL::csptr tl, ::fwCore::HiResClock::HiResClockType timestamp);

    /// Signal emitted when charucoBoard is detected
    CharucoBoardDetectedSignalType::sptr m_sigCharucoBoardDetected;

    /// Signal emitted when charucoBoard is not detected
    CharucoBoardNotDetectedSignalType::sptr m_sigCharucoBoardNotDetected;

    /// Preference key to retrieve width of the charucoBoard used for calibration
    std::string m_widthKey;

    /// Preference key to retrieve height of the charucoBoard used for calibration
    std::string m_heightKey;

    /// Preference key to retrieve height of the charucoBoard used for calibration
    std::string m_squareSizeKey;

    /// Preference key to retrieve height of the charucoBoard used for calibration
    std::string m_markerSizeKey;

    /// Width of the charucoBoard used for calibration
    size_t m_width;

    /// Height of the charucoBoard used for calibration
    size_t m_height;

    /// Used to know if we detected the charucoBoard the last time we check
    bool m_isDetected;

    /// Size of the charucoBoard'square used for calibration
    float m_squareSize;

    /// Size of the aruco's markers used for calibration
    float m_markerSize;

    /// Last valid charucoBoard points for each timeline
    std::vector< ::fwData::PointList::sptr> m_cornerAndIdLists;

    /// Timestamp of the last managed image
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;
};

} //namespace videoCharucoCalibration
