#include "taskinfoform.h"
#include "qdatetime.h"
#include "ui_taskinfoform.h"

TaskInfoForm::TaskInfoForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::TaskInfoForm) {
  ui->setupUi(this);
  connect(ui->pushButton, &QPushButton::clicked, this,
          &TaskInfoForm::pushButton_clicked);
  ui->lineEdit->setEnabled(false);
}

TaskInfoForm::~TaskInfoForm() { delete ui; }

// 输入完成计算体、视密度
void TaskInfoForm::calcapp() {
  if (m1 != 0 && m2 != 0 && m3 != 0 && m4 != 0 && v != 0) {
    double apparent = m3 / (v - (m4 - m3 - m1) * v / (m2 - m1));
    apparentresult = QString::number(apparent, 'f', 3);
    ui->apparentres->setText(apparentresult);
  } else {
    ui->apparentres->setText("----");
    apparentresult.clear();
  }
}
void TaskInfoForm::calcbulk() {
  if (bm != 0 && bv != 0) {
    double bulk = bm / bv;
    bulkresult = QString::number(bulk, 'f', 3);
    ui->bulkres->setText(bulkresult);
  } else {
    ui->bulkres->setText("----");
    bulkresult.clear();
  }
}
void TaskInfoForm::on_lineEdit_m1_editingFinished() {
  m1 = ui->lineEdit_m1->text().toDouble();
  calcapp();
}
void TaskInfoForm::on_lineEdit_m2_editingFinished() {
  m2 = ui->lineEdit_m2->text().toDouble();
  calcapp();
}

void TaskInfoForm::on_lineEdit_v1_editingFinished() {
  v = ui->lineEdit_v1->text().toDouble();
  calcapp();
}

void TaskInfoForm::on_lineEdit_m3_editingFinished() {
  m3 = ui->lineEdit_m3->text().toDouble();
  calcapp();
}

void TaskInfoForm::on_lineEdit_m4_editingFinished() {
  m4 = ui->lineEdit_m4->text().toDouble();
  calcapp();
}

void TaskInfoForm::on_lineEdit_bm_editingFinished() {
  bm = ui->lineEdit_bm->text().toDouble();
  calcbulk();
}

void TaskInfoForm::on_lineEdit_bv_editingFinished() {
  bv = ui->lineEdit_bv->text().toDouble();
  calcbulk();
}

void TaskInfoForm::pushButton_clicked() {
  // QString 直接memcpy to char* to QString 中文会出现乱码
  /***因为Qstring.data为QChar,QChar编码为unicode16***/
  // 两种方法
  // 1 toLatin1()得到byteArray .data
  // 2 tostd::string获取c_str(与data()区别在于一定有\0,而data()看具体库实现);
  std::string sandtype = ui->type_combox->currentText().toStdString();
  std::string person = ui->person_edit->text().toStdString();
  std::string source = ui->from_edit->text().toStdString();
  std::string unit = ui->unit_edit->text().toStdString();
  if (person.size() && source.size() && unit.size()) {
    TaskInfo info;
    int count = ui->lineEdit->text().toInt();
    if (ui->radioButton->isChecked()) {
      info.manualControl = true;
    } else if (ui->radioButton_2->isChecked() && count) {
      info.manualControl = false;
      info.taskImgNum = count;
    } else
      return;
    info.bulkDensity = bulkresult.toDouble();
    info.apparentDensity = apparentresult.toDouble();
    QString id = (QDate::currentDate().toString("yyyyMMdd") +
                  QTime::currentTime().toString("hhmm"));
    std::string stdid = id.toStdString();
    // data返回带\0的地址，故size+1带上\0
    // qstring 为Unicode16编码
    std::memcpy(info.taskId, stdid.c_str(), stdid.size() + 1);
    std::memcpy(info.sampleType, sandtype.c_str(), sandtype.size() + 1);
    std::memcpy(info.sampleSource, source.c_str(), source.size() + 1);
    std::memcpy(info.testingUnit, unit.c_str(), unit.size() + 1);
    std::memcpy(info.inspector, person.c_str(), person.size() + 1);
    emit taskstart(info);
  }
}

void TaskInfoForm::on_radioButton_2_toggled(bool checked) {
  if (checked)
    ui->lineEdit->setEnabled(true);
  else
    ui->lineEdit->setEnabled(false);
}
