#include "detectMarkers.h"

#include <fstream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>

namespace detect {

DetectMarkers::DetectMarkers(const cv::Mat &grey)
    : image(grey)
{
    cv::Mat image = grey;
    std::string temp = "files/json/plane.json";
    auto [corners, ids, board] = detect_markers(image, temp);
    if (ids.size() > 0) {
        std::vector<cv::Point2f> charucoCorners;
        std::vector<int> charucoIds;
        std::string filename = "calib.txt";
        auto [distCoeffs, cameraMatrix] = readCameraParameters();

        cv::Ptr<cv::aruco::CharucoBoard> ptrChAruCoboard = new cv::aruco::CharucoBoard(board);

        cv::aruco::interpolateCornersCharuco(corners, ids, image, ptrChAruCoboard, charucoCorners, charucoIds);//, cameraMatrix, distCoeffs);
        if (charucoIds.size()) {

            int id = -1;
            for (auto a : charucoCorners) {
                ++id;
                arucoMarkers[id].push_back(a);
            }

            cv::Scalar color = cv::Scalar(255, 0, 0);
            cv::Vec3f rvec, tvec;
            bool valid = cv::aruco::estimatePoseCharucoBoard(charucoCorners, charucoIds, ptrChAruCoboard, cameraMatrix, distCoeffs, rvec, tvec);
            if (valid) {
                cv::Rodrigues(rvec, rotation);
                for (auto i=0; i < 3; ++i) {
                    translation.push_back(tvec[i]);
                }
            }
        }
    }
}

void DetectMarkers::drawMarkers(k4a::image &cImage)
{
    //    cv::aruco::drawDetectedMarkers(cImage, corners, ids);
}

std::map<int, std::vector<cv::Point2f>> DetectMarkers::getArucoCorners()
{
    return arucoMarkers;
}

std::tuple<cv::Mat, cv::Mat> DetectMarkers::getMatrix()
{
    return {rotation, translation};
}

std::tuple<std::vector<std::vector<cv::Point2f> >,
           std::vector<int>,
           cv::aruco::CharucoBoard> DetectMarkers::detect_markers(const cv::Mat &img, const std::string_view temp)
{
    auto [target, board] = read_board_parameters(temp);
    cv::aruco::DetectorParameters params = cv::aruco::DetectorParameters();
    params.cornerRefinementMethod = cv::aruco::CORNER_REFINE_NONE;
//    cv::Point3f points = board.getChessboardCorners();
    cv::aruco::ArucoDetector det(board.getDictionary(), params);
    std::vector<int> aruco_ids;
    std::vector<std::vector<cv::Point2f>> aruco_corners;
    det.detectMarkers(img, aruco_corners, aruco_ids);
    return {aruco_corners, aruco_ids, board};
}

std::tuple<json11::Json, cv::aruco::CharucoBoard> DetectMarkers::read_board_parameters(const std::string_view json_file)
{
    std::stringstream ss;
    std::ifstream is(&json_file[0]);
    ss << is.rdbuf();
    std::string err;
    json11::Json target = json11::Json::parse(ss.str(), err);
    if (target.is_null()) {
        return {target, {}};
    }
    auto shape = target["shapes"].array_items()[0];
    cv::Size sz(shape["squares_x"].int_value(), shape["squares_y"].int_value());
    cv::aruco::CharucoBoard board(cv::Size(int(shape["squares_x"].number_value()), int(shape["squares_y"].number_value())),
            shape["square_length"].number_value(),
            shape["marker_length"].number_value(),
            cv::aruco::getPredefinedDictionary(shape["aruco_dict_name"].number_value()));
    return {target, board};
}

std::tuple<cv::Mat,cv::Mat> DetectMarkers::readCameraParameters()
{
    cv::Mat distCoeffs(8, 1, CV_32F);
    cv::Mat cameraMatrix(3, 3, CV_32F);
    std::ifstream is("files/mkv/calib_matr.txt");
    if (!is.is_open())
        return {distCoeffs, cameraMatrix};
    std::string line;
    for (auto i=0; i < 8; ++i) {
        std::getline(is, line);
        float a;
        is >> a;
        distCoeffs.at<float>(i,0) = a;
    }
    std::getline(is, line);
    for (auto i=0; i < 3; ++i)
        for (auto j=0; j < 3; ++j) {
            std::getline(is, line);
            cv::Point3f p;
            float a;
            is >> a;
            cameraMatrix.at<float>(i,j) = a;
        }

    std::array<std::array<float,3>,3> _rotation;
    std::array<float,8> _distCoeffs;

    for (auto i=0; i < 8; ++i)
        _distCoeffs[i] = reinterpret_cast<float*>(distCoeffs.data)[i];

    for (auto i=0; i < 3; ++i) {
        for (auto j=0; j < 3; ++j)
            _rotation[j][i] = reinterpret_cast<float*>(cameraMatrix.data)[i*3+j];
    }

    return {distCoeffs, cameraMatrix};
}

}
