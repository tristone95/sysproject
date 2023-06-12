#include "sandinfodialog.h"
#include "global.h"
#include "ui_sandinfodialog.h"
#include <QPainter>

SandInfoDialog::SandInfoDialog(SandInfo *info, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SandInfoDialog) {
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    // 此对象只在栈中创建，故出栈析构，而DeleteOnClose又会析构
    // 就会导致RTTI symbol not found for class 'QObject'
    // setAttribute(Qt::WA_DeleteOnClose);

    ui->imgWidget->installEventFilter(this); // 安装事件过滤器

    img = info->image;
    ui->l1->setText(QString("%1%2").arg(QString::number(info->minRectLength, 'f', 3), 9, ' ').arg("μm"));
    ui->l2->setText(QString("%1%2").arg(QString::number(info->minRectWidth, 'f', 3), 9, ' ').arg("μm"));
    ui->l3->setText(QString("%1%2").arg(QString::number(info->perimeter, 'f', 3), 9, ' ').arg("μm"));
    ui->l4->setText(QString("%1%2").arg(QString::number(info->area, 'f', 3), 9, ' ').arg("μm²"));
    ui->l5->setText(QString("%1%2").arg(QString::number(info->pEDiameter, 'f', 3), 9, ' ').arg("μm"));
    ui->l6->setText(QString("%1%2").arg(QString::number(info->aEDiameter, 'f', 3), 9, ' ').arg("μm"));
    ui->l7->setText(QString("%1%2").arg(QString::number(info->minCDiameter, 'f', 3), 9, ' ').arg("μm"));
    ui->l8->setText(QString("%1%2").arg(QString::number(info->maxIDiameter, 'f', 3), 9, ' ').arg("μm"));
    ui->l9->setText(QString("%1%2").arg(QString::number(info->maxFeret, 'f', 3), 9, ' ').arg("μm"));
    ui->l10->setText(QString("%1%2").arg(QString::number(info->minFeret, 'f', 3), 9, ' ').arg("μm"));
    ui->l11->setText(QString("%1").arg(QString::number(info->rectangularity, 'f', 3), 9, ' '));
    ui->l12->setText(QString("%1").arg(QString::number(info->aspectRatio, 'f', 3), 9, ' '));
    ui->l13->setText(QString("%1").arg(QString::number(info->compactness, 'f', 3), 9, ' '));
    ui->l14->setText(QString("%1").arg(QString::number(info->pIrregularity, 'f', 3), 9, ' '));
    ui->l15->setText(QString("%1").arg(QString::number(info->sphericity, 'f', 3), 9, ' '));
    ui->l16->setText(QString("%1").arg(QString::number(info->granularity, 'f', 3), 9, ' '));
    ui->l17->setText(QString("%1").arg(QString::number(info->roundness, 'f', 3), 9, ' '));
}

SandInfoDialog::~SandInfoDialog() {
    delete ui;
}

void SandInfoDialog::paintImg() {
    QPainter painter(ui->imgWidget);
    // 有裁切则缩放
    if (qMax(img->width(), img->height()) > 100) {
        int max = qMax(img->width(), img->height());
        int width = img->width() * 1.0 / max * 200;
        int height = img->height() * 1.0 / max * 200;
        painter.drawImage((200 - width) / 2,
                          (200 - height) / 2,
                          img->scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else
        painter.drawImage((200 - img->width() * 2) / 2,
                          (200 - img->height() * 2) / 2,
                          img->scaledToHeight(2 * img->height(), Qt::SmoothTransformation));
    painter.end();
}
// 事件过滤器中实现对子部件的绘制
bool SandInfoDialog::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->imgWidget && event->type() == QEvent::Paint)
        paintImg();
    return QWidget::eventFilter(watched, event); // 向上传递事件
}

void SandInfoDialog::showEvent(QShowEvent *) {
    move(parentWidget()->pos().x() + (parentWidget()->width() - width()) / 2,
         parentWidget()->pos().y() + (parentWidget()->height() - height()) / 2);
}

void SandInfoDialog::on_pushButton_clicked() { accept(); }
