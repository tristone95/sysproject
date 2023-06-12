#include "widget.h"
#include "ui_widget.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QTcpSocket>
#include <QValidator>

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
  ui->setupUi(this);
  setWindowFlag(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_DeleteOnClose);
  // ip、端口验证
  QRegularExpression expip(
      "\\b(?:(?:[1-9]?[0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}(?:[1-9]?[0-9]"
      "|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\b");
  QRegularExpression expport(
      "^(?:1(02[4-9]|0[3-9][0-9]|[1-9][0-9]{2})|[2-9][0-9]{3}|[1-5][0-9]{4}|6["
      "0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])$");
  ipval = new QRegularExpressionValidator(expip, this);
  portval = new QRegularExpressionValidator(expport, this);
}

Widget::~Widget() {
  delete ui;
  delete ipval;
  delete portval;
}

void Widget::on_pushButton_clicked() {
  QString strip = ui->lineEdit->text();
  QString strport = ui->lineEdit_2->text();
  int pos = 0;
  ui->label_3->setText(tr("连接服务器..."));
  // 输入合法方可
  if (ipval->validate(strip, pos) == QValidator::Acceptable &&
      portval->validate(strport, pos) == QValidator::Acceptable) {
    ui->pushButton->setEnabled(false);
    emit login(ui->lineEdit->text(), ui->lineEdit_2->text().toInt());
  }
}

void Widget::on_pushButton_2_clicked() { qApp->exit(0); }

void Widget::on_lineEdit_editingFinished() {
  QString str = ui->lineEdit->text();
  int pos = 0;
  if (str.isEmpty())
    return;
  else if (ipval->validate(str, pos) == QValidator::Acceptable) {
    ui->label_4->setText("");
  } else {
    ui->label_4->setText(tr("IP地址错误，请重新输入"));
    ui->label_4->setStyleSheet("color:red");
  }
}

void Widget::on_lineEdit_2_editingFinished() {
  QString str = ui->lineEdit_2->text();
  int pos = 0;
  if (str.isEmpty())
    return;
  else if (portval->validate(str, pos) == QValidator::Acceptable) {
    ui->label_5->setText("");
  } else {
    ui->label_5->setText(tr("端口错误，请重新输入"));
    ui->label_5->setStyleSheet("color:red");
  }
}

void Widget::relogin() {
  ui->label_3->setText(tr("连接超时，请重试"));
  ui->pushButton->setEnabled(true);
}
