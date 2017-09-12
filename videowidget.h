
#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include "videowidgetsurface.h"

#include <QWidget>

class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    VideoWidget(QWidget *parent = 0);
    ~VideoWidget();

    QAbstractVideoSurface *videoSurface() const { return surface; }

    QSize sizeHint() const;
    QSize getFrameSize() const;

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    VideoWidgetSurface *surface;
};

#endif
