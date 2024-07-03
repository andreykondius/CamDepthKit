#pragma once
#include <k4arecord/playback.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QObject>

namespace kit {

class Camera  : public QObject
{
public:
    explicit Camera(const std::string_view mkv_file, QObject* parent=0);
    bool isValidPlayback() const;
    k4a::capture getNextCapture() const;
    k4a::capture getPrevCapture() const;
    k4a::capture getCurCapture() const;
    k4a_calibration_t *getCalibration() const;

private:
    void initialization() noexcept;
private:
    k4a_device_configuration_t config;
    std::unique_ptr<k4a::playback> playback;
    std::unique_ptr<k4a_calibration_t> calibration;
    std::unique_ptr<k4a::capture> capture;
};
  
}
