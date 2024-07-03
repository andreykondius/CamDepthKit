#pragma once

#include "type.h"
#include <k4a/k4a.h>

#include <k4arecord/playback.hpp>
#include <QObject>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/aruco.hpp>

namespace detect {
    class DetectMarkers;
};

namespace kit {

class Camera;
class Cloud : public QObject
{
public:
    explicit Cloud(const std::string_view mkv_file, QObject* parent=0);
    std::vector<PointCloud> &getPoints();
    std::vector<PointCloud> &getPointsAruco();
    void clearResurces();
    void saveCloud(const std::string_view file_name, const std::vector<PointCloud> &points);
    void memClear();
    float getAspectRation() const;
    std::tuple<cv::Mat, cv::Mat> getMatrix();
private:
    void calcColorTextureBuff();
    std::vector<std::pair<int,int> > createBuffMarkers(const std::pair<int,int> &from_size,
                                         const std::pair<int,int> &to_size);
    std::vector<std::pair<int, int> > resizeBuffMarkers(const std::pair<int,int> &from_size,
                                                        const std::pair<int,int> &to_size);
    std::vector<Pixel> resizeBuffTexture(const Pixel *rgba,
                                         const std::pair<int,int> &from_size,
                                         const std::pair<int,int> &to_size);
    std::vector<Pixel> clipBuffTexture  (const Pixel *rgba,
                                         const std::pair<int,int> &src_size,
                                         const std::pair<int,int> &offset);
    std::vector<std::pair<int, int> > clipBuffMarkers(const std::pair<int,int> &offset);
    void generateCloudPoints();
    void generate_point_cloud(const k4a_image_t xy_table);
    void create_xy_table(const k4a_image_t &xy_table);
private:
    float aspectRatio = 1.0;
    std::unique_ptr<k4a::image> depthImage;
    std::unique_ptr<k4a::image> colorImage;
    std::vector<Pixel> colorTextureBuffer;
    std::vector<std::pair<int,int>> colorMarkersBuffer;
    std::vector<PointCloud> points;
    std::vector<PointCloud> pointsAruco;
    std::unique_ptr<Camera> camera;
//    std::array<std::array<float,3>,3> rotation;
//    std::array<float,3> translation;
    cv::Mat rotation;
    cv::Mat translation;
};

}

