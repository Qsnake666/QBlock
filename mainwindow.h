#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QTreeWidget>
#include <QDebug>
#include <QDesktopServices>
#include "Octree.cuh"
#include "Utility.cuh"
#include "Visualization.cuh"
#include "openglwidget.h"
#include <iostream>
#include <fstream>
#include "ioStyle.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //Brief：基本参数结构体
    typedef struct Basic_Para {
        double radius;
        double resolution;
        int level;
        int inputLevel;
    }Basic_Para, * Para_Struct;

    Para_Struct parameters;

    Octree* oct = new Octree;

    Utility* uty = new Utility;

    Visualization* vis = new Visualization;

    Visualization::Matrix sPtcMat;

    Visualization::Matrix sPtcMatT; //转置

    Visualization::Matrix outMat; //输出

    std::vector < Octree*> octMul;

    //Brief：初始化ui
    void initUi();

    //Brief：读取Block文件
    void readBlkFile(std::vector<PointCloud::Point>& pointCloud, QString filepath, QString mode, int code, Para_Struct parameters, QString output);

    //Brief：重置Resolution
    void fromResolution(std::vector<PointCloud::Point> pointCloud, double resolution, double radius, QString output);

    //Brief：重置Radius
    void fromRadius(Octree* oct, Utility* uty, std::vector<PointCloud::Point> pointCloud, PointCloud::Point point, double resolution, double radius, QString output);

    //Brief：重置块体，手都模式Level
    void fromLevel(int level, Octree* oct, Utility* uty, std::vector<PointCloud::Point> pointCloud, PointCloud::Point point, double resolution, double radius, QString output);
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
