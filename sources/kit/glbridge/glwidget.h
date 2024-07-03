#pragma once

#include "type.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QMatrix4x4>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    using QOpenGLWidget::QOpenGLWidget;
    GLWidget();
    ~GLWidget();

    void initGL();
    void removeCurCloud();
    void clearScene();
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    void rotateBy(const float xAngle, const float yAngle, const float zAngle);
    void resetMatrix();
    void pushPoints(const std::string_view deviceName, const std::vector<kit::PointCloud> &a_points);
    void pushArucoMarkers(const std::vector<kit::PointCloud> &a_points, kit::Point val={1,0,0});
    void setBaseCam(const bool val);
    void setcolorResolution(const int idx);
    void setColorBackground(const QColor &color);
    void setColorCurCloud(const QColor &color);
    void setFColorCurIndex(const bool flag);
    void setPointSizeScene(const float sz);
    void setPointSizeCur(const float sz);
    void setRotxCurCloud(const float rot);
    void setRotyCurCloud(const float rot);
    void setRotzCurCloud(const float rot);
    void setPosxCurCloud(const float pos);
    void setPosyCurCloud(const float pos);
    void setPoszCurCloud(const float pos);
    uint getCountClouds() const;
    uint getCurIdx() const;
    void setCurIdx(const uint idx);
    void setRotateVelocity(const float val);
    void setMoveVelocity(const int val);
    std::array<std::array<float, 4>, 4> getMatrixCurDefCloud();
    std::array<std::array<float, 4>, 4> getMatrixCurGlueCloud();
    std::array<std::array<float, 3>, 3> getMatrixCurDefCloud3x3();
    void setMatrixCurDefCloud(const std::array<std::array<float, 4>, 4> &val);
    void setMatrixCurGlueCloud(const std::array<std::array<float, 4>, 4> &val);
    void multiMatrCurPoints(const std::array<std::array<float, 4>, 4> &mat);
    void multiMatrCurArucoMarkers(const std::array<std::array<float, 4>, 4> &mat);
    std::map<int, kit::CloudData> getCloudsData();
    std::map<int, kit::CloudData> getArucoMarkers();
    void setSaveMatrix(const bool val);
    void setIndSaveMatrix(const uint ind);
    void setAspectRatio(const float val);


signals:
    void clicked();

private:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;    
    void keyPressEvent(QKeyEvent *event) override;
    void moveCam(const float dx, const float dz);

private:
    uint countClouds = 0;
    uint curIdx = 0;        // текущий индекс облака
    float aspectRatio = 1.0;
    bool fSaveMatrix = false;
    bool indSaveMatrix = 0;
    QPoint lastPos;

    kit::GlData scene;
    std::map<int, kit::CloudData> cloudsData;
    std::map<int, kit::CloudData> arucoMarkers;
    QMatrix4x4 matrOut;

signals:
    void toSaveMatr(float *matr);

};
