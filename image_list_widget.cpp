#pragma execution_character_set("utf-8")
#include "image_list_widget.h"

#include <QDebug>
#include <QFileInfo>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QDropEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QPainter>
#include <QHeaderView>
#include <QItemSelection>
#include <QMenu>
#include <QAction>
ImageListWidget::ImageListWidget(QWidget *_parent_widget)
    : QTableView(_parent_widget)
    , db_table_model_(new QStandardItemModel)
    , draggingLine(new QLabel(this))
{
    initUI();
    initConnect();
}

void ImageListWidget::insertData(QString filePath)
{
    qDebug() << "insertData:" << filePath;
    if (isFileExited(filePath)) {
        QMessageBox::information(this, "提示", "文件已经存在");
        return;
    }
    if (isPicture(filePath)) {
        QString fileName = getFileName(filePath);

        int row = db_table_model_->rowCount();
        QStandardItem *itemNumber = new QStandardItem(QString::number(row + 1));
        itemNumber->setTextAlignment(Qt::AlignCenter);
        QStandardItem *itemFileName = new QStandardItem(fileName);
        QStandardItem *itemFilePath = new QStandardItem(filePath);
        db_table_model_->setItem(row, 0, itemNumber);
        db_table_model_->setItem(row, 1, itemFileName);
        db_table_model_->setItem(row, 2, itemFilePath);
    }
}

QVector<QString> ImageListWidget::getFileLists() {

    QVector<QString> res;
    for (int i = 0; i < db_table_model_->rowCount(); i++) {
        res.push_back( db_table_model_->item(i, 2)->text() ) ;
    }
    return res;
}

void ImageListWidget::initUI()
{
    // 设置设置上下文菜单策略为自定义
    setContextMenuPolicy(Qt::CustomContextMenu);


    mRowHeight = 30;
    mValidPress = false;
    mRowFrom = 0;
    mRowTo = 0;
    this->horizontalHeader()
        ->hide();  //隐藏列头(带列头的情况需要修改代码，重新计算高度偏移)
    this->verticalHeader()->hide();                     //隐藏行头
    this->verticalHeader()->setDefaultSectionSize(30);  //默认列高度
    this->horizontalHeader()->setStretchLastSection(true);  //最后一列自适应宽度
    //最后一列自适应宽度
    setEditTriggers(QTableView::NoEditTriggers);    //不能编辑
    setSelectionBehavior(QTableView::SelectRows);   //一次选中整行
    setSelectionMode(QTableView::SingleSelection);  //单行选中
    setAlternatingRowColors(true);                  //行间隔色
    setShowGrid(false);                             //去掉网格线
    setFocusPolicy(Qt::NoFocus);  //去掉item选中时虚线框
    setStyleSheet(
        "QTableView {border: 1px solid gray;background: #E8E8E8;}\
                          QTableView::item{color:black;}\
                          QTableView::item:selected{color:black;background: #63B8FF;}");

    setColumnWidth(0, 40);
    selectRow(0);
    draggingLine->setFixedHeight(2);
    draggingLine->setGeometry(1, 0, width(), 2);
    draggingLine->setStyleSheet("border: 1px solid #8B7500;");
    draggingLine->hide();

    setAcceptDrops(true);  // 接受放置
    QStringList table_h_headers;
    table_h_headers << "文件名称"
                    << "文件路径";
    db_table_model_->setHorizontalHeaderLabels(table_h_headers);

    setModel(db_table_model_);
}

void ImageListWidget::initConnect() {

      // 连接上下文菜单请求信号
    connect(this, &QTableView::customContextMenuRequested, this, &ImageListWidget::slotShowContextMenu);
}

bool ImageListWidget::isPicture(QString filePath)
{
    if (filePath.isEmpty()) {
        return false;
    }

    QFileInfo fileInfo(filePath);
    if (fileInfo.suffix().compare("png", Qt::CaseInsensitive) == 0 ||
        fileInfo.suffix().compare("jpg", Qt::CaseInsensitive) == 0 ||
        fileInfo.suffix().compare("jpeg", Qt::CaseInsensitive) == 0) {
        return true;
    }
    return false;
}

QString ImageListWidget::getFileName(QString filePath)
{
    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.fileName();
    return fileName;
}

bool ImageListWidget::isFileExited(QString filePath)
{
    for (int i = 0; i < db_table_model_->rowCount(); i++) {
        if (filePath == db_table_model_->item(i, 2)->text()) {
            return true;
        }
    }
    return false;
}

void ImageListWidget::DoDrag()
{
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    mimeData->setText(mDragText);
    drag->setMimeData(mimeData);

    // 设置拖拽图片
    QPixmap drag_img(width(), mRowHeight);
    drag_img.fill(QColor(255, 255, 255, 100));
    QPainter painter(&drag_img);
    painter.setPen(QColor(0, 0, 0, 200));
    painter.drawText(QRectF(40, 0, width(), mRowHeight), mDragText,
                     QTextOption(Qt::AlignVCenter));

    drag->setPixmap(drag_img);
    drag->setHotSpot(mDragPointAtItem);

    //***注意：此句会阻塞，进入drag的拖拽消息循环，会依次触发dragEnterEvent、dragMoveEvent、dropEvent消息
    if (drag->exec(Qt::MoveAction) == Qt::MoveAction) {
        // 确认移动操作
    }
}

void ImageListWidget::ResetOrder()
{
    for (int i = 0; i < db_table_model_->rowCount(); i++) {
        db_table_model_->item(i, 0)->setText(QString::number(i + 1));
    }
}

void ImageListWidget::MoveRow(int from, int to)
{
    if (from == to) {
        return;
    }
    QStandardItem *item = db_table_model_->item(from, 1);
    QStandardItem *itemFilePath = db_table_model_->item(from, 2);
    if (item && itemFilePath) {
        QString strText = item->text();
        QString strFilePath = itemFilePath->text();

        QList<QStandardItem *> items;
        QStandardItem *item0 = new QStandardItem("");
        QStandardItem *item1 = new QStandardItem(strText);
        QStandardItem *item2 = new QStandardItem(strFilePath);
        items.append(item0);
        items.append(item1);
        items.append(item2);
        item0->setTextAlignment(Qt::AlignCenter);

        //移动行思路：就是先在要移动到的位置新建同样内容的行，然后删除被移动的行
        db_table_model_->insertRow(to, items);
        if (from < to) {
            db_table_model_->removeRow(from);
            selectRow(to - 1);
        } else {
            db_table_model_->removeRow(from + 1);
            selectRow(to);
        }
        ResetOrder();
        emit sigRowChange(mRowFrom, mRowTo);
    }
}

void ImageListWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        QModelIndex index = indexAt(e->pos());
        if (index.isValid()) {
            mValidPress = true;
            mDragPoint = e->pos();
            mDragText = db_table_model_->item(index.row(), 1)->text();
            mDragPointAtItem = mDragPoint - QPoint(0, index.row() * mRowHeight);
            mRowFrom = index.row();
        }
    }
    QTableView::mousePressEvent(e);
}

void ImageListWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (!mValidPress) {
        return;
    }
    if (!(e->buttons() & Qt::LeftButton)) return;

    if ((e->pos() - mDragPoint).manhattanLength() <
        QApplication::startDragDistance())
        return;

    draggingLine->show();

    DoDrag();  //开始拖拽，完成拖拽后才会继续往下走

    draggingLine->hide();
    mValidPress = false;
}

void ImageListWidget::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasText()) {
        if (e->mimeData()->hasUrls()) {
            return QTableView::dragEnterEvent(e);
        }
        e->acceptProposedAction();
    } else {
        e->ignore();
        QTableView::dragEnterEvent(e);
    }
}

void ImageListWidget::dragMoveEvent(QDragMoveEvent *e)
{
    if (e->mimeData()->hasText()) {
        int nCurRow = 0;
        QModelIndex index = indexAt(e->pos());
        if (index.isValid()) {
            if (e->pos().y() - index.row() * mRowHeight >= mRowHeight / 2) {
                nCurRow = index.row() + 1;
            } else {
                nCurRow = index.row();
            }
        } else {
            nCurRow = db_table_model_->rowCount();
        }

        mRowTo = nCurRow;
        draggingLine->setGeometry(1, mRowTo * mRowHeight, width() - 2, 2);

        e->acceptProposedAction();
        return;
    }

    e->ignore();
    QTableView::dragMoveEvent(e);
}

void ImageListWidget::dropEvent(QDropEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        return QTableView::dropEvent(e);
    }
    if (e->mimeData()->hasText()) {
        if (mRowTo != mRowFrom) {
            MoveRow(mRowFrom, mRowTo);
        }
        e->acceptProposedAction();
        return;
    }

    QTableView::dropEvent(e);
}

void ImageListWidget::selectionChanged(const QItemSelection &selected,
                                       const QItemSelection &deselected)
{
    QModelIndexList selectedIndexes = selected.indexes();
    if (selectedIndexes.isEmpty()) {
        return;
    }
    QModelIndex index = selectedIndexes.at(2);
    QString filePath = db_table_model_->data(index).toString();
    emit sigSelectFilePathChange(filePath);
    QTableView::selectionChanged(selected, deselected);
}

void ImageListWidget::slotShowContextMenu(const QPoint &pos)
{
    // 创建右键菜单
    QMenu contextMenu(tr("Context Menu"), this);

    // 添加菜单项
    QAction action1("删除", this);
    connect(&action1, &QAction::triggered, this, &ImageListWidget::slotDeleteImageItem);
    contextMenu.addAction(&action1);


    // 在鼠标右键点击的位置显示菜单
    contextMenu.exec(mapToGlobal(pos));
}

void ImageListWidget::slotDeleteImageItem()
{

    if(!currentIndex().isValid()){
        return;
    }
    db_table_model_->removeRow( currentIndex().row());
    ResetOrder();
}
