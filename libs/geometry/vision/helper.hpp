/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2021 IHU Strasbourg
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

#include "geometry/vision/config.hpp"

#include <data/matrix4.hpp>
#include <data/point_list.hpp>
#include <data/vector.hpp>

#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>

#include <vector>

namespace sight::geometry::vision::helper
{

using error_and_points_t = std::pair<double, std::vector<cv::Point2f> >;

/**
 * @brief compute the mean error of reprojection
 * @param _objectPoints : vector of 3d points (to be reprojected)
 * @param _imagePoints: vector of 2d points (reference)
 * @param _rvecs: rotation vector
 * @param _tvecs: translation vector
 * @param _cameraMatrix: camera matrix (fx, fy, cx, cy)
 * @param _distCoeffs: distorsion coefficients of the camera
 * @return std::pair< double, std::vector< cv::Point2f > >, first element is the arithmetical root-squared mean error
 * and the second element is a vector of reprojected 2d points (for display purpose)
 */
GEOMETRY_VISION_API error_and_points_t compute_reprojection_error(
    const std::vector<cv::Point3f>& _object_points,
    const std::vector<cv::Point2f>& _image_points,
    const cv::Mat& _rvecs,
    const cv::Mat& _tvecs,
    const cv::Mat& _camera_matrix,
    const cv::Mat& _dist_coeffs
);

/**
 * @brief cameraPoseMonocular compute pose from monocular camera
 * @param _objectPoints: vector of 3d points corresponding to the object
 * @param _imagePoints: vector of 2d points
 * @param _cameraMatrix: camera matrix (fx, fy, cx, cy)
 * @param _distCoeffs: distorsion coefficients of the camera
 * @param _flag: cv::solvePnP method to use
 * @return cv::Mat (4x4 with float values) corresponding to the camera pose
 */
GEOMETRY_VISION_API cv::Matx44f camera_pose_monocular(
    const std::vector<cv::Point3f>& _object_points,
    const std::vector<cv::Point2f>& _image_points,
    const cv::Mat _camera_matrix,
    const cv::Mat& _dist_coeffs,
    int _flag = cv::SOLVEPNP_ITERATIVE
);

/**
 * @brief cameraPoseStereo compute pose from stereo camera
 * @param _objectPoints: vector of 3d points corresponding to the object
 * @param _cameraMatrix1: camera matrix (fx, fy, cx, cy) for reference camera
 * @param _distCoeffs1: distorsion coefficients of camera1
 * @param _cameraMatrix2: camera matrix (fx, fy, cx, cy) for second camera
 * @param _distCoeffs2: distorsion coefficients of camera2
 * @param _imgPoints1: vector of 2d points in camera 1
 * @param _imgPoints2: vector of 2d points in camera 2
 * @param _R: rotation matrix from camera1 to camera2
 * @param _T: translation vector from camera1 to camera2
 * @return
 */
GEOMETRY_VISION_API cv::Matx44f camera_pose_stereo(
    const std::vector<cv::Point3f>& _object_points,
    const cv::Mat& _camera_matrix1,
    const cv::Mat& _dist_coeffs1,
    const cv::Mat& _camera_matrix2,
    const cv::Mat& _dist_coeffs2,
    const std::vector<cv::Point2f>& _img_points1,
    const std::vector<cv::Point2f>& _img_points2,
    const cv::Mat& _r,
    const cv::Mat& _t
);

/**
 * @brief calibrateTool compute a transformation matrix from tracked device attached to a passive pointing tool and its
 * tip
 * @param _matricesVector: vector containing a set of sight::data::matrix4 from the tracked device
 * @param _calibrationMatrix corresponding to the transformation matrix from the tracked device to the
 * tip of the attached passive pointing device
 * @param _centerMatrix corresponding to the center of the computed sphere used to calibrate the pointing tool
 */
GEOMETRY_VISION_API void calibrate_pointing_tool(
    const sight::data::vector::csptr _matrices_vector,
    sight::data::matrix4::sptr _calibration_matrix,
    sight::data::matrix4::sptr _center_matrix
);

/**
 * @brief Tries to detect a chessboard with the given dimensions in the image.
 *
 * @param[in] _img image in which to search for a chessboard.
 * @param[in] _xDim Width of the chessboard in number of tiles.
 * @param[in] _yDim Height of the chessboard in number of tiles.
 * @param[in] _scale Scale applied to the input image. Downscaling speeds up the detection.
 *
 * @pre _img must have 8bit RGB, RGBA or grayscale pixels.
 *
 * @return List of detected chessboard points. nullptr if detection failed.
 */
GEOMETRY_VISION_API sight::data::point_list::sptr detect_chessboard(
    const cv::Mat& _img,
    std::size_t _x_dim,
    std::size_t _y_dim,
    float _scale
);

} // namespace sight::geometry::vision::helper
