#pragma once
   
#include <json11/json11.hpp>
#include <opencv2/aruco.hpp>
#include <tuple>
#include <map>
#include <k4a/k4a.h>
#include <k4arecord/playback.hpp>

namespace detect {

struct PointMarker
{
    uint x,y;
    cv::Point3f point;
};

class DetectMarkers
{
public:
    DetectMarkers(const cv::Mat &grey);
    void drawMarkers(k4a::image &cImage);
    std::map<int, std::vector<cv::Point2f> > getArucoCorners();
    std::tuple<cv::Mat, cv::Mat> getMatrix();

private:
    std::tuple<std::vector<std::vector<cv::Point2f> >, std::vector<int>, cv::aruco::CharucoBoard> detect_markers(const cv::Mat &img, const std::string_view temp);
    std::tuple<json11::Json, cv::aruco::CharucoBoard> read_board_parameters(const std::string_view json_file);
    std::tuple<cv::Mat,cv::Mat> readCameraParameters();
private:
    cv::Mat image;
    cv::Mat rotation{3, 3, CV_64FC1};
    cv::Mat translation;
    std::map<int, std::vector<cv::Point2f>> arucoMarkers;
};

}
