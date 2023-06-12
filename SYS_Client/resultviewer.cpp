#include "resultviewer.h"
#include "sandcard.h"
#include "ui_resultviewer.h"
#include <QGridLayout>

ResultViewer::ResultViewer(QWidget *topWind)
    : ui(new Ui::ResultViewer) {
    ui->setupUi(this);
    topWindow = topWind;
}

ResultViewer::~ResultViewer() {
    delete ui;
    delete sandsData;
    delete cardLayout;
    for (auto var : showCard) {
        delete var;
    }
}

void ResultViewer::initWindow(TaskInfo &info) {
    QString id = QString::fromStdString(info.taskId);
    ui->ID->setText(id);
    ui->Type->setText(QString::fromStdString(info.sampleType));
    ui->Source->setText(QString::fromStdString(info.sampleSource));
    ui->Person->setText(QString::fromStdString(info.inspector));
    ui->Time->setText(QString("%1年%2月%3日 %4时%5分")
                          .arg(id.first(4), id.mid(4, 2), id.mid(6, 2), id.mid(8, 2), id.mid(10, 2)));
}

void ResultViewer::updateResult(ResultInfo &rInfo) {
    ui->result1->setText(QString::number(rInfo.granularityPass, 'f', 2));
    ui->result2->setText(QString::number(rInfo.roundness, 'f', 2));
    ui->result3->setText(QString::number(rInfo.sphericity, 'f', 2));
    ui->result4->setText(QString::number(rInfo.averageGranularity, 'f', 2));
    ui->result5->setText(QString("%1%2").arg(QString::number(rInfo.bulkDensity, 'f', 2), "g/cm³"));
    ui->result6->setText(QString("%1%2").arg(QString::number(rInfo.apparentDensity, 'f', 2), "g/cm³"));
    ui->result7->setText(QString::number(rInfo.particleNumTotal));
}

void ResultViewer::setDatas(std::vector<SandInfo> &vecInfo) {
    if (dataFlag)
        delete sandsData;
    else
        dataFlag = true;
    sandsData = new std::vector<SandInfo>(std::move(vecInfo));
    showSand();
}

void ResultViewer::showSand() {
    if (layOutFlag) {
        for (auto var : showCard) {
            cardLayout->removeWidget(var);
            delete var;
        }
        delete cardLayout;
        showCard.clear();
    } else
        layOutFlag = true;
    cols = (width() - 20) / 160;
    cardLayout = new QGridLayout(ui->content);
    for (int var = 0; var < sandsData->size(); ++var) {
        SandCard *sand = new SandCard(&sandsData->at(var), topWindow);
        cardLayout->addWidget(sand, var / cols, var % cols);
        showCard.push_back(sand);
    }
}

void ResultViewer::setComboItems(int count) {
    ui->comboBox->clear();
    for (int var = 1; var <= count; ++var) {
        ui->comboBox->addItem(QString("%1%2%3").arg("第", QString::number(var), "秒"));
    }
    ui->comboBox->setCurrentIndex(0);
}
// 使滚动条回滚至顶部
void ResultViewer::scrollTop() {
    ui->scrollArea_2->ensureVisible(0, 0);
}

QComboBox *ResultViewer::getCombox() {
    return ui->comboBox;
}

QPushButton *ResultViewer::getButton() {
    return ui->pushButton;
}

void ResultViewer::resizeEvent(QResizeEvent *event) {
    if ((width() - 20) / 160 == cols) {
        return;
    } else {
        cols = (width() - 20) / 160;
        QGridLayout *tmp = new QGridLayout;
        int index = 0;
        for (auto widget : showCard) {
            cardLayout->removeWidget(widget);
            tmp->addWidget(widget, index / cols, index % cols);
            ++index;
        }
        delete ui->content->layout();
        ui->content->setLayout(tmp);
        cardLayout = tmp;
    }
}

void ResultViewer::on_pushButton_2_clicked() {
    emit showPdf();
}
