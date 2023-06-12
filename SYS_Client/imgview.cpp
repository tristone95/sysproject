#include "imgview.h"
#include <QApplication>
#include <QWheelEvent>

ImgView::ImgView(QWidget *parent)
    : QGraphicsView(parent) {
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void ImgView::wheelEvent(QWheelEvent *event) {
    if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
        // 置缩放标志为false,表明进行了缩放
        if (noScaleFlag)
            noScaleFlag = false;
        double scaled = this->viewportTransform().m11();
        // 进行缩放
        int step = event->angleDelta().y();
        if (step > 0 && scaled < 5) {
            scale(1.2, 1.2);
        } else if (step < 0 && scaled > 0.08) {
            scale(0.8, 0.8);
        }
    } else
        QAbstractScrollArea::wheelEvent(event);
}

void ImgView::resizeEvent(QResizeEvent *event) {
    if (noScaleFlag)
        fitInView(0, 0, 1785, 2515, Qt::KeepAspectRatio);
}
