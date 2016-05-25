#ifndef GLVIDEOWIDGET_H
#define GLVIDEOWIDGET_H

#include <QApplication>
#include <QGLWidget>
#include <QMutex>
#include <QPixmap>
#include <QStyle>
#include <QScreen>

class GLVideoWidget : public QGLWidget
{
    Q_OBJECT

    bool needsReposition;

    QWidget* main_window;
    QPixmap pixmap;
    QRectF bounds;
public:
    QMutex image_mutex;
    QImage* image;

    GLVideoWidget(QWidget* parent = NULL);
protected:
    void paintEvent(QPaintEvent*);
public slots:
    void ConfigureVideo(const unsigned int width, const unsigned int height, const QImage::Format pixel_format);
    void DelegateUpdate();
};

#endif // GLVIDEOWIDGET_H
