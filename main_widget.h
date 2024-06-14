#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QProgressDialog>
class ImageListWidget;
class PdfSaveThread;
class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);
    void insertData(const QString fileName);
public slots:
    void slotSelectFilePathChange(QString filePath);
    void slotSelectSaveFilePath();
    void slotDoSavePDF();
    void slotDoFinished();
    void slotupdateProcess(int value);
private:
    // 主架构布局
    QVBoxLayout *layoutMain;
    // 内容布局，包含图片列表，以及图片显示功能。
    QHBoxLayout *layoutContent;
    // 保存内容信息布局
    QHBoxLayout *layoutSave;

    ImageListWidget *listWidget;
    QLabel *labImage;
    QPushButton *btnSaveFile;
    QLineEdit *edtSaveFile;
    QPushButton *btnDoSavePDF;
    PdfSaveThread *pdfSaveThread;
    QProgressDialog * qProgressDialog;
    void initUI();
    void initConnect();
signals:

};

#endif // MAINWIDGET_H
