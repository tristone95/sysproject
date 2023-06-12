#ifndef SANDCARD_H
#define SANDCARD_H

#include <QWidget>
#include <global.h>

namespace Ui {
class SandCard;
}

class SandCard : public QWidget {
    Q_OBJECT

public:
    explicit SandCard(SandInfo *info, QWidget *topWind = nullptr);
    ~SandCard();
    void clicked();
    void drawImg();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    Ui::SandCard *ui;
    SandInfo *info;
    QPoint mousePos;
    QWidget *topWindow;
};

#endif // SANDCARD_H
