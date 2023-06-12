#include "imgwidget.h"
#include <QPainter>
#include <QTimer>

ImgWidget::ImgWidget(QWidget *parent)
    : QWidget{parent} {
    resize(560, 420);
    buffer = QPixmap(560, 420);
    QPainter painter;
    painter.begin(&buffer);
    painter.setBrush(QBrush(Qt::white));
    painter.drawRect(-1, -1, 562, 422);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setFont(QFont("Microsoft YaHei UI", 32));
    painter.drawText(80, 185, QString("Waiting For Picture."));
    painter.end();
    frameTimer = new QTimer(this);
    frameTimer->setTimerType(Qt::PreciseTimer);
    connect(frameTimer, &QTimer::timeout, this, &ImgWidget::refresh);
    frameTimer->start(20);
}

void ImgWidget::refresh() {
    if (showFlag) {
        if (!imgsBuf.empty()) {
            if (index < imgsBuf.front().size()) {
                QPixmap tmp = QPixmap::fromImage(imgsBuf.front()[index]);
                buffer =
                    QPixmap(tmp.scaledToHeight(height(), Qt::SmoothTransformation));
                ++index;
                if (index == imgsBuf.front().size() - 1) {
                    imgsBuf.pop_front();
                    index = 0;
                }
                update();
            }
        }
    } else {
        frameTimer->stop();
        imgsBuf.clear();
        buffer = QPixmap(560, 420);
        QPainter painter;
        painter.begin(&buffer);
        painter.setBrush(QBrush(Qt::white));
        painter.drawRect(-1, -1, 562, 422);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setFont(QFont("Microsoft YaHei UI", 32));
        painter.setRenderHint(QPainter::Antialiasing);
        painter.drawText(60, 185, QString("Capture Finished."));
        painter.end();
        update();
    }
}

void ImgWidget::pushBuf(const std::vector<QImage> &images) {
    imgsBuf.push_back(images);
}

void ImgWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.drawPixmap(70, 0, buffer.width(), buffer.height(), buffer);
}
