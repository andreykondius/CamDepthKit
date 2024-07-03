#include "camera.h"

namespace kit {

Camera::Camera(const std::string_view mkv_file, QObject *parent) :
    QObject(parent),
    playback(std::make_unique<k4a::playback>(k4a::playback::open(&mkv_file[0]))),
    calibration(std::make_unique<k4a_calibration_t>(playback->get_calibration()))
{
    initialization();
    capture = std::make_unique<k4a::capture>();
}

bool Camera::isValidPlayback() const
{
    return playback->is_valid();
}

k4a::capture Camera::getNextCapture() const
{
    playback->get_next_capture(capture.get());
    return *capture;
}

k4a::capture Camera::getPrevCapture() const
{
    playback->get_previous_capture(capture.get());
    return *capture;
}

k4a::capture Camera::getCurCapture() const
{
    return *capture;
}

k4a_calibration_t *Camera::getCalibration() const
{
    return reinterpret_cast<k4a_calibration_t*>(calibration.get());
}

void Camera::initialization() noexcept
{
    config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    config.camera_fps = K4A_FRAMES_PER_SECOND_30;
    config.depth_mode = K4A_DEPTH_MODE_WFOV_2X2BINNED;
    config.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;
    config.color_resolution = K4A_COLOR_RESOLUTION_3072P;
    config.synchronized_images_only = true;
}

}
