#include "glwidget.h"
#include "tooldrawer.h"
#include <QDebug>
#include <QtWidgets>
#include <QPainter>

#define ZOOMSTEP 1.1

GLWidget::GLWidget(QWidget *parent) :
    QGLWidget(parent)
{
    m_xRot = 0;
    m_yRot = 0;
    m_zoom = 1;

    m_xPan = 0;
    m_yPan = 0;
    m_distance = 100;

    m_xLookAt = 0;
    m_yLookAt = 0;
    m_zLookAt = 0;

    m_xMin = 0;
    m_xMax = 0;
    m_yMin = 0;
    m_yMax = 0;
    m_zMin = 0;
    m_zMax = 0;
    m_xSize = 0;
    m_ySize = 0;
    m_zSize = 0;

    connect(&m_timerAnimation, SIGNAL(timeout()), this, SLOT(onTimerAnimation()));
    m_timerAnimation.start(25);
}

double GLWidget::calculateVolume(QVector3D size) {
    return size.x() * size.y() * size.z();
}

void GLWidget::addDrawable(GLDrawable *drawable)
{
    m_drawables.append(drawable);
}

void GLWidget::fitDrawables()
{
    GLDrawable *bigest = NULL;
    QVector3D maxSize(0, 0, 0);

    foreach (GLDrawable *dr, m_drawables) {
        QVector3D size = dr->getSizes();
        //if (calculateVolume(size) > calculateVolume(maxSize)) {
        if (size.length() > maxSize.length() && strcmp(dr->metaObject()->className(), "ToolDrawer") == -1) {
            maxSize = size;
            bigest = dr;
        }
    }

    if (bigest != NULL) {
//        m_distance = qMax<double>(qMax<double>(maxSize.x(), maxSize.y()), maxSize.z()) * 1.2 + bigest->getMaximumExtremes().z();
        m_distance = qMax<double>(maxSize.x(), maxSize.y()) / 2 / 0.25 * 1.1 + (bigest->getMaximumExtremes().z() - bigest->getMinimumExtremes().z()) / 2;

        m_xLookAt = (bigest->getMaximumExtremes().x() - bigest->getMinimumExtremes().x()) / 2 + bigest->getMinimumExtremes().x();
        m_zLookAt = -((bigest->getMaximumExtremes().y() - bigest->getMinimumExtremes().y()) / 2 + bigest->getMinimumExtremes().y());
        m_yLookAt = (bigest->getMaximumExtremes().z() - bigest->getMinimumExtremes().z()) / 2 + bigest->getMinimumExtremes().z();
//        m_yLookAt = bigest->getMinimumExtremes().z();

        m_xMin = bigest->getMinimumExtremes().x();
        m_xMax = bigest->getMaximumExtremes().x();
        m_yMin = bigest->getMinimumExtremes().y();
        m_yMax = bigest->getMaximumExtremes().y();
        m_zMin = bigest->getMinimumExtremes().z();
        m_zMax = bigest->getMaximumExtremes().z();

        m_xSize = m_xMax - m_xMin;
        m_ySize = m_yMax - m_yMin;
        m_zSize = m_zMax - m_zMin;

    } else {
        m_distance = 200;
        m_xLookAt = 0;
        m_yLookAt = 0;
        m_zLookAt = 0;
    }
//    if (bigest != NULL) {
//        if (strcmp(bigest->metaObject()->className(), "ToolDrawer") != -1) {
//            m_distance = qMax<double>(qMax<double>(maxSize.x(), maxSize.y()), maxSize.z()) * 2;

//            m_xLookAt = (bigest->getMaximumExtremes().x() - bigest->getMinimumExtremes().x()) / 2 + bigest->getMinimumExtremes().x();
//            m_zLookAt = -((bigest->getMaximumExtremes().y() - bigest->getMinimumExtremes().y()) / 2 + bigest->getMinimumExtremes().y());
//            m_yLookAt = (bigest->getMaximumExtremes().z() - bigest->getMinimumExtremes().z()) / 2 + bigest->getMinimumExtremes().z();

//            m_xMin = 0;
//            m_xMax = 0;
//            m_yMin = 0;
//            m_yMax = 0;
//            m_zMin = 0;
//            m_zMax = 0;
//        } else {
//            m_distance = qMax<double>(qMax<double>(maxSize.x(), maxSize.y()), maxSize.z()) * 2;

//            m_xLookAt = (bigest->getMaximumExtremes().x() - bigest->getMinimumExtremes().x()) / 2 + bigest->getMinimumExtremes().x();
//            m_zLookAt = -((bigest->getMaximumExtremes().y() - bigest->getMinimumExtremes().y()) / 2 + bigest->getMinimumExtremes().y());
//            m_yLookAt = (bigest->getMaximumExtremes().z() - bigest->getMinimumExtremes().z()) / 2 + bigest->getMinimumExtremes().z();

//            m_xMin = bigest->getMinimumExtremes().x();
//            m_xMax = bigest->getMaximumExtremes().x();
//            m_yMin = bigest->getMinimumExtremes().y();
//            m_yMax = bigest->getMaximumExtremes().y();
//            m_zMin = bigest->getMinimumExtremes().z();
//            m_zMax = bigest->getMaximumExtremes().z();
//        }
//    } else {
//        m_xLookAt = 0;
//        m_yLookAt = 0;
//        m_zLookAt = 0;

//        m_xMin = 0;
//        m_xMax = 0;
//        m_yMin = 0;
//        m_yMax = 0;
//        m_zMin = 0;
//        m_zMax = 0;

//        m_xPan = 0;
//        m_yPan = 0;

//        m_xRot = 45;
//        m_yRot = 45;

//        m_zoom = 1;
//    }

    m_xSize = m_xMax - m_xMin;
    m_ySize = m_yMax - m_yMin;
    m_zSize = m_zMax - m_zMin;

    m_xPan = 0;
    m_yPan = 0;

    m_xRot = 45;
    m_yRot = 45;

    m_zoom = 1;

    //updateProjection();
    //updateGL();
    update();
}
bool GLWidget::antialiasing() const
{
    return m_antialiasing;
}

void GLWidget::setAntialiasing(bool antialiasing)
{
    m_antialiasing = antialiasing;
}

void GLWidget::onTimerAnimation()
{
    ToolDrawer* tool = static_cast<ToolDrawer*>(m_drawables[1]);

    tool->setRotationAngle(tool->rotationAngle() + M_PI / 25);
    if (tool->rotationAngle() > 2 * M_PI) tool->setRotationAngle(tool->rotationAngle() - 2 * M_PI);

    update();
}


void GLWidget::initializeGL()
{
    //    QGLFormat fmt;
    //    fmt.setSampleBuffers(true);
    //    fmt.setSamples(8); //2, 4, 8, 16
    //    QGLFormat::setDefaultFormat(fmt);

//    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    glEnable(GL_LINE_SMOOTH);
//    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_MULTISAMPLE);
//    //glEnable(GL_CULL_FACE);
//    glShadeModel(GL_SMOOTH);

    GLint bufs;
    GLint samples;
    glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
    glGetIntegerv(GL_SAMPLES, &samples);
    qDebug("Have %d buffers and %d samples", bufs, samples);

//    qglClearColor(QColor(Qt::white));
}

void GLWidget::paintGL()
{    

}

void GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    //updateProjection();
}

void GLWidget::updateProjection()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double asp = (double)width() / height();
    glFrustum ((-0.5 + m_xPan) * asp, (0.5 + m_xPan) * asp, -0.5 + m_yPan, 0.5 + m_yPan, 2, m_distance * 2);
    //glOrtho((-10 + m_xPan) * asp, (10 + m_xPan) * asp, -10 + m_yPan, 10 + m_yPan, 0, m_distance * 2);

    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::paintEvent(QPaintEvent *pe)
{
    makeCurrent();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // Draw 3D
    qglClearColor(QColor(Qt::white));

    if (m_antialiasing) {
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_LINE_SMOOTH);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_MULTISAMPLE);
    updateProjection();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    double r = m_distance;
    double angY = M_PI / 180 * m_yRot;
    double angX = M_PI / 180 * m_xRot;

//    qDebug() << angY << angX << cos(angX) << sin(angX);
//    qDebug() << r * cos(angX) * sin(angY) + m_xLookAt << r * sin(angX) + m_yLookAt << r * cos(angX) * cos(angY) + m_zLookAt;

    gluLookAt(r * cos(angX) * sin(angY) + m_xLookAt, r * sin(angX) + m_yLookAt, r * cos(angX) * cos(angY) + m_zLookAt,
              m_xLookAt, m_yLookAt, m_zLookAt,
              fabs(m_xRot) == 90 ? -sin(angY + (m_xRot < 0 ? M_PI : 0)) : 0, cos(angX), fabs(m_xRot) == 90 ? -cos(angY + (m_xRot < 0 ? M_PI : 0)) : 0);

    glTranslatef(m_xLookAt, m_yLookAt, m_zLookAt);
    glScalef(m_zoom, m_zoom, m_zoom);
    glTranslatef(-m_xLookAt, -m_yLookAt, -m_zLookAt);

    glRotatef(-90, 1.0, 0.0, 0.0);

    glLineWidth(1);

//    glBegin(GL_LINES);
//    glColor3f(1.0, 1.0, 0.0);
//    glVertex3f(0, 0, 0);
//    glVertex3f(m_xLookAt, -m_zLookAt, m_yLookAt);
//    glEnd();

    // Draw X-axis
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(10, 0, 0);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(11, 0, 0);
    glVertex3f(9, 0.5, 0);
    glVertex3f(9, -0.5, 0);
    glVertex3f(11, 0, 0);
    glEnd();

    // Draw Y-axis
    glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 10, 0);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0, 11, 0);
    glVertex3f(-0.5, 9, 0);
    glVertex3f(0.5, 9, 0);
    glVertex3f(0, 11, 0);
    glEnd();

    // Draw Z-axis
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 10);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0, 0, 11);
    glVertex3f(-0.5, 0, 9);
    glVertex3f(0.5, 0, 9);
    glVertex3f(0, 0, 11);
    glEnd();

    // Draw 2x2 XY rect
    glBegin(GL_LINE_STRIP);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(1.0, 1.0, 0.0);
    glVertex3f(-1.0, 1.0, 0.0);
    glVertex3f(-1.0, -1.0, 0.0);
    glVertex3f(1.0, -1.0, 0.0);
    glVertex3f(1.0, 1.0, 0.0);
    glEnd();

    // Draw drawables
    foreach (GLDrawable *dr, m_drawables) dr->draw();

    // Draw 2D
    glShadeModel(GL_FLAT);

    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    QPainter painter(this);
    //painter.setRenderHint(QPainter::Antialiasing);
    //painter.drawText(QPoint(10, 20), "Overpainting text");
    double x = 10;
    double y = this->height() - 60;

    //painter.setPen(Qt::red);

    painter.drawText(QPoint(x, y), QString("X: %1 ... %2").arg(m_xMin, 0, 'f', 3).arg(m_xMax, 0, 'f', 3));
    painter.drawText(QPoint(x, y + 15), QString("Y: %1 ... %2").arg(m_yMin, 0, 'f', 3).arg(m_yMax, 0, 'f', 3));
    painter.drawText(QPoint(x, y + 30), QString("Z: %1 ... %2").arg(m_zMin, 0, 'f', 3).arg(m_zMax, 0, 'f', 3));
    painter.drawText(QPoint(x, y + 45), QString("%1 / %2 / %3").arg(m_xSize, 0, 'f', 3).arg(m_ySize, 0, 'f', 3).arg(m_zSize, 0, 'f', 3));
    painter.end();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
    m_xLastRot = m_xRot;
    m_yLastRot = m_yRot;
    m_xLastPan = m_xPan;
    m_yLastPan = m_yPan;
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::MiddleButton && !(event->modifiers() & Qt::ShiftModifier)) {
        m_yRot = normalizeAngle(m_yLastRot - (event->pos().x() - m_lastPos.x()) * 0.5);
        m_xRot = m_xLastRot + (event->pos().y() - m_lastPos.y()) * 0.5;

        if (m_xRot < -90) m_xRot = -90;
        if (m_xRot > 90) m_xRot = 90;

        //qDebug() << m_yRot << m_xRot;
    }

    if (event->buttons() & Qt::MiddleButton && event->modifiers() & Qt::ShiftModifier) {
        m_xPan = m_xLastPan - (event->pos().x() - m_lastPos.x()) * 1 / (double)width();
        m_yPan = m_yLastPan + (event->pos().y() - m_lastPos.y()) * 1 / (double)height();

        //updateProjection();
    }

    //updateGL();
    update();
}

void GLWidget::wheelEvent(QWheelEvent *we)
{
    if (m_zoom > 0.1 && we->delta() < 0) {
        m_xPan -= ((double)we->pos().x() / width() - 0.5 + m_xPan) * (1 - 1 / ZOOMSTEP);
        m_yPan += ((double)we->pos().y() / height() - 0.5 - m_yPan) * (1 - 1 / ZOOMSTEP);

        m_zoom /= ZOOMSTEP;
    }
    else if (m_zoom < 10 && we->delta() > 0)
    {
        m_xPan -= ((double)we->pos().x() / width() - 0.5 + m_xPan) * (1 - ZOOMSTEP);
        m_yPan += ((double)we->pos().y() / height() - 0.5 - m_yPan) * (1 - ZOOMSTEP);

        m_zoom *= ZOOMSTEP;
    }

//    qDebug() << m_zoom;

    //updateProjection();
    //updateGL();
    update();
}

double GLWidget::normalizeAngle(double angle)
{
    while (angle < 0) angle += 360;
    while (angle > 360) angle -= 360;

    return angle;
}
