#include "glwidget.h"
#include "trigonometry.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <gl/GL.h>

using namespace kit;

GLWidget::GLWidget()
{
    initializeOpenGLFunctions();
}

GLWidget::~GLWidget()
{
    clearScene();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(200, 200);
}

void GLWidget::rotateBy(const float xAngle, const float yAngle, const float zAngle)
{
    scene.xRot += xAngle*scene.rotateVelocity;
    scene.yRot += yAngle*scene.rotateVelocity;
    scene.zRot += zAngle*scene.rotateVelocity;
    update();
}

void GLWidget::resetMatrix()
{
//    cloudsData[curIdx].matrModelView.setToIdentity();
    //    arucoMarkers[curIdx].matrModelView.setToIdentity();
}

void GLWidget::pushPoints(const std::string_view deviceName, const std::vector<kit::PointCloud> &a_points)
{
    ++countClouds;
    curIdx = countClouds-1;
    cloudsData[curIdx].deviceName = deviceName;
    cloudsData[curIdx].defMatrModelView.setToIdentity();
    cloudsData[curIdx].glueMatrModelView.setToIdentity();
    for (auto p : a_points)
        cloudsData[curIdx].dataCloud.emplace_back(p);
}

void GLWidget::pushArucoMarkers(const std::vector<kit::PointCloud> &a_points, kit::Point val)
{
    arucoMarkers[curIdx].defMatrModelView.setToIdentity();
    arucoMarkers[curIdx].glueMatrModelView.setToIdentity();
    for (auto p : a_points) {
        p.rgb[0] = val.point[0];
        p.rgb[1] = val.point[1];
        p.rgb[2] = val.point[2];
        arucoMarkers[curIdx].dataCloud.emplace_back(p);
    }
}

void GLWidget::setBaseCam(const bool val)
{
    cloudsData[curIdx].fBaseCamera = val;
    arucoMarkers[curIdx].fBaseCamera = val;
    if (val) {
        for (auto &it : cloudsData) {
            if (it.first != curIdx)
                it.second.fBaseCamera = false;
        }
        for (auto &it : arucoMarkers) {
            if (it.first != curIdx)
                it.second.fBaseCamera = false;
        }
    }
}

void GLWidget::setcolorResolution(const int idx)
{
    cloudsData[curIdx].colorResolution = idx;
}

void GLWidget::setColorBackground(const QColor &color)
{
    scene.backColor = color;
    update();
}

void GLWidget::setColorCurCloud(const QColor &color)
{
    cloudsData[curIdx].color.Color = color;
    update();
}

void GLWidget::setFColorCurIndex(const bool flag)
{
    cloudsData[curIdx].color.flagColorBuf = flag;
    update();
}

void GLWidget::setPointSizeScene(const float sz)
{
    for (auto idx=0; idx < cloudsData.size(); ++idx) {
        cloudsData[idx].pointSize = sz;
    }
    update();
}

void GLWidget::setPointSizeCur(const float sz)
{
    cloudsData[curIdx].pointSize = sz;
    update();
}

void GLWidget::setRotxCurCloud(const float rot)
{
    cloudsData[curIdx].xRot = rot;
    update();
}

void GLWidget::setRotyCurCloud(const float rot)
{
    cloudsData[curIdx].yRot = rot;
    update();
}

void GLWidget::setRotzCurCloud(const float rot)
{
    cloudsData[curIdx].zRot = rot;
    update();
}

void GLWidget::setPosxCurCloud(const float pos)
{
    cloudsData[curIdx].xPos = pos;
    update();
}

void GLWidget::setPosyCurCloud(const float pos)
{
    cloudsData[curIdx].yPos = pos;
    update();

}

void GLWidget::setPoszCurCloud(const float pos)
{
    cloudsData[curIdx].zPos = pos;
    update();
}

uint GLWidget::getCountClouds() const
{
    return countClouds;
}

uint GLWidget::getCurIdx() const
{
    return curIdx;
}

void GLWidget::setCurIdx(const uint idx)
{
    curIdx = idx;
}

void GLWidget::setRotateVelocity(const float val)
{
    scene.rotateVelocity = val;
}

void GLWidget::setMoveVelocity(const int val)
{
    scene.moveVelocity = val;
}

std::array<std::array<float, 4>, 4> GLWidget::getMatrixCurDefCloud()
{
    std::array<std::array<float, 4>, 4> res;
    for (auto i=0; i < 4; ++i)
        for (auto j=0; j < 4; ++j) {
            res[i][j] = cloudsData[curIdx].defMatrModelView(i,j);
        }
    return res;
}

std::array<std::array<float, 4>, 4> GLWidget::getMatrixCurGlueCloud()
{
    std::array<std::array<float, 4>, 4> res;
    for (auto i=0; i < 4; ++i)
        for (auto j=0; j < 4; ++j) {
            res[i][j] = cloudsData[curIdx].glueMatrModelView(i,j);
        }
    return res;
}

std::array<std::array<float, 3>, 3> GLWidget::getMatrixCurDefCloud3x3()
{
    std::array<std::array<float, 3>, 3> res;
    for (auto i=0; i < 3; ++i)
        for (auto j=0; j < 3; ++j) {
            res[i][j] = cloudsData[curIdx].defMatrModelView(i,j);
        }
    return res;
}

void GLWidget::setMatrixCurDefCloud(const std::array<std::array<float, 4>, 4> &val)
{
    for (auto i=0; i < 4; ++i) {
        QVector4D v(val[i][0], val[i][1], val[i][2], val[i][3]);
        cloudsData[curIdx].defMatrModelView.setRow(i, v);
    }
}

void GLWidget::setMatrixCurGlueCloud(const std::array<std::array<float, 4>, 4> &val)
{
    for (auto i=0; i < 4; ++i) {
        QVector4D v(val[i][0], val[i][1], val[i][2], val[i][3]);
        cloudsData[curIdx].glueMatrModelView.setRow(i, v);
    }
}

void GLWidget::multiMatrCurPoints(const std::array<std::array<float, 4>, 4> &mat)
{
    for (auto &p: cloudsData[curIdx].dataCloud) {
        std::array<float, 4> temp;
        temp = mat * std::array<float, 4>{p.depth[0], p.depth[1], p.depth[2], 1.0};
        p.depth[0] = temp[0];
        p.depth[1] = temp[1];
        p.depth[2] = temp[2];
    }
}

void GLWidget::multiMatrCurArucoMarkers(const std::array<std::array<float, 4>, 4> &mat)
{
    for (auto &p: arucoMarkers[curIdx].dataCloud) {
        std::array<float, 4> temp;
        temp = mat * std::array<float, 4>{p.depth[0], p.depth[1], p.depth[2], 1.0};
        p.depth[0] = temp[0];
        p.depth[1] = temp[1];
        p.depth[2] = temp[2];
    }
}

std::map<int, kit::CloudData> GLWidget::getCloudsData()
{
    return cloudsData;
}

std::map<int, kit::CloudData> GLWidget::getArucoMarkers()
{
    return arucoMarkers;
}

void GLWidget::setSaveMatrix(const bool val)
{
    fSaveMatrix = val;
    update();
}

void GLWidget::setIndSaveMatrix(const uint ind)
{
    indSaveMatrix = ind;
}

void GLWidget::setAspectRatio(const float val)
{
    aspectRatio = val;
}

void GLWidget::initGL()
{
    const char *vsrc =
        "attribute highp vec3 position;\n"
        "attribute lowp vec3 rgb;\n"
        "attribute lowp float pointSize;\n"
        "uniform mediump mat4 matrixCloud;\n"
        "uniform mediump mat4 matrixScene;\n"
        "varying lowp vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    color = vec4(rgb, 1.0);\n"
        "    gl_PointSize = pointSize;\n"
        "    gl_Position = matrixScene * matrixCloud * vec4(position, 1.0);\n"
        "}\n";

    const char *fsrc =
            "varying lowp vec4 color;\n"
            "void main(void)\n"
            "{\n"
            "    gl_FragColor = color;\n"
            "}\n";

    auto setShader = [&](std::map<int, kit::CloudData> &container) {
        container[curIdx].program = new QOpenGLShaderProgram;
        container[curIdx].program->addShaderFromSourceCode(QOpenGLShader::Vertex, vsrc);
        container[curIdx].program->addShaderFromSourceCode(QOpenGLShader::Fragment, fsrc);
        container[curIdx].program->link();

        container[curIdx].positionAttr = container[curIdx].program->attributeLocation("position");
        container[curIdx].rgbAttr = container[curIdx].program->attributeLocation("rgb");
        container[curIdx].psAttr = container[curIdx].program->attributeLocation("pointSize");

        container[curIdx].matrixSceneAttr = container[curIdx].program->uniformLocation("matrixScene");
        container[curIdx].matrixAttr = container[curIdx].program->uniformLocation("matrixCloud");

    };
    setShader(cloudsData);
    setShader(arucoMarkers);
}

void GLWidget::removeCurCloud()
{
    delete cloudsData[curIdx].program;
    delete arucoMarkers[curIdx].program;
    cloudsData.erase(curIdx);
}

void GLWidget::clearScene()
{
    makeCurrent();
    for (auto idx=0; idx < cloudsData.size(); ++idx) {
        delete cloudsData[idx].program;
    }
    for (auto idx=0; idx < arucoMarkers.size(); ++idx) {
        delete arucoMarkers[idx].program;
    }
    cloudsData.clear();
    arucoMarkers.clear();
    countClouds = 0;
    curIdx = 0;
    doneCurrent();
    update();
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    QOpenGLWidget::initializeGL();
}

void GLWidget::paintGL()
{
    if (!cloudsData.size())
        return;
    glClearColor(scene.backColor.redF(), scene.backColor.greenF(), scene.backColor.blueF(), scene.backColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_PROGRAM_POINT_SIZE);

    QMatrix4x4 mScene;
    mScene.perspective(90, aspectRatio, 0.01, 10000.0);
//    mScene.ortho(-1000.0, 1000.0, -1000.0, 1000.0, 0.01, 10000.0);
    mScene.rotate(scene.xRot, 1.0f, 0.0f, 0.0f);
    mScene.rotate(scene.yRot, 0.0f, 1.0f, 0.0f);
    mScene.rotate(scene.zRot, 0.0f, 0.0f, 1.0f);
    mScene.translate(scene.xPos, scene.yPos, scene.zPos);

    for (auto idx=0; idx < cloudsData.size(); ++idx) {

//        cloudsData[idx].matrModelView.setToIdentity();// = cloudsData[idx].defMatrModelView;
        QMatrix4x4 matrModelView = cloudsData[idx].glueMatrModelView;
        matrModelView.rotate(cloudsData[idx].xRot, 1.0f, 0.0f, 0.0f);
        matrModelView.rotate(cloudsData[idx].yRot, 0.0f, 1.0f, 0.0f);
        matrModelView.rotate(cloudsData[idx].zRot, 0.0f, 0.0f, 1.0f);
        matrModelView.translate(cloudsData[idx].xPos, cloudsData[idx].yPos, cloudsData[idx].zPos);
        if (fSaveMatrix && indSaveMatrix == idx) {
            fSaveMatrix = false;
            std::array<std::array<float,4>,4> mv, defm;
            for (auto i = 0; i < 4; ++i)
                for (auto j = 0; j < 4; ++j) {
                    mv[i][j] = matrModelView.data()[j*4+i];
                    defm[i][j] = cloudsData[idx].defMatrModelView.data()[j*4+i];
                }
            emit toSaveMatr(&(mv*defm)[0][0]);
        }

        // arucoMarkers
        arucoMarkers[idx].program->bind();
        arucoMarkers[idx].program->setUniformValue(arucoMarkers[idx].matrixSceneAttr, mScene);
        arucoMarkers[idx].program->setUniformValue(arucoMarkers[idx].matrixAttr, matrModelView);
        arucoMarkers[idx].program->setAttributeValue(arucoMarkers[idx].psAttr, 10);      // size of point = 4
        glVertexAttribPointer(arucoMarkers[idx].positionAttr, 3, GL_FLOAT, GL_FALSE, sizeof(kit::PointCloud), &arucoMarkers[idx].dataCloud[0].depth[0]);
        glVertexAttribPointer(arucoMarkers[idx].rgbAttr, 3, GL_FLOAT, GL_FALSE, sizeof(kit::PointCloud), &arucoMarkers[idx].dataCloud[0].rgb[0]);
        glEnableVertexAttribArray(arucoMarkers[idx].rgbAttr);
        glEnableVertexAttribArray(arucoMarkers[idx].positionAttr);
        glDrawArrays(GL_POINTS, 0, arucoMarkers[idx].dataCloud.size());
        glDisableVertexAttribArray(arucoMarkers[idx].rgbAttr);
        glDisableVertexAttribArray(arucoMarkers[idx].positionAttr);
        arucoMarkers[idx].program->release();

        // pointsCloud
        cloudsData[idx].program->bind();
        cloudsData[idx].program->setUniformValue(cloudsData[idx].matrixSceneAttr, mScene);
        cloudsData[idx].program->setUniformValue(cloudsData[idx].matrixAttr, matrModelView);
        cloudsData[idx].program->setAttributeValue(cloudsData[idx].psAttr, cloudsData[idx].pointSize);
        glUniform1f(cloudsData[idx].psAttr, cloudsData[idx].pointSize);
        glVertexAttribPointer(cloudsData[idx].positionAttr, 3, GL_FLOAT, GL_FALSE, sizeof(kit::PointCloud), &cloudsData[idx].dataCloud[0].depth[0]);
        glVertexAttribPointer(cloudsData[idx].rgbAttr, 3, GL_FLOAT, GL_FALSE, sizeof(kit::PointCloud), &cloudsData[idx].dataCloud[0].rgb[0]);
        glEnableVertexAttribArray(cloudsData[idx].rgbAttr);
        if (cloudsData[idx].color.flagColorBuf) {
            QVector3D rgb = {float(qRed(cloudsData[idx].color.Color.rgb())) / 255.0f,
                             float(qGreen(cloudsData[idx].color.Color.rgb())) / 255.0f,
                             float(qBlue(cloudsData[idx].color.Color.rgb())) / 255.0f};
            cloudsData[idx].program->setAttributeValue(cloudsData[idx].rgbAttr, rgb);
            glDisableVertexAttribArray(cloudsData[idx].rgbAttr);
        }

        glEnableVertexAttribArray(cloudsData[idx].positionAttr);

        glDrawArrays(GL_POINTS, 0, cloudsData[idx].dataCloud.size());

        glDisableVertexAttribArray(cloudsData[idx].rgbAttr);
        glDisableVertexAttribArray(cloudsData[idx].positionAttr);
        cloudsData[idx].program->release();
    }
}
void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_W) {
        moveCam(0.0, scene.moveVelocity);
    } else if (event->key() == Qt::Key_S) {
        moveCam(0.0, -scene.moveVelocity);
    } else if (event->key() == Qt::Key_A) {
        moveCam(-scene.moveVelocity, 0.0);
    } else if (event->key() == Qt::Key_D) {
        moveCam(scene.moveVelocity, 0.0);
    } else if (event->key() == Qt::Key_Q) {
        scene.yPos += scene.moveVelocity;
    } else if (event->key() == Qt::Key_E) {
        scene.yPos -= scene.moveVelocity;
    }
    update();
}

void GLWidget::moveCam(const float dx, const float dz)
{
    scene.xPos -= dx * cos(scene.yRot*3.1415/180) + dz*sin(scene.yRot*3.1415/180);
    scene.zPos += -dx * sin(scene.yRot*3.1415/180) + dz*cos(scene.yRot*3.1415/180);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    this->setFocus();
    lastPos = event->position().toPoint();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->position().toPoint().x() - lastPos.x();
    int dy = event->position().toPoint().y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        rotateBy(dy, dx, 0);
    } else if (event->buttons() & Qt::RightButton) {
        rotateBy(dy, 0, dx);
    }
    lastPos = event->position().toPoint();
}
