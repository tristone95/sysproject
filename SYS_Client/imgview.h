#ifndef IMGVIEW_H
#define IMGVIEW_H

#include <QGraphicsView>

class ImgView : public QGraphicsView {
    Q_OBJECT
public:
    explicit ImgView(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    bool noScaleFlag = true; // 自适应大小后是否缩放标志
};

#endif // IMGVIEW_H
