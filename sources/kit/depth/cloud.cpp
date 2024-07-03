#include "cloud.h"
#include "camera.h"
#include <fstream>
#include <iostream>

#include "detectMarkers.h"
#include <opencv2/imgproc/imgproc.hpp>

namespace kit {

Cloud::Cloud(const std::string_view mkv_file, QObject *parent)
    : QObject(parent),
      camera(std::make_unique<kit::Camera>(mkv_file, this))
{
    camera->getNextCapture();
    depthImage = std::make_unique<k4a::image>(camera->getCurCapture().get_depth_image());
    colorImage = std::make_unique<k4a::image>(camera->getCurCapture().get_color_image());

    calcColorTextureBuff();
    generateCloudPoints();
}

std::vector<PointCloud> &Cloud::getPoints()
{
    return points;
}

std::vector<PointCloud> &Cloud::getPointsAruco()
{
    return pointsAruco;
}

void Cloud::clearResurces()
{
    depthImage.reset();
    colorImage->reset();
    colorTextureBuffer.clear();
    colorMarkersBuffer.clear();
    points.clear();
    pointsAruco.clear();
    camera.reset();
//    dm.reset();
}

void Cloud::calcColorTextureBuff()
{
    int cWidth  = colorImage->get_width_pixels();
    int cHeight = colorImage->get_height_pixels();
    int dWidth  = depthImage->get_width_pixels();
    int dHeight = depthImage->get_height_pixels();
    if (dHeight == 0)
        throw std::runtime_error("Нет буфера глубины");
    aspectRatio = 1.0 / (float)(dWidth) / (float)(dHeight);
    int ratio = (dWidth<<16) / dHeight;
    const int new_width = (cWidth - ((cHeight*ratio)>>16))/2;

    colorMarkersBuffer = createBuffMarkers({cWidth, cHeight}, {cWidth, cHeight});   // 368640
    colorTextureBuffer = resizeBuffTexture(reinterpret_cast<Pixel *>(colorImage->get_buffer()),
                                           {cWidth, cHeight},
                                           {cWidth, cHeight});
    colorMarkersBuffer = clipBuffMarkers({new_width, 0});
    colorTextureBuffer = clipBuffTexture  (reinterpret_cast<Pixel *>(colorTextureBuffer.data()),
                                           {cWidth, cHeight},
                                           {new_width, 0});
    colorMarkersBuffer = resizeBuffMarkers({cWidth-(new_width<<1), cHeight}, {dWidth, dHeight});   // 368640
    colorTextureBuffer = resizeBuffTexture(reinterpret_cast<Pixel *>(colorTextureBuffer.data()),
                                           {cWidth-(new_width<<1), cHeight},
                                           {dWidth, dHeight});
}

std::vector<std::pair<int,int>> Cloud::createBuffMarkers(const std::pair<int, int> &from_size, const std::pair<int, int> &to_size)
{
    int w1 = from_size.first;
    int h1 = from_size.second;
    int w2 = to_size.first;
    int h2 = to_size.second;
    int rW = (int)((w2 << 16) / w1) + 1;
    int rH = (int)((h2 << 16) / h1) + 1;
    std::vector<std::pair<int,int>> res;

    cv::Mat img = cv::Mat(colorImage->get_height_pixels(), colorImage->get_width_pixels(), CV_8UC4, colorImage->get_buffer());
    cv::Mat greyMat;
    cv::cvtColor(img, greyMat, cv::COLOR_BGR2GRAY);
    std::unique_ptr<detect::DetectMarkers> dm = std::make_unique<detect::DetectMarkers>(greyMat);
    rotation    = std::get<0>(dm->getMatrix());
    translation = std::get<1>(dm->getMatrix());

    std::array<std::array<float,3>,3> _rotation;
    std::array<float,3> _translation;

    for (auto i=0; i < 3; ++i)
        _translation[i] = reinterpret_cast<float*>(translation.data)[i];
    //        _distCoeffs[i] = reinterpret_cast<float*>(&distCoeffs.data)[i];

    for (auto i=0; i < 3; ++i) {
        for (auto j=0; j < 3; ++j)
            _rotation[j][i] = reinterpret_cast<float*>(rotation.data)[i*3+j];
        //        _rotation[i][j] = reinterpret_cast<float*>(&cameraMatrix.data)[i*3+j];
    }

//    auto [r,t] = dm->getMatrix();
//    for (auto i=0; i < 3; ++i) {
//        _translation[i] = reinterpret_cast<float*>(t.data)[i];
//        for (auto j=0; j < 3; ++j)
//            _rotation[i][j] = reinterpret_cast<float*>(r.data)[i*3+j];
//    }

    std::map<int, std::vector<cv::Point2f>> arucoCorners;
    arucoCorners = dm->getArucoCorners();
    for (auto ac : arucoCorners)
        for (auto c : ac.second) {
            int x = (int(c.x) * rW) >> 16;
            int y = (int(c.y) * rH) >> 16;
            res.emplace_back(std::pair<int,int>{x,y});
        }
    return res;
}

std::vector<std::pair<int,int>> Cloud::resizeBuffMarkers(const std::pair<int, int> &from_size, const std::pair<int, int> &to_size)
{
    int w1 = from_size.first;
    int h1 = from_size.second;
    int w2 = to_size.first;
    int h2 = to_size.second;
    int rW = (int)((w2 << 16) / w1) + 1;
    int rH = (int)((h2 << 16) / h1) + 1;
    for (auto ind=0; ind < colorMarkersBuffer.size(); ++ind) {
        colorMarkersBuffer[ind].first = (colorMarkersBuffer[ind].first * rW) >> 16;
        colorMarkersBuffer[ind].second = (colorMarkersBuffer[ind].second * rH) >> 16;
    }
    return colorMarkersBuffer;
}

std::vector<Pixel> Cloud::resizeBuffTexture(const Pixel *rgba, const std::pair<int, int> &from_size, const std::pair<int, int> &to_size)
{
    int w1 = from_size.first;
    int h1 = from_size.second;
    int w2 = to_size.first;
    int h2 = to_size.second;
    std::vector<Pixel> res(w2*h2);
    int x_ratio = (int)((w1<<16)/w2) +1;
    int y_ratio = (int)((h1<<16)/h2) +1;
    int x, y;
    for (int i=0; i < h2; i++) {
        for (int j=0; j < w2; j++) {
            x = ((j*x_ratio)>>16);
            y = ((i*y_ratio)>>16);
            res[ (i*w2)+j ].Alpha = rgba[(y*w1)+x].Alpha;
            res[ (i*w2)+j ].Red   = rgba[(y*w1)+x].Red;
            res[ (i*w2)+j ].Green = rgba[(y*w1)+x].Green;
            res[ (i*w2)+j ].Blue  = rgba[(y*w1)+x].Blue;
        }
    }
    return res;
}

std::vector<Pixel> Cloud::clipBuffTexture(const Pixel *rgba, const std::pair<int, int> &src_size, const std::pair<int, int> &offset)
{
    int w = src_size.first;
    int h = src_size.second;
    int dw = offset.first;
    int dh = offset.second;
    std::vector<Pixel> res;
    for (int i=dh; i < h-dh; i++) {
        for (int j=dw; j < w-dw; j++) {
            res.push_back({0,0,0,0});
            res[ res.size()-1 ].Alpha = rgba[(i*w)+j].Alpha;
            res[ res.size()-1 ].Red   = rgba[(i*w)+j].Red;
            res[ res.size()-1 ].Green = rgba[(i*w)+j].Green;
            res[ res.size()-1 ].Blue  = rgba[(i*w)+j].Blue;
        }
    }
    return res;
}

std::vector<std::pair<int,int>> Cloud::clipBuffMarkers(const std::pair<int, int> &offset)
{
    int dw = offset.first;
    int dh = offset.second;
    for (auto ind=0; ind < colorMarkersBuffer.size(); ++ind) {
        colorMarkersBuffer[ind].first -= dw;
        colorMarkersBuffer[ind].second -= dh;
    }
    return colorMarkersBuffer;
}

void Cloud::generateCloudPoints()
{
    k4a_image_t xy_table = NULL;
    k4a_image_create(K4A_IMAGE_FORMAT_CUSTOM,
                     depthImage->get_width_pixels(),
                     depthImage->get_height_pixels(),
                     depthImage->get_width_pixels() * sizeof(k4a_float2_t),
                     &xy_table);
    create_xy_table(xy_table);
    generate_point_cloud(xy_table);  // 3d
//    write_point_cloud("points_cloud/test.ply");
}

void Cloud::generate_point_cloud(const k4a_image_t xy_table)
{
    int len = depthImage->get_width_pixels() * depthImage->get_height_pixels();

    uint16_t *depth_data = reinterpret_cast<uint16_t*>(depthImage->get_buffer());
    k4a_float2_t *xy_table_data = (k4a_float2_t *)(void *)k4a_image_get_buffer(xy_table);

    pointsAruco.clear();
    points.clear();
    for (int i = 0; i < len; i++) {
        if (depth_data[i] != 0 && !isnan(xy_table_data[i].xy.x) && !isnan(xy_table_data[i].xy.y)) {
            float x = xy_table_data[i].xy.x * (float)depth_data[i];
            float y = -xy_table_data[i].xy.y * (float)depth_data[i];
            float z = -(float)depth_data[i];

            if (isnan(x) || isnan(y) || isnan(z))
                continue;

            float r,g,b;
            r = float(colorTextureBuffer[i].Red) / 255;
            g = float(colorTextureBuffer[i].Green) / 255;
            b = float(colorTextureBuffer[i].Blue) / 255;
            points.push_back({{r,g,b},{x,y,z}});

            for (auto ind=0; ind < colorMarkersBuffer.size(); ++ind) {
                int index = colorMarkersBuffer[ind].first+colorMarkersBuffer[ind].second*depthImage->get_width_pixels();
                if (index  == i)
                    pointsAruco.push_back({{1.0,0.0,0.0}, {x,y,z}});
            }
        }
    }
}

void Cloud::create_xy_table(const k4a_image_t &xy_table)
{
    k4a_float2_t *table_data = (k4a_float2_t *)(void *)k4a_image_get_buffer(xy_table);
    int width  = depthImage->get_width_pixels();
    int height = depthImage->get_height_pixels();
    k4a_float2_t p;
    k4a_float3_t ray;
    for (int y = 0, idx = 0; y < height; y++) {
        p.xy.y = (float)y;
        for (int x = 0; x < width; x++, idx++) {
            p.xy.x = (float)x;
            int valid;
            k4a_calibration_2d_to_3d(camera->getCalibration(), &p, 1.0f, K4A_CALIBRATION_TYPE_DEPTH, K4A_CALIBRATION_TYPE_DEPTH, &ray, &valid);
            if (valid) {
                table_data[idx].xy.x = ray.xyz.x;
                table_data[idx].xy.y = ray.xyz.y;
            }
            else {
                table_data[idx].xy.x = nanf("");
                table_data[idx].xy.y = nanf("");
            }
        }
    }
}

void Cloud::saveCloud(const std::string_view file_name, const std::vector<PointCloud> &points)
{
    const std::string PLY_START_HEADER  = "ply";
    const std::string PLY_END_HEADER    = "end_header";
    const std::string PLY_ASCII         = "format ascii 1.0";
    const std::string PLY_ELEMENT_VERTEX = "element vertex";

    std::ofstream ofs(file_name.data());
    ofs << PLY_START_HEADER << std::endl;
    ofs << PLY_ASCII << std::endl;
    ofs << PLY_ELEMENT_VERTEX << " " << points.size() << std::endl;
    ofs << "property float x" << std::endl;
    ofs << "property float y" << std::endl;
    ofs << "property float z" << std::endl;
    ofs << "property uchar red" << std::endl;
    ofs << "property uchar green" << std::endl;
    ofs << "property uchar blue" << std::endl;
    ofs << PLY_END_HEADER << std::endl;
    ofs.close();
    std::stringstream ss;
    for (auto i=0; i < points.size(); ++i) {
        ss << points[i].depth[0] << " " << points[i].depth[1] << " " << points[i].depth[2] << " " <<
              points[i].rgb[0]*255 << " " << points[i].rgb[1]*255 << " " << points[i].rgb[2]*255 << std::endl;
    }
    std::ofstream ofs_text(file_name.data(), std::ios::out | std::ios::app);
    ofs_text.write(ss.str().c_str(), (std::streamsize)ss.str().length());
}

void Cloud::memClear()
{
    depthImage->reset();
    colorImage->reset();
    colorTextureBuffer.clear();
    colorMarkersBuffer.clear();
    points.clear();
    pointsAruco.clear();
    camera.reset();
}

float Cloud::getAspectRation() const
{
    return aspectRatio;
}

std::tuple<cv::Mat, cv::Mat> Cloud::getMatrix()
{
    return {rotation, translation};
}

}
