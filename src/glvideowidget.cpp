#include "glvideowidget.h"

GLVideoWidget::GLVideoWidget(QWidget* parent)
    : QGLWidget(parent)
{
    image = NULL;
    main_window = parent;
    needsReposition = false;
}

void GLVideoWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    if(image == NULL) {
        bounds.setRect(0, 0, this->width(), this->height());
        painter.fillRect(bounds, QColor(0, 0, 0));
    } else {
        image_mutex.lock();
        pixmap.convertFromImage(*image);
        painter.drawPixmap(bounds, pixmap, bounds);
        image_mutex.unlock();
    }
}

void GLVideoWidget::ConfigureVideo(const unsigned int width, const unsigned int height, const QImage::Format pixel_format)
{
    bounds.setRect(0, 0, width, height);

    setFixedSize(width, height);

    image_mutex.lock();
    image = new QImage(width, height, pixel_format);
    image_mutex.unlock();

    needsReposition = true;
}

void GLVideoWidget::DelegateUpdate()
{
    update();

    if(needsReposition) {
        main_window->setGeometry(
            QStyle::alignedRect(Qt::LeftToRight,
                                Qt::AlignCenter,
                                main_window->size(),
                                QApplication::primaryScreen()->availableGeometry())
        );

        needsReposition = false;
    }
}
