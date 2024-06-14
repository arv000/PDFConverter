#include "main_window.h"
#include "main_widget.h"
#include <QMenuBar>
#include <QtDebug>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QFileDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mainWidget(new MainWidget(this))
    , menuBar(new QMenuBar(this))
{
    setAcceptDrops(true);
    initUI();
    initConnect();
}

MainWindow::~MainWindow() {}

void MainWindow::initUI()
{
    setMinimumWidth(800);
    setMinimumHeight(700);
    setMenuBar(menuBar);
    setCentralWidget(mainWidget);
}

void MainWindow::initConnect()
{
    QMenu *fileMenu = menuBar->addMenu(tr("File"));
    QAction *openAction = new QAction(tr("Open"), this);
    fileMenu->addAction(openAction);
    connect(openAction, SIGNAL(triggered()), this, SLOT(slotOpenFile()));
}

void MainWindow::slotOpenFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image File", "", "Images (*.png *.jpg *.jpeg)");
    mainWidget->insertData(fileName);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        for (const QUrl &url : urlList) {
            mainWidget->insertData(url.toString());
        }
    }
}
