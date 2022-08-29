#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMainWindow>
#include "Visualization.cuh"
#include "glut.h"
#include "signalscollect.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <Utility.cuh>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    // Q_OBJECT
public:
    explicit OpenGLWidget(QWidget *parent = 0);

    Visualization::Matrix mat;
    std::vector <Visualization::Matrix> matMul;

    QPoint lastPos;

    //Brief：左键旋转，滚轮缩放，右键平移
    double rotateX = 0;
    double rotateY = 0;
    double rotateZ = 0;
    double zoom = 1;
    double transX = 0;
    double transY = 0;
    double transZ = 0;
    double radius = 100;
    double preRadius = 100;
    //void radiusChange(double radius,double preRadius);
    //SignalsCollect* sc;

protected:
    //Brief：重写初始化GL虚函数
    void initializeGL() Q_DECL_OVERRIDE;

    //Brief：重写GL窗体大小
    void resizeGL(int w, int h)Q_DECL_OVERRIDE;

    //Brief：重写绘制
    void paintGL()Q_DECL_OVERRIDE;

    //Brief：重写鼠标按下事件
    void mousePressEvent(QMouseEvent *event)Q_DECL_OVERRIDE;

    //Brief：重写鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event)Q_DECL_OVERRIDE;

    //Brief：重写鼠标双击事件
    void mouseDoubleClickEvent(QMouseEvent *event)Q_DECL_OVERRIDE;

    //Brief：重写鼠标滚轮事件
    void wheelEvent(QWheelEvent *event)Q_DECL_OVERRIDE;

    //Brief：绘制坐标轴
    void graficarLineas();
};

#endif // OPENGLWIDGET_H
