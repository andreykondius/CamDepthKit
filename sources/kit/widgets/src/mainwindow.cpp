#include "mainwindow.h"
#include "trigonometry.h"
#include "glwidget.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <array>
#include <fstream>
#include <chrono>
#include <thread>
#include <QMessageBox>

using namespace kit;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , basePath(QDir::currentPath())
{
    ui->setupUi(this);
    for (auto i=0; i < 4; ++i)
        for (auto j=0; j < 4; ++j) {
            modelViewMatr[i][j] = std::make_unique<QTableWidgetItem>("");
        }
    connect(ui->openGLWidget, &GLWidget::toSaveMatr, this, &MainWindow::onSaveMatr);
    QString title = ("CamDepth Kit (v."+std::to_string(PROJECT_MAJOR_VERSION)+"."+
                                            std::to_string(PROJECT_MINOR_VERSION)+"."+
                                            std::to_string(PROJECT_PATCH_VERSION)
                     ).c_str();
    this->setWindowTitle(title);
}

MainWindow::~MainWindow()
{
    clearResurces();
    delete ui;
}

bool MainWindow::event(QEvent *event)
{
    return QWidget::event(event);
}

void MainWindow::on_action_exit_triggered()
{
    close();
}

void MainWindow::on_toolExit_clicked()
{
    on_action_exit_triggered();
}

void MainWindow::on_action_add_triggered()
{
    if (ui->openGLWidget->getCountClouds() == 2) {
        QMessageBox msgBox;
        msgBox.setText("Количество открытых загруженных mkv-файлов - 2. Для добавления новой пары файлов необходимо очистить ресурсы !");
        msgBox.exec();
        return;
    }

    QDir::setCurrent(basePath.absolutePath());
    QDir::setCurrent("files/mkv");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open MKV-file"), QDir::currentPath(), tr("MKV-file (*.mkv)"));
    if (!addCloudMkv(fileName))
        return;
}

void MainWindow::setUiPointsCloud(const uint idx)
{
    std::string deviceName = ui->openGLWidget->getCloudsData()[idx].deviceName;
    cloudData[deviceName].deviceName = deviceName;
    cloudData[deviceName].fBaseCamera = ui->baseCam->isChecked();
    cloudData[deviceName].colorResolution = ui->colorRes->currentIndex();
    cloudData[deviceName].depthMode = ui->depthMode->currentIndex();
    cloudData[deviceName].xRot = ui->rot0->value();
    cloudData[deviceName].yRot = ui->rot1->value();
    cloudData[deviceName].zRot = ui->rot2->value();
    cloudData[deviceName].xPos = ui->tr0->value();
    cloudData[deviceName].yPos = ui->tr1->value();
    cloudData[deviceName].zPos = ui->tr2->value();
    cloudData[deviceName].color.flagColorBuf = ui->checkColor->isChecked();
    cloudData[deviceName].color.indColor = ui->pointsColor->currentIndex();
    cloudData[deviceName].pointSize = ui->pointSize_current->value();
}

void MainWindow::matrixUpdateCurCam()
{
    std::array<std::array<float, 4>, 4> matr = ui->openGLWidget->getMatrixCurDefCloud();
    for (auto i=0; i < 4; ++i)
        for (auto j=0; j < 4; ++j) {
            modelViewMatr[i][j]->setText(std::to_string(matr[i][j]).c_str());
            ui->tab_matrix->setItem(i,j, modelViewMatr[i][j].get());
        }
}

void MainWindow::matrixUpdateCurCam(const std::array<std::array<float, 4>, 4> &matr)
{
    for (auto i=0; i < 4; ++i)
        for (auto j=0; j < 4; ++j) {
            modelViewMatr[i][j]->setText(std::to_string(matr[i][j]).c_str());
            ui->tab_matrix->setItem(i,j, modelViewMatr[i][j].get());
        }
}

void MainWindow::loadUiPointsCoud(const kit::CloudData &data)
{
    std::string deviceName = data.deviceName;
    ui->baseCam->setChecked(data.fBaseCamera);
    ui->colorRes->setCurrentIndex(data.colorResolution);
    ui->depthMode->setCurrentIndex(data.depthMode);
    ui->rot0->setValue(cloudData[deviceName].xRot);
    ui->rot1->setValue(cloudData[deviceName].yRot);
    ui->rot2->setValue(cloudData[deviceName].zRot);
    ui->tr0->setValue(cloudData[deviceName].xPos);
    ui->tr1->setValue(cloudData[deviceName].yPos);
    ui->tr2->setValue(cloudData[deviceName].zPos);
    ui->checkColor->setChecked(cloudData[deviceName].color.flagColorBuf);
    ui->pointsColor->setCurrentIndex(cloudData[deviceName].color.indColor);
    ui->pointSize_current->setValue(cloudData[deviceName].pointSize);
    matrixUpdateCurCam();
    update();
}

void MainWindow::on_colorRes_activated(int index)
{
    ui->openGLWidget->setcolorResolution(index);
}

void MainWindow::on_defaultCam_clicked()
{
    std::string deviceName = ui->openGLWidget->getCloudsData()[ui->openGLWidget->getCurIdx()].deviceName;
    cloudData[deviceName].deviceName = deviceName;
    cloudData[deviceName].fBaseCamera = false;
    cloudData[deviceName].colorResolution = 0;
    cloudData[deviceName].color.flagColorBuf = false;
    cloudData[deviceName].color.indColor = 0;
    cloudData[deviceName].depthMode = 0;
    cloudData[deviceName].xRot = 0;
    cloudData[deviceName].yRot = 0;
    cloudData[deviceName].zRot = 0;
    cloudData[deviceName].xPos = 0;
    cloudData[deviceName].yPos = 0;
    cloudData[deviceName].zPos = 0;
    cloudData[deviceName].pointSize = 1;
    loadUiPointsCoud(cloudData[deviceName]);
}

void MainWindow::on_applyCameras_clicked()
{
    //    setUiPointsCloud
}

void MainWindow::on_baseCam_stateChanged(int arg1)
{
    ui->openGLWidget->setBaseCam(arg1);
}

void MainWindow::on_rotateVel_textChanged(const QString &arg1)
{
    ui->openGLWidget->setRotateVelocity(arg1.toFloat());
}

void MainWindow::on_moveVel_textChanged(const QString &arg1)
{
    ui->openGLWidget->setMoveVelocity(arg1.toInt());
}

void MainWindow::on_actionSave_Clouds_triggered()
{
    QDir::setCurrent(basePath.absolutePath());
    QDir::setCurrent("files/points_cloud");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save ply-file"), QDir::currentPath(), tr("PLY-file (*.ply)"));
    if (fileName.isEmpty()) {
        return;
    }
    uint idx = ui->openGLWidget->getCurIdx();
    cloud[idx]->saveCloud(fileName.toStdString().c_str(), ui->openGLWidget->getCloudsData()[idx].dataCloud);
}

void MainWindow::on_actionSave_matrix_triggered()
{
    if (ui->openGLWidget->getCountClouds() == 0) {
        QMessageBox msgBox;
        msgBox.setText("Не загружено облако. Сохранение матрицы невозможно !");
        msgBox.exec();
        return;
    }
    QDir::setCurrent(basePath.absolutePath());
    QDir::setCurrent("files/matricies");
    fileSave = QFileDialog::getSaveFileName(this, tr("Save mx-file"), QDir::currentPath(), tr("MX-file (*.mx)"));
    if (fileSave.isEmpty()) {
        return;
    }

    ui->openGLWidget->setSaveMatrix(true);
    ui->openGLWidget->setIndSaveMatrix(ui->openGLWidget->getCurIdx());
}

void MainWindow::on_actionLoad_matrix_triggered()
{
    if (ui->openGLWidget->getCountClouds() == 0) {
        QMessageBox msgBox;
        msgBox.setText("Не загружено облако. Загрузка матрицы невозможна !");
        msgBox.exec();
        return;
    }
    QDir::setCurrent(basePath.absolutePath());
    QDir::setCurrent("files/matricies");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open mx-file"), QDir::currentPath(), tr("MX-file (*.mx)"));
    if (fileName.isEmpty()) {
        return;
    }

    loadMatrix(fileName);
}

void MainWindow::on_get_matrix_clicked()
{
    std::array<std::array<float, 4>, 4> matr = ui->openGLWidget->getMatrixCurDefCloud();
    setUiMatrix(matr);
    update();
}

void MainWindow::setUiMatrix(const std::array<std::array<float, 4>, 4> &mat)
{
    for (auto i=0; i < 4; ++i)
        for (auto j=0; j < 4; ++j) {
            modelViewMatr[i][j]->setText(std::to_string(mat[i][j]).c_str());
            ui->tab_matrix->setItem(i,j, modelViewMatr[i][j].get());
        }
    update();
}

std::tuple<TQuat,float> MainWindow::calcMat(const std::array<float, 3> &subNorm,
                                            const std::array<float, 3> &normPlane,
                                            const std::array<float, 3> &normRotate)
{
    float angle = 0.5 * angleDirPlane(subNorm, normPlane);
    TQuat quat;
    quat.rot_quat(angle, normRotate);
    return {quat, angle};
};

void MainWindow::calcRotMatr(std::array<std::array<float,3>,3> &subPoint,
                             const std::array<float, 3> &normPlane,
                             const std::array<float, 3> &normRotate)
{
    std::array<std::array<float,3>,3> mat, oldmat;
    oldmat = ui->openGLWidget->getMatrixCurDefCloud3x3();

    float old_angle = 100.0;
    for (;;) {
        auto [quat, angle] = calcMat(norm(subPoint[1]-subPoint[0]), normPlane, normRotate);

        if (fabs(angle) >= fabs(old_angle) || fabs(angle) < 0.05)
            break;
        old_angle = angle;

        mat = quat.quatTo3x3Mat();
        mat = oldmat * mat;
        oldmat = mat;
        subPoint[0] = mat * subPoint[0];
        subPoint[1] = mat * subPoint[1];
        subPoint[2] = mat * subPoint[2];

        std::array<std::array<float,4>,4> out;
        for (auto i=0; i < 3; ++i)
            for (auto j=0; j < 3; ++j)
                out[i][j] = mat[i][j];
        out[0][3] = 0; out[1][3] = 0; out[2][3] = 0; out[3][0] = 0; out[3][1] = 0; out[3][2] = 0; out[3][3] = 1;

        ui->openGLWidget->setMatrixCurGlueCloud(out);
        matrixUpdateCurCam();
        update();
        ui->openGLWidget->update();
        QCoreApplication::processEvents();
    }
}

void MainWindow::calcTransformMatr(const std::array<std::array<float,3>,3> &subPoint,
                                   const std::array<std::array<float,3>,3> &basePoint)
{
    std::array<std::array<float, 4>, 4> mat = ui->openGLWidget->getMatrixCurGlueCloud();
    mat[0][3] += (basePoint[0][0]-subPoint[0][0]);
    mat[1][3] += (basePoint[0][1]-subPoint[0][1]);
    mat[2][3] += (basePoint[0][2]-subPoint[0][2]);

    ui->openGLWidget->multiMatrCurPoints(mat);
    ui->openGLWidget->multiMatrCurArucoMarkers(mat);
    ui->openGLWidget->setMatrixCurGlueCloud(identityMatr4x4());
    mat = mat * ui->openGLWidget->getMatrixCurDefCloud();
    ui->openGLWidget->setMatrixCurDefCloud(mat);

    update();
    ui->openGLWidget->update();
}

void MainWindow::calcRTMatr(std::array<std::array<float,3>,3> &subPoint,
                            const std::array<std::array<float,3>,3> &basePoint)
{
    // norm of base-plane
    std::array<float, 3> n0 = crossProduct(norm(basePoint[1]-basePoint[0]), norm(basePoint[2]-basePoint[0]));
    std::array<float, 3> n1 = crossProduct(n0, norm(basePoint[1]-basePoint[0]));
    std::array<float, 3> n2 = crossProduct(n0, n1);
    calcRotMatr(subPoint, n1, -n0);         // rotate around {0,0,-1}
    calcRotMatr(subPoint, n0, -n1);         // rotate around {0,1,0}
    calcRotMatr(subPoint, n1, n2);          // rotate around {1,1,0}
    calcTransformMatr(subPoint,basePoint);  // transform
}

void MainWindow::on_action_glue_triggered()     // calc matr of transform
{
    if (ui->openGLWidget->getCountClouds() != 2) {
        QMessageBox msgBox;
        msgBox.setText("Отсутствует вторая камера. Автоматическая склейка невозможна !");
        msgBox.exec();
        return;       
    }
    auto getRT = [&](const cv::Mat &r, const cv::Mat &t)->std::tuple<std::array<std::array<float,3>,3>, std::array<float,3>> {
        std::array<std::array<float,3>,3> rt;
        std::array<float,3> tr;
        for (auto i=0; i < 3; ++i) {
            tr[i] = reinterpret_cast<float*>(t.data)[i];
            for (auto j=0; j < 3; ++j)
                rt[i][j] = reinterpret_cast<float*>(r.data)[i*3+j];
        }
        return {rt,tr};
    };

    auto listPoints = ui->openGLWidget->getCloudsData();
    cv::Mat rt_a,tr_a, rt_b,tr_b;
    std::array<std::array<float,3>,3> rotation_a, rotation_b, temp, rmat_b_to_a;
    std::array<float,3> translation_a, translation_b, tvec_b_to_a;
    uint indBase = 0;
    bool fBasecam = false;
    bool fSubcam = false;
    for (auto it: cloud) {
        int curIdx = it.first;
        if (listPoints[curIdx].fBaseCamera) {
            fBasecam = true;
            indBase = curIdx;
            rt_a = std::get<0>(it.second->getMatrix());
            tr_a = std::get<1>(it.second->getMatrix());
            rotation_a = std::get<0>(getRT(rt_a,tr_a));
            translation_a = std::get<1>(getRT(rt_a,tr_a));
        } else {
            ui->openGLWidget->setColorCurCloud(curIdx);
            fSubcam = true;
            if (ui->listCameras->currentIndex() == indBase) {
                on_listCameras_activated(curIdx);
                ui->baseCam->setChecked(true);
            }
            rt_b = std::get<0>(it.second->getMatrix());
            tr_b = std::get<1>(it.second->getMatrix());
            rotation_b = std::get<0>(getRT(rt_b,tr_b));
            translation_b = std::get<1>(getRT(rt_b,tr_b));
        }
    }

    // to transpose matr b
    temp = rotation_b;
    for (auto i=0; i < 3; ++i)
        for (auto j=0; j < 3; ++j) {
            rotation_b[i][j] = temp[i][j];
        }
    rmat_b_to_a = rotation_a * rotation_b;
    tvec_b_to_a = translation_a - rmat_b_to_a * translation_b;
//    tvec_b_to_a = rmat_b_to_a * translation_b - translation_a;
//    tvec_b_to_a = rmat_b_to_a * translation_b;// - translation_a;

    std::array<std::array<float,4>,4> out;
    for (auto i=0; i < 3; ++i)
        for (auto j=0; j < 3; ++j)
            out[i][j] = rmat_b_to_a[i][j];
    out[3][3] = 1;
    out[0][3] = tvec_b_to_a[0]; out[1][3] = tvec_b_to_a[1]; out[2][3] = tvec_b_to_a[2];
//    out[0][3] = 0.0; out[1][3] = 0.0; out[2][3] = 0.0;
    out[3][0] = 0; out[3][1] = 0; out[3][2] = 0;
//    out[3][0] = tvec_b_to_a[0]; out[3][1] = tvec_b_to_a[1]; out[3][2] = tvec_b_to_a[2];

//    ui->openGLWidget->multiMatrCurPoints(out);
//    ui->openGLWidget->multiMatrCurArucoMarkers(out);

    ui->openGLWidget->removeCurCloud();

    ui->openGLWidget->setMatrixCurGlueCloud(identityMatr4x4());
    out = out * ui->openGLWidget->getMatrixCurDefCloud();
    ui->openGLWidget->setMatrixCurDefCloud(out);

    ui->openGLWidget->update();

//    std::Mat rmat_b_to_a = np.matmul(rmat_a, rmat_b.transpose());
//    std::Mat tvec_b_to_a = -np.matmul(rmat_b_to_a, pose_b.translation) + pose_a.translation

/*    std::map<int, kit::CloudData> listAruco = ui->openGLWidget->getArucoMarkers();
    std::array<std::array<float,3>,3> main, sub;
    // getting 3 points for calc matr of transform
    bool fBasecam = false;
    bool fSubcam = false;
    uint indBase = 0;
    for (auto item : listAruco) {
        if (item.second.fBaseCamera) {
            fBasecam = true;
            indBase = item.first;
            main[0][0] = item.second.dataCloud[0].depth[0];
            main[0][1] = item.second.dataCloud[0].depth[1];
            main[0][2] = item.second.dataCloud[0].depth[2];
            main[1][0] = item.second.dataCloud[1].depth[0];
            main[1][1] = item.second.dataCloud[1].depth[1];
            main[1][2] = item.second.dataCloud[1].depth[2];
            main[2][0] = item.second.dataCloud[2].depth[0];
            main[2][1] = item.second.dataCloud[2].depth[1];
            main[2][2] = item.second.dataCloud[2].depth[2];
        } else {
            fSubcam = true;
            if (ui->listCameras->currentIndex() == indBase) {
                on_listCameras_activated(item.first);
                ui->baseCam->setChecked(true);
            }
            ui->openGLWidget->setCurIdx(item.first);
            sub[0][0] = item.second.dataCloud[0].depth[0];
            sub[0][1] = item.second.dataCloud[0].depth[1];
            sub[0][2] = item.second.dataCloud[0].depth[2];
            sub[1][0] = item.second.dataCloud[1].depth[0];
            sub[1][1] = item.second.dataCloud[1].depth[1];
            sub[1][2] = item.second.dataCloud[1].depth[2];
            sub[2][0] = item.second.dataCloud[2].depth[0];
            sub[2][1] = item.second.dataCloud[2].depth[1];
            sub[2][2] = item.second.dataCloud[2].depth[2];
        }
    }
    if (!fBasecam) {
        QMessageBox msgBox;
        msgBox.setText("Не задана базовая камера. Автоматическая склейка невозможна !");
        msgBox.exec();
        return;
    }
    calcRTMatr(sub, main); */
}

void MainWindow::clearResurces()
{
    indItem = 0;
    ui->listCameras->clear();
    ui->openGLWidget->clearScene();
    ui->gbConfig->setEnabled(false);
    for (;;) {
        if (cloud.size() == 0)
            break;
        auto it = cloud.begin();
        delete it->second;
        cloud.erase(cloud.begin());
    }
}

bool MainWindow::addCloudMkv(const QString& fileName)
{
    if (fileName.isEmpty()) {
        return false;
    }
    uint idx = 0;
    if (ui->listCameras->count()) {
        setUiPointsCloud(ui->openGLWidget->getCountClouds()-1);
    }
    idx = ui->openGLWidget->getCountClouds();
    ui->gbConfig->setEnabled(true);
    oldIndex = idx;
    try {
        cloud[idx] = new kit::Cloud(fileName.toStdString(), this);
    } catch (std::exception &e) {
        QMessageBox msgBox;
        QString text = "Ошибка открытия файла: ";
        text.append(e.what());
        text = text + " !";
        msgBox.setText(text);
        msgBox.exec();
        return false;
    }
    ui->openGLWidget->pushPoints(fileName.toStdString(), cloud[idx]->getPoints());
    kit::Point point{1.0,0.0,0.0};
    ++indItem;
    if (indItem == 2)
        point = {0.0, 1.0, 0.0};
    ui->openGLWidget->pushArucoMarkers(cloud[idx]->getPointsAruco(), point);
    ui->openGLWidget->initGL();
    ui->listCameras->addItem(std::to_string(indItem).c_str());
    ui->listCameras->setCurrentIndex(ui->listCameras->count()-1);
    on_defaultCam_clicked();
    cloud[idx]->memClear();
    return true;
}

void MainWindow::loadMatrix(const QString &fileName)
{
    std::array<std::array<float, 4>, 4> matr;

    std::ifstream ifs(fileName.toStdString(), std::ios_base::in | std::ios_base::binary);
    ifs.read(reinterpret_cast<char*>(&matr[0][0]), 4*4*sizeof(float));
    ifs.close();
    ui->openGLWidget->setMatrixCurDefCloud(matr);

    ui->openGLWidget->multiMatrCurPoints(matr);
    ui->openGLWidget->multiMatrCurArucoMarkers(matr);
    ui->openGLWidget->setMatrixCurGlueCloud(identityMatr4x4());

    matrixUpdateCurCam();
    update();
}

void MainWindow::on_action_clear_triggered()
{
    clearResurces();
}

void MainWindow::onSaveMatr(const float *matr)
{
    std::ofstream ofs(fileSave.toStdString(), std::ios_base::out | std::ios_base::binary);
    std::array<std::array<float,4>, 4> newm;
    for (auto i = 0; i < 4; ++i)
        for (auto j = 0; j < 4; ++j)
            newm[i][j] = reinterpret_cast<const float*>(matr)[j*4+i];
//    ofs.write(reinterpret_cast<char*>(&newm[0][0]), 4*4*sizeof(float));
    ofs.write(reinterpret_cast<const char*>(matr), 4*4*sizeof(float));
    ofs.close();
}


void MainWindow::on_pointSize_clouds_textChanged(const QString &arg1)
{
    ui->openGLWidget->setPointSizeScene(arg1.toFloat());
    for (auto i=0; i < ui->listCameras->count(); ++i) {
        std::string deviceName = ui->openGLWidget->getCloudsData()[i].deviceName;
        cloudData[deviceName].pointSize = arg1.toFloat();
        ui->pointSize_current->setValue(cloudData[deviceName].pointSize);
    }
}

void MainWindow::on_pointSize_current_textChanged(const QString &arg1)
{
    ui->openGLWidget->setPointSizeCur(arg1.toFloat());
}

void MainWindow::on_rot0_textChanged(const QString &arg1)
{
    ui->openGLWidget->setRotxCurCloud(arg1.toFloat());
}

void MainWindow::on_rot1_textChanged(const QString &arg1)
{
    ui->openGLWidget->setRotyCurCloud(arg1.toFloat());
}

void MainWindow::on_rot2_textChanged(const QString &arg1)
{
    ui->openGLWidget->setRotzCurCloud(arg1.toFloat());
}

void MainWindow::on_tr0_textChanged(const QString &arg1)
{
    ui->openGLWidget->setPosxCurCloud(arg1.toFloat());
}

void MainWindow::on_tr1_textChanged(const QString &arg1)
{
    ui->openGLWidget->setPosyCurCloud(arg1.toFloat());
}

void MainWindow::on_tr2_textChanged(const QString &arg1)
{
    ui->openGLWidget->setPoszCurCloud(arg1.toFloat());
}

void MainWindow::on_checkColor_stateChanged(int arg1)
{
    ui->pointsColor->setEnabled(false);
    ui->openGLWidget->setFColorCurIndex(false);
    if (ui->checkColor->isChecked()) {
        ui->pointsColor->setEnabled(true);
        ui->openGLWidget->setFColorCurIndex(true);
    }
}

void MainWindow::on_listCameras_activated(int index)
{
    setUiPointsCloud(oldIndex);
    ui->openGLWidget->setCurIdx(ui->listCameras->currentIndex());
    loadUiPointsCoud( ui->openGLWidget->getCloudsData()[index] );
    oldIndex = index;
    update();
}

void MainWindow::on_comboBox_Color_activated(int index)
{
    ui->openGLWidget->setColorBackground( QColor(ui->comboBox_Color->itemText(index)) );
}

void MainWindow::on_pointsColor_activated(int index)
{
    ui->openGLWidget->setFColorCurIndex(true);
    ui->openGLWidget->setColorCurCloud( QColor(ui->comboBox_Color->itemText(index)) );
}

void MainWindow::on_actionAuto_test_triggered()
{

}

