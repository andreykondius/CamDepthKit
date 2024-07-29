#pragma once

#include "cloud.h"
#include "quat.h"
#include <QMainWindow>
#include <QTableWidgetItem>
#include <QtCore/QDir>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    bool event(QEvent *event) override;
    void loadUiPointsCoud(const kit::CloudData &data);
    void setUiPointsCloud(const uint idx);
    void matrixUpdateCurCam();
    void matrixUpdateCurCam(const std::array<std::array<float,4>,4> &matr);
    std::tuple<kit::TQuat,float> calcMat(const std::array<float, 3> &subNorm,
                                         const std::array<float, 3> &normPlane,
                                         const std::array<float, 3> &normRotate);
    void calcRTMatr(std::array<std::array<float,3>,3> &subPoint,
                    const std::array<std::array<float,3>,3> &basePoint);
    void calcTransformMatr(const std::array<std::array<float,3>,3> &subPoint,
                           const std::array<std::array<float,3>,3> &basePoint);
    void calcRotMatr(std::array<std::array<float,3>,3> &subPoint,
                     const std::array<float, 3> &normPlane,
                     const std::array<float, 3> &normRotate);
    void setUiMatrix(const std::array<std::array<float,4>,4> &mat);
    void clearResurces();
    bool addCloudMkv(const QString &fileName);
    void loadMatrix(const QString& fileName);

private:
    uint indItem = 0;
    uint oldIndex = 0;
    Ui::MainWindow *ui;    
    std::map<int, kit::Cloud*> cloud;
    kit::Cloud* _cloud;
    QString fileSave;
    QDir basePath;
    std::map<std::string, kit::CloudData> cloudData;
    std::array<std::array<std::unique_ptr<QTableWidgetItem>,4>,4> modelViewMatr;

private slots:
    void on_action_exit_triggered();
    void on_toolExit_clicked();
    void on_action_add_triggered();
    void on_comboBox_Color_activated(int index);
    void on_pointsColor_activated(int index);
    void on_pointSize_clouds_textChanged(const QString &arg1);
    void on_pointSize_current_textChanged(const QString &arg1);
    void on_rot0_textChanged(const QString &arg1);
    void on_rot1_textChanged(const QString &arg1);
    void on_rot2_textChanged(const QString &arg1);
    void on_tr0_textChanged(const QString &arg1);
    void on_tr1_textChanged(const QString &arg1);
    void on_tr2_textChanged(const QString &arg1);
    void on_checkColor_stateChanged(int arg1);
    void on_listCameras_activated(int index);
    void on_colorRes_activated(int index);
    void on_defaultCam_clicked();
    void on_applyCameras_clicked();
    void on_baseCam_stateChanged(int arg1);
    void on_rotateVel_textChanged(const QString &arg1);
    void on_moveVel_textChanged(const QString &arg1);
    void on_actionSave_Clouds_triggered();
    void on_actionSave_matrix_triggered();
    void on_actionLoad_matrix_triggered();
    void on_get_matrix_clicked();
    void on_action_glue_triggered();
    void on_action_clear_triggered();
    void on_actionAuto_test_triggered();

public slots:
    void onSaveMatr(const float *matr);

};
