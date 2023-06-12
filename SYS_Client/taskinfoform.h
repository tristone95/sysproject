#ifndef TASKINFOFORM_H
#define TASKINFOFORM_H

#include "global.h"
#include <QWidget>

namespace Ui {
class TaskInfoForm;
}

class TaskInfoForm : public QWidget {
  Q_OBJECT

public:
  explicit TaskInfoForm(QWidget *parent = nullptr);
  ~TaskInfoForm();

signals:
  bool taskstart(const TaskInfo &info);

private slots:

  void on_lineEdit_m1_editingFinished();
  void on_lineEdit_m2_editingFinished();
  void on_lineEdit_v1_editingFinished();
  void on_lineEdit_m3_editingFinished();
  void on_lineEdit_m4_editingFinished();
  void on_lineEdit_bm_editingFinished();
  void on_lineEdit_bv_editingFinished();

  void pushButton_clicked();

  void on_radioButton_2_toggled(bool checked);

private:
  Ui::TaskInfoForm *ui;
  double bm = 0, bv = 0, m1 = 0, m2 = 0, m3 = 0, v = 0, m4 = 0;
  QString apparentresult, bulkresult;
  QString taskmode;
  QString taskid;
  void calcapp();
  void calcbulk();
};

#endif // TASKINFOFORM_H
