#ifndef PDFVIEWERDIALOG_H
#define PDFVIEWERDIALOG_H

#include <QDialog>
class QGraphicsScene;
namespace Ui {
class PdfViewerDialog;
}

class PdfViewerDialog : public QDialog {
    Q_OBJECT

public:
    explicit PdfViewerDialog(QByteArray &pdfArray, QString taskId, QWidget *parent);
    ~PdfViewerDialog();
    void convertToImg();
    void setImgs();

private slots:
    void on_save_clicked();

    void on_close_clicked();

private:
    Ui::PdfViewerDialog *ui;
    QByteArray *m_pdf;           // pdf字符数据
    std::vector<QImage> pdfImgs; // pdf转出的Qimage
    QPixmap *pdfPic;             // 拼接完整pdf
    QGraphicsScene *sceen;       // sceen
    QString taskId;
};

#endif // PDFVIEWERDIALOG_H
