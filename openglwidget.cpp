#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget *parent):QOpenGLWidget(parent)
{

}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.53, 0.81, 0.92, 0.0);//背景

    glShadeModel(GL_SMOOTH);//启用阴影平滑

    glClearDepth(1.0);//设置深度缓存

    glEnable(GL_DEPTH_TEST);//执行深度比较并更新深度缓冲区
}

void OpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);//左下角0，0 视频区域 长w 宽h
    glMatrixMode(GL_PROJECTION);//选择投影矩阵
    glLoadIdentity();//重置投影矩阵
    //设置视口的大小
    //https://blog.csdn.net/zhouschina/article/details/16821735
    gluPerspective(75, (GLfloat)w/(GLfloat)h, 0.1, 100000.0);
    glMatrixMode(GL_MODELVIEW);//选择模型观察矩阵
    glLoadIdentity();//重置投影矩阵
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event) {
    if (Qt::LeftButton == event->button()) {
        lastPos = event->position().toPoint();
    }
    else if (Qt::RightButton == event->button()) {
        lastPos = event->position().toPoint();
    }
}

void OpenGLWidget::mouseDoubleClickEvent(QMouseEvent *event) {

}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
    if (Qt::LeftButton & event->buttons()) {
        GLfloat dx = event->position().toPoint().x() - lastPos.x();
        GLfloat dy = event->position().toPoint().x() - lastPos.y();
        rotateX += dy * 0.01;
        rotateY += dx * 0.01;
        rotateZ += dy * 0.01;
        this->update();
     }
    else if (Qt::RightButton & event->buttons()) {
        GLfloat dx = event->position().toPoint().x() - lastPos.x();
        GLfloat dy = event->position().toPoint().x() - lastPos.y();
        transX += dy * 0.001;
        transY += dx * 0.001;
        this->update();
    }
    lastPos = event->position().toPoint();
}

void OpenGLWidget::wheelEvent(QWheelEvent *event) {
    GLfloat degrees = event->angleDelta().y(); //新版本Qt已经废除delta()由angleData().y()代替
    zoom -= degrees * 0.2f;

    //注意, 如果zoom为负值, eye就会被放置到z轴的负方向, 而从物体的背面看了.
    if (zoom < 1.0f) {
        zoom = 1.0f;
    }
    if (zoom > radius * 1.45){
        preRadius = radius;
        radius *= 2;
        //radiusChange(radius, preRadius);
    }
    this->update();
}

/*
void OpenGLWidget::radiusChange(double radius,double preRadius){
    if (radius != preRadius){
       // emit sc->radiusChanged();
    }
}
*/

void OpenGLWidget::paintGL(){
    //清除屏幕和Z缓冲区
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);//清除屏幕和深度缓存
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glLoadIdentity(); //重置当前的模型观察矩阵

    gluLookAt(0.0f, 0.0f, zoom, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(rotateX, 1, 0, 0);
    glRotatef(rotateY, 0, 1, 0);
    glRotatef(rotateZ, 0, 0, 1);
    glTranslated(transX, transY, transZ);

    if (matMul.size() == 0){
        for(int i = 0; i < this->mat.height; i+=9){
            if (this->mat.elements[i* this->mat.stride + 3] == MIX_ATTRIBUTE){
                //Mix mode
                //Front
                glBegin(GL_POLYGON);
                glColor3f( 1.0, 0.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 8) * this->mat.stride + 0], this->mat.elements[(i + 8) * this->mat.stride + 1],
                        this->mat.elements[(i + 8) * this->mat.stride + 2] );
                glColor3f( 0.0, 1.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 5) * this->mat.stride + 0], this->mat.elements[(i + 5) * this->mat.stride + 1],
                        this->mat.elements[(i + 5) * this->mat.stride + 2] );
                glColor3f( 0.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 6) * this->mat.stride + 0], this->mat.elements[(i + 6) * this->mat.stride + 1],
                        this->mat.elements[(i + 6) * this->mat.stride + 2] );
                glColor3f( 1.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 7) * this->mat.stride + 0], this->mat.elements[(i + 7) * this->mat.stride + 1],
                        this->mat.elements[(i + 7) * this->mat.stride + 2] );
                glEnd();

                //Back
                glBegin(GL_POLYGON);
                glColor3f( 1.0, 0.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 4) * this->mat.stride + 0], this->mat.elements[(i + 4) * this->mat.stride + 1],
                        this->mat.elements[(i + 4) * this->mat.stride + 2] );
                glColor3f( 0.0, 1.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 1) * this->mat.stride + 0], this->mat.elements[(i + 1) * this->mat.stride + 1],
                        this->mat.elements[(i + 1) * this->mat.stride + 2] );
                glColor3f( 0.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 2) * this->mat.stride + 0], this->mat.elements[(i + 2) * this->mat.stride + 1],
                        this->mat.elements[(i + 2) * this->mat.stride + 2] );
                glColor3f( 1.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 3) * this->mat.stride + 0], this->mat.elements[(i + 3) * this->mat.stride + 1],
                        this->mat.elements[(i + 3) * this->mat.stride + 2] );
                glEnd();

                //Right
                glBegin(GL_POLYGON);
                glColor3f( 1.0, 0.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 8) * this->mat.stride + 0], this->mat.elements[(i + 8) * this->mat.stride + 1],
                        this->mat.elements[(i + 8) * this->mat.stride + 2] );
                glColor3f( 0.0, 1.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 5) * this->mat.stride + 0], this->mat.elements[(i + 5) * this->mat.stride + 1],
                        this->mat.elements[(i + 5) * this->mat.stride + 2] );
                glColor3f( 0.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 1) * this->mat.stride + 0], this->mat.elements[(i + 1) * this->mat.stride + 1],
                        this->mat.elements[(i + 1) * this->mat.stride + 2] );
                glColor3f( 1.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 4) * this->mat.stride + 0], this->mat.elements[(i + 4) * this->mat.stride + 1],
                        this->mat.elements[(i + 4) * this->mat.stride + 2] );
                glEnd();

                //Left
                glBegin(GL_POLYGON);
                glColor3f( 1.0, 0.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 3) * this->mat.stride + 0], this->mat.elements[(i + 3) * this->mat.stride + 1],
                        this->mat.elements[(i + 3) * this->mat.stride + 2] );
                glColor3f( 0.0, 1.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 2) * this->mat.stride + 0], this->mat.elements[(i + 2) * this->mat.stride + 1],
                        this->mat.elements[(i + 2) * this->mat.stride + 2] );
                glColor3f( 0.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 6) * this->mat.stride + 0], this->mat.elements[(i + 6) * this->mat.stride + 1],
                        this->mat.elements[(i + 6) * this->mat.stride + 2] );
                glColor3f( 1.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 7) * this->mat.stride + 0], this->mat.elements[(i + 7) * this->mat.stride + 1],
                        this->mat.elements[(i + 7) * this->mat.stride + 2] );
                glEnd();

                //Top
                glBegin(GL_POLYGON);
                glColor3f( 1.0, 0.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 1) * this->mat.stride + 0], this->mat.elements[(i + 1) * this->mat.stride + 1],
                        this->mat.elements[(i + 1) * this->mat.stride + 2] );
                glColor3f( 0.0, 1.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 5) * this->mat.stride + 0], this->mat.elements[(i + 5) * this->mat.stride + 1],
                        this->mat.elements[(i + 5) * this->mat.stride + 2] );
                glColor3f( 0.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 6) * this->mat.stride + 0], this->mat.elements[(i + 6) * this->mat.stride + 1],
                        this->mat.elements[(i + 6) * this->mat.stride + 2] );
                glColor3f( 1.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 2) * this->mat.stride + 0], this->mat.elements[(i + 2) * this->mat.stride + 1],
                        this->mat.elements[(i + 2) * this->mat.stride + 2] );
                glEnd();

                //Button
                glBegin(GL_POLYGON);
                glColor3f( 1.0, 0.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 8) * this->mat.stride + 0], this->mat.elements[(i + 8) * this->mat.stride + 1],
                        this->mat.elements[(i + 8) * this->mat.stride + 2] );
                glColor3f( 0.0, 1.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 4) * this->mat.stride + 0], this->mat.elements[(i + 4) * this->mat.stride + 1],
                        this->mat.elements[(i + 4) * this->mat.stride + 2] );
                glColor3f( 0.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 3) * this->mat.stride + 0], this->mat.elements[(i + 3) * this->mat.stride + 1],
                        this->mat.elements[(i + 3) * this->mat.stride + 2] );
                glColor3f( 1.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 7) * this->mat.stride + 0], this->mat.elements[(i + 7) * this->mat.stride + 1],
                        this->mat.elements[(i + 7) * this->mat.stride + 2] );
                glEnd();
            }
            else{
                //Front
                glBegin(GL_POLYGON);
                float a = this->mat.elements[i* this->mat.stride + 3] / 200.f ;
                glColor3f( 5 * a, 3 * a, 2 * a);
                //glColor3f( 1.0, 0.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 8) * this->mat.stride + 0], this->mat.elements[(i + 8) * this->mat.stride + 1],
                        this->mat.elements[(i + 8) * this->mat.stride + 2] );
                //glColor3f( 0.0, 1.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 5) * this->mat.stride + 0], this->mat.elements[(i + 5) * this->mat.stride + 1],
                        this->mat.elements[(i + 5) * this->mat.stride + 2] );
                //glColor3f( 0.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 6) * this->mat.stride + 0], this->mat.elements[(i + 6) * this->mat.stride + 1],
                        this->mat.elements[(i + 6) * this->mat.stride + 2] );
                //glColor3f( 1.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 7) * this->mat.stride + 0], this->mat.elements[(i + 7) * this->mat.stride + 1],
                        this->mat.elements[(i + 7) * this->mat.stride + 2] );
                glEnd();

                //Back
                glBegin(GL_POLYGON);
                glColor3f( 5 * a, 3 * a, 2 * a);
                //glColor3f( 1.0, 0.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 4) * this->mat.stride + 0], this->mat.elements[(i + 4) * this->mat.stride + 1],
                        this->mat.elements[(i + 4) * this->mat.stride + 2] );
                //glColor3f( 0.0, 1.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 1) * this->mat.stride + 0], this->mat.elements[(i + 1) * this->mat.stride + 1],
                        this->mat.elements[(i + 1) * this->mat.stride + 2] );
                //glColor3f( 0.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 2) * this->mat.stride + 0], this->mat.elements[(i + 2) * this->mat.stride + 1],
                        this->mat.elements[(i + 2) * this->mat.stride + 2] );
                //glColor3f( 1.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 3) * this->mat.stride + 0], this->mat.elements[(i + 3) * this->mat.stride + 1],
                        this->mat.elements[(i + 3) * this->mat.stride + 2] );
                glEnd();

                //Right
                glBegin(GL_POLYGON);
                glColor3f( 5 * a, 3 * a, 2 * a);
                //glColor3f( 1.0, 0.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 8) * this->mat.stride + 0], this->mat.elements[(i + 8) * this->mat.stride + 1],
                        this->mat.elements[(i + 8) * this->mat.stride + 2] );
                //glColor3f( 0.0, 1.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 5) * this->mat.stride + 0], this->mat.elements[(i + 5) * this->mat.stride + 1],
                        this->mat.elements[(i + 5) * this->mat.stride + 2] );
                //glColor3f( 0.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 1) * this->mat.stride + 0], this->mat.elements[(i + 1) * this->mat.stride + 1],
                        this->mat.elements[(i + 1) * this->mat.stride + 2] );
                //glColor3f( 1.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 4) * this->mat.stride + 0], this->mat.elements[(i + 4) * this->mat.stride + 1],
                        this->mat.elements[(i + 4) * this->mat.stride + 2] );
                glEnd();

                //Left
                glBegin(GL_POLYGON);
                glColor3f( 5 * a, 3 * a, 2 * a);
                //glColor3f( 1.0, 0.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 3) * this->mat.stride + 0], this->mat.elements[(i + 3) * this->mat.stride + 1],
                        this->mat.elements[(i + 3) * this->mat.stride + 2] );
                //glColor3f( 0.0, 1.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 2) * this->mat.stride + 0], this->mat.elements[(i + 2) * this->mat.stride + 1],
                        this->mat.elements[(i + 2) * this->mat.stride + 2] );
                //glColor3f( 0.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 6) * this->mat.stride + 0], this->mat.elements[(i + 6) * this->mat.stride + 1],
                        this->mat.elements[(i + 6) * this->mat.stride + 2] );
                //glColor3f( 1.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 7) * this->mat.stride + 0], this->mat.elements[(i + 7) * this->mat.stride + 1],
                        this->mat.elements[(i + 7) * this->mat.stride + 2] );
                glEnd();

                //Top
                glBegin(GL_POLYGON);
                glColor3f( 5 * a, 3 * a, 2 * a);
                //glColor3f( 1.0, 0.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 1) * this->mat.stride + 0], this->mat.elements[(i + 1) * this->mat.stride + 1],
                        this->mat.elements[(i + 1) * this->mat.stride + 2] );
                //glColor3f( 0.0, 1.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 5) * this->mat.stride + 0], this->mat.elements[(i + 5) * this->mat.stride + 1],
                        this->mat.elements[(i + 5) * this->mat.stride + 2] );
                //glColor3f( 0.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 6) * this->mat.stride + 0], this->mat.elements[(i + 6) * this->mat.stride + 1],
                        this->mat.elements[(i + 6) * this->mat.stride + 2] );
                //glColor3f( 1.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 2) * this->mat.stride + 0], this->mat.elements[(i + 2) * this->mat.stride + 1],
                        this->mat.elements[(i + 2) * this->mat.stride + 2] );
                glEnd();

                //Button
                glBegin(GL_POLYGON);
                glColor3f( 5 * a, 3 * a, 2 * a);
                //glColor3f( 1.0, 0.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 8) * this->mat.stride + 0], this->mat.elements[(i + 8) * this->mat.stride + 1],
                        this->mat.elements[(i + 8) * this->mat.stride + 2] );
                //glColor3f( 0.0, 1.0, 0.0 );
                glVertex3d(this->mat.elements[(i + 4) * this->mat.stride + 0], this->mat.elements[(i + 4) * this->mat.stride + 1],
                        this->mat.elements[(i + 4) * this->mat.stride + 2] );
                //glColor3f( 0.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 3) * this->mat.stride + 0], this->mat.elements[(i + 3) * this->mat.stride + 1],
                        this->mat.elements[(i + 3) * this->mat.stride + 2] );
                //glColor3f( 1.0, 0.0, 1.0 );
                glVertex3d(this->mat.elements[(i + 7) * this->mat.stride + 0], this->mat.elements[(i + 7) * this->mat.stride + 1],
                        this->mat.elements[(i + 7) * this->mat.stride + 2] );
                glEnd();
            }
        }
    }
    else if(matMul.size() > 0){
        for (int j = 0; j < matMul.size(); j++){
            this->mat = matMul[j];
            for(int i = 0; i < this->mat.height; i+=9){
                if (this->mat.elements[i* this->mat.stride + 3] == MIX_ATTRIBUTE){
                    //Mix mode
                    //Front
                    glBegin(GL_POLYGON);
                    glColor3f( 1.0, 0.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 8) * this->mat.stride + 0], this->mat.elements[(i + 8) * this->mat.stride + 1],
                            this->mat.elements[(i + 8) * this->mat.stride + 2] );
                    glColor3f( 0.0, 1.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 5) * this->mat.stride + 0], this->mat.elements[(i + 5) * this->mat.stride + 1],
                            this->mat.elements[(i + 5) * this->mat.stride + 2] );
                    glColor3f( 0.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 6) * this->mat.stride + 0], this->mat.elements[(i + 6) * this->mat.stride + 1],
                            this->mat.elements[(i + 6) * this->mat.stride + 2] );
                    glColor3f( 1.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 7) * this->mat.stride + 0], this->mat.elements[(i + 7) * this->mat.stride + 1],
                            this->mat.elements[(i + 7) * this->mat.stride + 2] );
                    glEnd();
                    //Back
                    glBegin(GL_POLYGON);
                    glColor3f( 1.0, 0.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 4) * this->mat.stride + 0], this->mat.elements[(i + 4) * this->mat.stride + 1],
                            this->mat.elements[(i + 4) * this->mat.stride + 2] );
                    glColor3f( 0.0, 1.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 1) * this->mat.stride + 0], this->mat.elements[(i + 1) * this->mat.stride + 1],
                            this->mat.elements[(i + 1) * this->mat.stride + 2] );
                    glColor3f( 0.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 2) * this->mat.stride + 0], this->mat.elements[(i + 2) * this->mat.stride + 1],
                            this->mat.elements[(i + 2) * this->mat.stride + 2] );
                    glColor3f( 1.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 3) * this->mat.stride + 0], this->mat.elements[(i + 3) * this->mat.stride + 1],
                            this->mat.elements[(i + 3) * this->mat.stride + 2] );
                    glEnd();

                    //Right
                    glBegin(GL_POLYGON);
                    glColor3f( 1.0, 0.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 8) * this->mat.stride + 0], this->mat.elements[(i + 8) * this->mat.stride + 1],
                            this->mat.elements[(i + 8) * this->mat.stride + 2] );
                    glColor3f( 0.0, 1.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 5) * this->mat.stride + 0], this->mat.elements[(i + 5) * this->mat.stride + 1],
                            this->mat.elements[(i + 5) * this->mat.stride + 2] );
                    glColor3f( 0.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 1) * this->mat.stride + 0], this->mat.elements[(i + 1) * this->mat.stride + 1],
                            this->mat.elements[(i + 1) * this->mat.stride + 2] );
                    glColor3f( 1.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 4) * this->mat.stride + 0], this->mat.elements[(i + 4) * this->mat.stride + 1],
                            this->mat.elements[(i + 4) * this->mat.stride + 2] );
                    glEnd();

                    //Left
                    glBegin(GL_POLYGON);
                    glColor3f( 1.0, 0.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 3) * this->mat.stride + 0], this->mat.elements[(i + 3) * this->mat.stride + 1],
                            this->mat.elements[(i + 3) * this->mat.stride + 2] );
                    glColor3f( 0.0, 1.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 2) * this->mat.stride + 0], this->mat.elements[(i + 2) * this->mat.stride + 1],
                            this->mat.elements[(i + 2) * this->mat.stride + 2] );
                    glColor3f( 0.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 6) * this->mat.stride + 0], this->mat.elements[(i + 6) * this->mat.stride + 1],
                            this->mat.elements[(i + 6) * this->mat.stride + 2] );
                    glColor3f( 1.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 7) * this->mat.stride + 0], this->mat.elements[(i + 7) * this->mat.stride + 1],
                            this->mat.elements[(i + 7) * this->mat.stride + 2] );
                    glEnd();

                    //Top
                    glBegin(GL_POLYGON);
                    glColor3f( 1.0, 0.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 1) * this->mat.stride + 0], this->mat.elements[(i + 1) * this->mat.stride + 1],
                            this->mat.elements[(i + 1) * this->mat.stride + 2] );
                    glColor3f( 0.0, 1.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 5) * this->mat.stride + 0], this->mat.elements[(i + 5) * this->mat.stride + 1],
                            this->mat.elements[(i + 5) * this->mat.stride + 2] );
                    glColor3f( 0.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 6) * this->mat.stride + 0], this->mat.elements[(i + 6) * this->mat.stride + 1],
                            this->mat.elements[(i + 6) * this->mat.stride + 2] );
                    glColor3f( 1.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 2) * this->mat.stride + 0], this->mat.elements[(i + 2) * this->mat.stride + 1],
                            this->mat.elements[(i + 2) * this->mat.stride + 2] );
                    glEnd();

                    //Button
                    glBegin(GL_POLYGON);
                    glColor3f( 1.0, 0.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 8) * this->mat.stride + 0], this->mat.elements[(i + 8) * this->mat.stride + 1],
                            this->mat.elements[(i + 8) * this->mat.stride + 2] );
                    glColor3f( 0.0, 1.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 4) * this->mat.stride + 0], this->mat.elements[(i + 4) * this->mat.stride + 1],
                            this->mat.elements[(i + 4) * this->mat.stride + 2] );
                    glColor3f( 0.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 3) * this->mat.stride + 0], this->mat.elements[(i + 3) * this->mat.stride + 1],
                            this->mat.elements[(i + 3) * this->mat.stride + 2] );
                    glColor3f( 1.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 7) * this->mat.stride + 0], this->mat.elements[(i + 7) * this->mat.stride + 1],
                            this->mat.elements[(i + 7) * this->mat.stride + 2] );
                    glEnd();
                }
                else{
                    //Front
                    glBegin(GL_POLYGON);
                    float a = this->mat.elements[i* this->mat.stride + 3] / 200.f ;
                    glColor3f( 5 * a, 3 * a, 2 * a);
                    //glColor3f( 1.0, 0.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 8) * this->mat.stride + 0], this->mat.elements[(i + 8) * this->mat.stride + 1],
                            this->mat.elements[(i + 8) * this->mat.stride + 2] );
                    //glColor3f( 0.0, 1.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 5) * this->mat.stride + 0], this->mat.elements[(i + 5) * this->mat.stride + 1],
                            this->mat.elements[(i + 5) * this->mat.stride + 2] );
                    //glColor3f( 0.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 6) * this->mat.stride + 0], this->mat.elements[(i + 6) * this->mat.stride + 1],
                            this->mat.elements[(i + 6) * this->mat.stride + 2] );
                    //glColor3f( 1.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 7) * this->mat.stride + 0], this->mat.elements[(i + 7) * this->mat.stride + 1],
                            this->mat.elements[(i + 7) * this->mat.stride + 2] );
                    glEnd();

                    //Back
                    glBegin(GL_POLYGON);
                    glColor3f( 5 * a, 3 * a, 2 * a);
                    //glColor3f( 1.0, 0.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 4) * this->mat.stride + 0], this->mat.elements[(i + 4) * this->mat.stride + 1],
                            this->mat.elements[(i + 4) * this->mat.stride + 2] );
                    //glColor3f( 0.0, 1.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 1) * this->mat.stride + 0], this->mat.elements[(i + 1) * this->mat.stride + 1],
                            this->mat.elements[(i + 1) * this->mat.stride + 2] );
                    //glColor3f( 0.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 2) * this->mat.stride + 0], this->mat.elements[(i + 2) * this->mat.stride + 1],
                            this->mat.elements[(i + 2) * this->mat.stride + 2] );
                    //glColor3f( 1.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 3) * this->mat.stride + 0], this->mat.elements[(i + 3) * this->mat.stride + 1],
                            this->mat.elements[(i + 3) * this->mat.stride + 2] );
                    glEnd();

                    //Right
                    glBegin(GL_POLYGON);
                    glColor3f( 5 * a, 3 * a, 2 * a);
                    //glColor3f( 1.0, 0.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 8) * this->mat.stride + 0], this->mat.elements[(i + 8) * this->mat.stride + 1],
                            this->mat.elements[(i + 8) * this->mat.stride + 2] );
                    //glColor3f( 0.0, 1.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 5) * this->mat.stride + 0], this->mat.elements[(i + 5) * this->mat.stride + 1],
                            this->mat.elements[(i + 5) * this->mat.stride + 2] );
                    //glColor3f( 0.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 1) * this->mat.stride + 0], this->mat.elements[(i + 1) * this->mat.stride + 1],
                            this->mat.elements[(i + 1) * this->mat.stride + 2] );
                    //glColor3f( 1.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 4) * this->mat.stride + 0], this->mat.elements[(i + 4) * this->mat.stride + 1],
                            this->mat.elements[(i + 4) * this->mat.stride + 2] );
                    glEnd();

                    //Left
                    glBegin(GL_POLYGON);
                    glColor3f( 5 * a, 3 * a, 2 * a);
                    //glColor3f( 1.0, 0.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 3) * this->mat.stride + 0], this->mat.elements[(i + 3) * this->mat.stride + 1],
                            this->mat.elements[(i + 3) * this->mat.stride + 2] );
                    //glColor3f( 0.0, 1.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 2) * this->mat.stride + 0], this->mat.elements[(i + 2) * this->mat.stride + 1],
                            this->mat.elements[(i + 2) * this->mat.stride + 2] );
                    //glColor3f( 0.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 6) * this->mat.stride + 0], this->mat.elements[(i + 6) * this->mat.stride + 1],
                            this->mat.elements[(i + 6) * this->mat.stride + 2] );
                    //glColor3f( 1.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 7) * this->mat.stride + 0], this->mat.elements[(i + 7) * this->mat.stride + 1],
                            this->mat.elements[(i + 7) * this->mat.stride + 2] );
                    glEnd();

                    //Top
                    glBegin(GL_POLYGON);
                    glColor3f( 5 * a, 3 * a, 2 * a);
                    //glColor3f( 1.0, 0.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 1) * this->mat.stride + 0], this->mat.elements[(i + 1) * this->mat.stride + 1],
                            this->mat.elements[(i + 1) * this->mat.stride + 2] );
                    //glColor3f( 0.0, 1.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 5) * this->mat.stride + 0], this->mat.elements[(i + 5) * this->mat.stride + 1],
                            this->mat.elements[(i + 5) * this->mat.stride + 2] );
                    //glColor3f( 0.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 6) * this->mat.stride + 0], this->mat.elements[(i + 6) * this->mat.stride + 1],
                            this->mat.elements[(i + 6) * this->mat.stride + 2] );
                    //glColor3f( 1.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 2) * this->mat.stride + 0], this->mat.elements[(i + 2) * this->mat.stride + 1],
                            this->mat.elements[(i + 2) * this->mat.stride + 2] );
                    glEnd();

                    //Button
                    glBegin(GL_POLYGON);
                    glColor3f( 5 * a, 3 * a, 2 * a);
                    //glColor3f( 1.0, 0.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 8) * this->mat.stride + 0], this->mat.elements[(i + 8) * this->mat.stride + 1],
                            this->mat.elements[(i + 8) * this->mat.stride + 2] );
                    //glColor3f( 0.0, 1.0, 0.0 );
                    glVertex3d(this->mat.elements[(i + 4) * this->mat.stride + 0], this->mat.elements[(i + 4) * this->mat.stride + 1],
                            this->mat.elements[(i + 4) * this->mat.stride + 2] );
                    //glColor3f( 0.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 3) * this->mat.stride + 0], this->mat.elements[(i + 3) * this->mat.stride + 1],
                            this->mat.elements[(i + 3) * this->mat.stride + 2] );
                    //glColor3f( 1.0, 0.0, 1.0 );
                    glVertex3d(this->mat.elements[(i + 7) * this->mat.stride + 0], this->mat.elements[(i + 7) * this->mat.stride + 1],
                            this->mat.elements[(i + 7) * this->mat.stride + 2] );
                    glEnd();
                }
            }
        }
    }
    graficarLineas();
}

void OpenGLWidget::graficarLineas()
{
    //坐标轴
    glBegin(GL_LINES);
    glColor3f(0,1,0);
    glVertex3f(0,0,0);
    glVertex3f(20,0,0);

    glColor3f(0,1,0);
    glVertex3f(0,0,0);
    glVertex3f(0,20,0);

    glColor3f(0,1,0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,20);
    glEnd();
}
