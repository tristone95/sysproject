#include "pdfviewerdialog.h"
#include "fpdf_edit.h"
#include "fpdf_scopers.h"
#include "fpdfview.h"
#include "ui_pdfviewerdialog.h"
#include <QDir>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>

PdfViewerDialog::PdfViewerDialog(QByteArray &pdfArray, QString taskId, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PdfViewerDialog) {
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    m_pdf = &pdfArray;
    this->taskId = taskId;
    setGeometry(parent->geometry());
    pdfPic = new QPixmap(1785, 7598); // 三张图片7578+2*margin
    sceen = new QGraphicsScene;
    sceen->setBackgroundBrush(QBrush(0xf0f0f0));
}

PdfViewerDialog::~PdfViewerDialog() {
    delete ui;
    delete pdfPic;
    delete sceen;
}

void PdfViewerDialog::convertToImg() {
    FPDF_LIBRARY_CONFIG config;
    config.version = 2;
    config.m_pUserFontPaths = NULL;
    config.m_pIsolate = NULL;
    config.m_v8EmbedderSlot = 0;
    FPDF_InitLibraryWithConfig(&config);

    FPDF_DOCUMENT doc = FPDF_LoadMemDocument(m_pdf->data(), m_pdf->size(), NULL);
    if (!doc)
        return;

    int page_count = FPDF_GetPageCount(doc);
    for (int var = 0; var < page_count; ++var) {
        FPDF_PAGE page = FPDF_LoadPage(doc, var);
        double scale = 3.0;
        int width = static_cast<int>(FPDF_GetPageWidth(page) * scale);
        int height = static_cast<int>(FPDF_GetPageHeight(page) * scale);
        int alpha = FPDFPage_HasTransparency(page) ? 1 : 0;
        ScopedFPDFBitmap bitmap(FPDFBitmap_Create(width, height, alpha)); // BGRx
        if (bitmap) {
            FPDF_DWORD fill_color = alpha ? 0x00000000 : 0xFFFFFFFF;
            FPDFBitmap_FillRect(bitmap.get(), 0, 0, width, height, fill_color);

            int rotation = 0;
            int flags = FPDF_ANNOT;
            FPDF_RenderPageBitmap(bitmap.get(), page, 0, 0, width, height,
                                  rotation, flags);
            void *buffer = FPDFBitmap_GetBuffer(bitmap.get());
            QImage image((uchar *)buffer, width, height, QImage::Format_RGBA8888);
            // image共享buffer内存会被回收，故copy
            pdfImgs.push_back(image.copy());
        }
    }
}
// 获取pdf的QImage并拼到一张
void PdfViewerDialog::setImgs() {
    QPainter painter(pdfPic);
    int y = 0;
    int margin = 10; // 页面间距
    for (int var = 0; var < pdfImgs.size(); ++var) {
        painter.drawImage(QRect(0, y, pdfImgs[var].width(), pdfImgs[var].height()), pdfImgs[var]);
        y += pdfImgs[var].height();
        painter.setBrush(QBrush(0xF0F0F0));
        painter.setPen(QPen(Qt::NoPen));
        painter.drawRect(QRect(0, y, pdfImgs[var].width(), margin));
        y += 10;
    }
    painter.end();
    QGraphicsPixmapItem *picItem = new QGraphicsPixmapItem(*pdfPic);
    picItem->setTransformationMode(Qt::SmoothTransformation); // 对于像素图形项，需在其上开启抗锯齿才能生效
    sceen->addItem(picItem);
    ui->graphicsView->setScene(sceen);
    // 自适应大小
    ui->graphicsView->fitInView(0, 0, 1785, 2515, Qt::KeepAspectRatio);
}
void PdfViewerDialog::on_save_clicked() {
    // ExportPDF不存在则创建
    QDir pdfdir("./ExportPDF");
    if (!pdfdir.exists())
        pdfdir.mkdir(".");
    QString filename = QString("%1%2%3").arg("./ExportPDF/task_", taskId, ".pdf");
    filename = QFileDialog::getSaveFileName(this, tr("保存报告PDF"), filename, "PDF File(*.pdf *.PDF)");
    if (filename.isEmpty())
        return;
    else {
        QFile file(filename);
        if (!file.open(QFile::WriteOnly)) {
            return;
        }
        file.write(m_pdf->data(), m_pdf->size());
        file.close();
    }
}

void PdfViewerDialog::on_close_clicked() {
    accept();
}
