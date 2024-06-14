#include "pdf_save_thread.h"
#include <QPrinter>
#include <QImage>
#include <QPainter>
PdfSaveThread::PdfSaveThread()
{

}

void PdfSaveThread::setFilePaths(QVector<QString> filePaths,QString savePDFFileName)
{
    m_filePaths = filePaths;
    m_savePDFFileName = savePDFFileName;
}

void PdfSaveThread::run()
{
    QPrinter printer(QPrinter::PrinterMode::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(m_savePDFFileName);

    QPainter painter(&printer);

    for (int i = 0; i < m_filePaths.size(); ++i) {
        QString filePath = m_filePaths.at(i);
        filePath.replace("file:///","");
        QImage image(filePath);
        if (image.isNull()) {
            continue;  // Skip invalid images
        }

        // Scale the image to fit the page while maintaining aspect ratio
        QSize imageSize = image.size();
        imageSize.scale(printer.pageRect().size(), Qt::KeepAspectRatio);

        QPoint center = printer.pageRect().center() - QPoint(imageSize.width() / 2, imageSize.height() / 2);
        painter.drawImage(center, image.scaled(imageSize, Qt::KeepAspectRatio));
        emit sigUpdateProcess((i*100/m_filePaths.size()));
        if (i < m_filePaths.size() - 1) {
            printer.newPage();  // Start a new page for the next image
        }
    }

    painter.end();
}
