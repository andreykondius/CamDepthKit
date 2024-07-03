#pragma once

#include <QColor>
#include <QPoint>
#include <map>
#include <vector>
#include <QOpenGLContext>
#include <QMatrix4x4>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

namespace kit {

struct Point {
    float point[3];
};

struct Pixel
{
    unsigned char Blue;
    unsigned char Green;
    unsigned char Red;
    unsigned char Alpha;
};

struct PointCloud
{
    float rgb[3];
    float depth[3];
};

struct CloudData
{
    struct Color {
        bool flagColorBuf=false;
        int indColor;
        QColor Color;
    } color;

    bool fBaseCamera = false;
    float xRot = 0.0;
    float yRot = 0.0;
    float zRot = 0.0;
    float xPos = 0.0;
    float yPos = 0.0;
    float zPos = 0.0;
    int pointSize = 1;
    GLint positionAttr  = 0;
    GLint rgbAttr       = 0;
    GLint matrixSceneAttr    = 0;
    GLint matrixAttr    = 0;
    GLint psAttr = 0;
    GLint colorAttr = 0;
    int colorResolution;
    int depthMode;
    QOpenGLShaderProgram *program = nullptr;
    std::string deviceName;
    std::vector<kit::PointCloud> dataCloud;
    QMatrix4x4 defMatrModelView;    
    QMatrix4x4 glueMatrModelView;
};

struct GlData
{
    float xRot = 0.0;
    float yRot = 0.0;
    float zRot = 0.0;
    float xPos = 0.0;
    float yPos = 0.0;
    float zPos = 0.0;
    float rotateVelocity = 1;
    float moveVelocity = 100;
    QColor backColor = Qt::gray;
};

}
