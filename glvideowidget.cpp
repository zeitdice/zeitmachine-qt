#include "glvideowidget.h"

GLVideoWidget::GLVideoWidget(QWidget* parent)
    : QGLWidget(parent)
{
    image = NULL;
}

void GLVideoWidget::paintEvent(QPaintEvent*)
{
    if(image != NULL) {
        QPainter painter(this);

        image_mutex.lock();
        pixmap.convertFromImage(*image);
        painter.drawPixmap(bounds, pixmap, bounds);
        image_mutex.unlock();
    }
}

void GLVideoWidget::ConfigureVideo(const unsigned int width, const unsigned int height, const QImage::Format pixel_format)
{
    bounds.setRect(0, 0, width, height);

    setFixedWidth(width);
    setFixedHeight(height);

    image = new QImage(width, height, pixel_format);
}

void GLVideoWidget::DelegateUpdate()
{
    update();
}
