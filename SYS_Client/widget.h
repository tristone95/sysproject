#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class QValidator;

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
} // namespace Ui
QT_END_NAMESPACE

class Widget : public QWidget {
  Q_OBJECT

public:
  Widget(QWidget *parent = nullptr);
  ~Widget();
  void relogin();
signals:
  void login(QString ip, int port);

private slots:
  void on_pushButton_clicked();
  void on_pushButton_2_clicked();
  void on_lineEdit_editingFinished();
  void on_lineEdit_2_editingFinished();

private:
  Ui::Widget *ui;
  QValidator *ipval;
  QValidator *portval;
};
#endif // WIDGET_H
