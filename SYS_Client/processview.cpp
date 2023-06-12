#include "processview.h"
#include "ui_processview.h"
#include <QPainter>
#include <QTimer>

ProcessView::ProcessView(bool manual, int num, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProcessView)
    , manualFlag(manual) {
    ui->setupUi(this);
    ui->label->setVisible(false);
    initialWin(manual, num);
    QPixmap welCome(560, 420);
    QPainter painter;
    painter.begin(&welCome);
    painter.setBrush(QBrush(Qt::white));
    painter.drawRect(-1, -1, 562, 422);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setFont(QFont("Microsoft YaHei UI", 32));
    painter.drawText(80, 185, QString("Waiting For Picture."));
    painter.end();
    ui->label->setPixmap(welCome);
    /*
        refreshTimer = new QTimer(this);
        connect(refreshTimer, &QTimer::timeout, this, &ProcessView::showImg);*/
}
ProcessView::~ProcessView() { delete ui; }

void ProcessView::initialWin(bool manual, int num) {
    manualFlag = manual;
    // 自动模式不可见
    ui->pushButton->setVisible(manualFlag);
    // 手动模式不可见
    ui->progressBar->setVisible(!manualFlag);
    if (!manualFlag) {
        ui->progressBar->setMaximum(num);
    }
}

void ProcessView::upData(int progressNum, int totalNum,
                         const std::vector<QImage> &imgs) {
    // 测试环境太卡，关闭实时照片显示
    // showImgBuf.push_back(imgs);
    ui->progressBar->setValue(progressNum);
    ui->label_4->setText(QString::number(totalNum));
    ui->label_5->setText(QString::number(progressNum));
}

void ProcessView::upData(int progressNum, int totalNum) {
    ui->progressBar->setValue(progressNum);
    ui->label_4->setText(QString::number(totalNum));
    ui->label_5->setText(QString::number(progressNum));
}
/*
void ProcessView::startImg() {} // refreshTimer->start(500); }

void ProcessView::stopImg(int num, bool manual) {
   // refreshTimer->stop();

      QPixmap endImg(560, 420);
      QPainter painter;
      painter.begin(&endImg);
      painter.setBrush(QBrush(Qt::white));
      painter.drawRect(-1, -1, 562, 422);
      painter.setRenderHint(QPainter::Antialiasing);
      painter.setFont(QFont("Microsoft YaHei UI", 32));
      painter.setRenderHint(QPainter::Antialiasing);
      painter.drawText(60, 185, QString("Capture Finished."));
      painter.end();
      ui->label->setPixmap(endImg);
      ui->label->update();
    if (manual) {
        // 手动停止
        ui->pushButton->setEnabled(false);
        ui->progressBar->setMaximum(num);
        ui->progressBar->setVisible(true);
    }
}
*/
void ProcessView::showImg() {
    if (!showImgBuf.empty()) {
        QPixmap tmp = QPixmap::fromImage(showImgBuf.front()[index]);
        ui->label->setPixmap(tmp.scaledToHeight(420));
        ui->label->update();
        ++index;
        if (index == showImgBuf.front().size() - 1) {
            // showImgBuf.pop_front();
            index = 0;
        }
    }
}

void ProcessView::on_pushButton_clicked() { // 向mainwindow发送停止信号
    emit stop();
}
