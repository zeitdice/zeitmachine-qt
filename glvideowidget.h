#ifndef GLVIDEOWIDGET_H
#define GLVIDEOWIDGET_H

#include <QGLWidget>
#include <QMutex>
#include <QPixmap>

class GLVideoWidget : public QGLWidget
{
    Q_OBJECT

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
