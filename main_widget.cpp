#include "main_widget.h"
#include "image_list_widget.h"
#include "pdf_save_thread.h"
#include <QtDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QPrinter>
#include <QPainter>
#include <QThread>
#pragma execution_character_set("utf-8")
MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , layoutMain(new QVBoxLayout())
    , layoutContent(new QHBoxLayout())
    , layoutSave(new QHBoxLayout())
    , listWidget(new ImageListWidget(this))
    , labImage(new QLabel(this))
    , btnSaveFile(new QPushButton(tr("选择保存文件地址"), this))
    , edtSaveFile(new QLineEdit(this))
    , btnDoSavePDF(new QPushButton(tr("生成PDF"), this))
    , pdfSaveThread(new PdfSaveThread())

{
    initUI();
    initConnect();
    setAcceptDrops(true);
}

void MainWidget::insertData(const QString fileName)
{
    if (listWidget != nullptr) {
        listWidget->insertData(fileName);
    }
}

void MainWidget::slotSelectFilePathChange(QString filePath)
{
    qInfo() << "slotSelectFilePathChange:" << filePath;
    filePath.replace("file:///", "");
    QPixmap pm(filePath);
    QPixmap scaledPixmap =
        pm.scaled(labImage->width(), pm.height(), Qt::KeepAspectRatio);
    labImage->setPixmap(scaledPixmap);
}

void MainWidget::slotSelectSaveFilePath()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save PDF File", "",
                                                    "PDF Files (*.pdf)");
    if (!fileName.isEmpty()) {
        edtSaveFile->setText(fileName);
    }
}

void MainWidget::slotDoSavePDF()
{
    QVector<QString> filePaths = listWidget->getFileLists();
    if (filePaths.isEmpty()) {
        return;
    }
    qProgressDialog =
        new QProgressDialog("Processing...", "Cancel", 0, 100, this);
    qProgressDialog->show();
    btnDoSavePDF->setDisabled(true);
    pdfSaveThread->setFilePaths(filePaths, edtSaveFile->text());
    pdfSaveThread->start();
}

void MainWidget::slotDoFinished()
{
    QMessageBox::information(this, "提示", "文件生成成功");
    if (qProgressDialog) {
        qProgressDialog->setValue(100);
        delete qProgressDialog;
        qProgressDialog = nullptr;
    }

    btnDoSavePDF->setDisabled(false);
}

void MainWidget::slotupdateProcess(int value)
{
    if (qProgressDialog) {
        qProgressDialog->setValue(value);
    }
}

void MainWidget::initUI()
{
    listWidget->setMaximumWidth(600);
    layoutContent->addWidget(listWidget);
    labImage->setMinimumWidth(400);

    labImage->setMinimumHeight(this->height());
    layoutContent->addWidget(labImage);
    layoutSave->addWidget(edtSaveFile);
    layoutSave->addWidget(btnSaveFile);

    layoutMain->addLayout(layoutContent);
    layoutMain->addLayout(layoutSave);
    layoutMain->addWidget(btnDoSavePDF);
    setLayout(layoutMain);
}

void MainWidget::initConnect()
{
    connect(listWidget, &ImageListWidget::sigSelectFilePathChange, this,
            &MainWidget::slotSelectFilePathChange);
    connect(btnSaveFile, &QPushButton::clicked, this,
            &MainWidget::slotSelectSaveFilePath);
    connect(btnDoSavePDF, &QPushButton::clicked, this,
            &MainWidget::slotDoSavePDF);
    connect(pdfSaveThread, &QThread::finished, this,
            &MainWidget::slotDoFinished);
    connect(pdfSaveThread, &PdfSaveThread::sigUpdateProcess, this,
            &MainWidget::slotupdateProcess);
}
