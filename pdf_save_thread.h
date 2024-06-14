#ifndef PDFSAVETHREAD_H
#define PDFSAVETHREAD_H

#include <QThread>
#include <QObject>
#include <QThread>
#include <QVector>
class PdfSaveThread : public QThread
{
Q_OBJECT
public:
    PdfSaveThread();
    void setFilePaths(QVector<QString> filePaths,QString savePDFFileName);
private:
    QVector<QString> m_filePaths;
    QString m_savePDFFileName;

    // QThread interface
protected:
    void run();
signals:
    void sigUpdateProcess(int value);
};

#endif // PDFSAVETHREAD_H
