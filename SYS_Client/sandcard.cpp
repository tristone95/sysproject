#include "sandcard.h"
#include "sandinfodialog.h"
#include "ui_sandcard.h"
#include <QMouseEvent>
#include <QPainter>

SandCard::SandCard(SandInfo *info, QWidget *topWind)
    : ui(new Ui::SandCard) {
    ui->setupUi(this);
    ui->imgWidget->installEventFilter(this);
    this->info = info;
    topWindow = topWind;
    ui->label_5->setText(QString::number(info->granularity, 'f', 3));
    ui->label_6->setText(QString::number(info->roundness, 'f', 3));
    ui->label_7->setText(QString::number(info->sphericity, 'f', 3));
}

SandCard::~SandCard() {
    delete ui;
}

void SandCard::clicked() {
    SandInfoDialog showDialog(this->info, topWindow);
    showDialog.exec();
}

void SandCard::drawImg() {
    QPainter painter(ui->imgWidget);
    painter.drawImage((100 - info->image->width()) / 2,
                      (100 - info->image->height()) / 2, *info->image);
    painter.end();
}

bool SandCard::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->imgWidget && event->type() == QEvent::Paint)
        drawImg();
    return QObject::eventFilter(watched, event);
}

void SandCard::mousePressEvent(QMouseEvent *event) { mousePos = event->pos(); }

void SandCard::mouseReleaseEvent(QMouseEvent *event) {
    if (mousePos == event->pos())
        clicked();
}
