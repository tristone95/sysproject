#ifndef SANDINFODIALOG_H
#define SANDINFODIALOG_H

#include <QDialog>
struct SandInfo;

namespace Ui {
class SandInfoDialog;
}

class SandInfoDialog : public QDialog {
    Q_OBJECT

public:
    explicit SandInfoDialog(SandInfo *info, QWidget *parent = nullptr);
    ~SandInfoDialog();
    void paintImg();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void showEvent(QShowEvent *) override;
private slots:
    void on_pushButton_clicked();

private:
    Ui::SandInfoDialog *ui;
    QImage *img;
};

#endif // SANDINFODIALOG_H
