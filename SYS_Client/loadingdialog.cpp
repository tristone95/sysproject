#include "loadingdialog.h"
#include "ui_loadingdialog.h"
#include <QGraphicsDropShadowEffect>
#include <QMovie>
#include <QPainter>
LoadingDialog::LoadingDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoadingDialog) {
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_DeleteOnClose);
    initUi();
}

LoadingDialog::~LoadingDialog() {
    delete ui;
    delete m_loadingMovie;
}

void LoadingDialog::initUi() {
    setFixedSize(250, 250);
    m_loadingMovie = new QMovie(":/imgs/loading.gif");
    m_loadingMovie->setScaledSize(QSize(120, 120));
    ui->loadLabel->setScaledContents(true);
    ui->loadLabel->setMovie(m_loadingMovie);
    m_loadingMovie->start();

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor(32, 101, 165));
    shadow->setBlurRadius(10);
    ui->shadowFrame->setGraphicsEffect(shadow);
}

void LoadingDialog::setTipsText(QString tipText) {
    ui->tip->setText(tipText);
}

void LoadingDialog::moveToCenter(QWidget *p) {
    if (p != nullptr) {
        move(p->x() + (p->width() - 250) / 2,
             p->y() + (p->height() - 250) / 2);
    }
}
/*
void LoadingDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(QColor(252, 254, 252)));
    painter.setPen(Qt::transparent);
    QRect rect = this->rect().adjusted(9, 9, -9, -9);
    painter.drawRoundedRect(rect, 10, 10);
    painter.end();
}*/
