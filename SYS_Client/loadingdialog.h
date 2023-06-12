#ifndef LOADINGDIALOG_H
#define LOADINGDIALOG_H

#include <QDialog>

namespace Ui
{
class LoadingDialog;
}

class LoadingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoadingDialog(QWidget *parent = nullptr);
    ~LoadingDialog();
    void initUi();
    void setTipsText(QString tipText);
    void moveToCenter(QWidget *p);

protected:
    // void paintEvent(QPaintEvent *event) override;

private:
    Ui::LoadingDialog *ui;
    QMovie *m_loadingMovie;
};

#endif // LOADINGDIALOG_H
