# 一、需求说明
最近在换工作，都需要将所有的图片，例如身份证、毕业证等信息都需要合并到一个pdf中。找了软件发现几乎都需要收费，干脆就自己写一个好了。
+ 功能说明如下：
	1. 支持鼠标拖动加载图片，可以从桌面将图片类型的文件拖到软件中，一次最多添加150张。
	2. 支持点击菜单中的添加按钮添加图片。
	3. 支持菜单中选择目录将目录中所有图片添加到软件中，最多添加150张。
	4. 所有加载的图片地址需要再列表框中展示。
	5. 鼠标移动到列表框的地址栏中时，需要展示完整的图片地址。
	6. 支持通过鼠标拖动切换图片的顺序。
	7. 支持右键删除选中的图片。
	8. 点击清空能够清空所有选择的图片
	9. 选中的图片在左边进行图片展示区进行展示。
	10. 生成pdf图片时需要有进度条提示。
	11. 生成PDF文件有完成提示，清空列表。

如果各位其他需求，可以在留言中提出，如果我能做到，我会尽量满足。


# 二、主要功能研发说明
## 2.1 QTableView右键餐单
1. 设置设置上下文菜单策略为自定义
```cpp
setContextMenuPolicy(Qt::CustomContextMenu);
```
2. 连接上下文菜单请求信号
```cpp
connect(this, &QTableView::customContextMenuRequested, this, &ImageListWidget::slotShowContextMenu);
 ```
3. 创建菜单并且绑定菜单信号
```cpp
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

```
4. 删除信号处理
先获取当前被选择的行，然后在model中删除对应的行。
通过currentIndex()获取当前选中的行。
```cpp
void ImageListWidget::slotDeleteImageItem()
{
   if(!currentIndex().isValid()){
        return;
    }
    db_table_model_->removeRow( currentIndex().row());
    ResetOrder();
}
```
## 2.2 Qt显示进度条
进度条显示需要使用到Qt的原生的弹出框显示
+ 初始化进度条
```cpp
    qProgressDialog = new QProgressDialog("Processing...", "Cancel", 0, 100, this);
    qProgressDialog->show();
```
绑定消息， 当任务处理到一定进度时通知进度条更新进度
```cpp
	// 当任务完成时通知，进度条完成，
    connect(pdfSaveThread,&QThread::finished,this,&MainWidget::slotDoFinished);
	connect(pdfSaveThread,&PdfSaveThread::sigUpdateProcess,this,&MainWidget::slotupdateProcess);
```
+ 设置进度
```cpp
void MainWidget::slotupdateProcess(int value)
{
    if(qProgressDialog){
        qProgressDialog->setValue(value);
    }
}
```
+ 进度完成，进度到达100后设计他的值为100.并且删除进度弹框，然后回复按钮状态。
```cpp
void MainWidget::slotDoFinished()
{
    QMessageBox::information(this,"提示","文件生成成功");
    if(qProgressDialog){
        qProgressDialog->setValue(100);
        delete qProgressDialog;
        qProgressDialog = nullptr;
    }
    btnDoSavePDF->setDisabled(false);
}

```
## 2.3 Qt图片生成pdf
pdf的生成需要使用到printer组件，所以在工程文件中需要配置
```shll
QT       += core gui printsupport
```
生成pdf图片如下
```cpp
// 设置高清打印
  QPrinter printer(QPrinter::PrinterMode::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(m_savePDFFileName);

    QPainter painter(&printer);
	// 编译图片路径
    for (int i = 0; i < m_filePaths.size(); ++i) {
        QString filePath = m_filePaths.at(i);
        filePath.replace("file:///","");
        // 加载本地图片
        QImage image(filePath);
        if (image.isNull()) {
            continue;  // Skip invalid images
        }

        // Scale the image to fit the page while maintaining aspect ratio
        QSize imageSize = image.size();
        imageSize.scale(printer.pageRect().size(), Qt::KeepAspectRatio);
		// 设置居中展示
        QPoint center = printer.pageRect().center() - QPoint(imageSize.width() / 2, imageSize.height() / 2);
        painter.drawImage(center, image.scaled(imageSize, Qt::KeepAspectRatio));
        // 通知进度
        emit sigUpdateProcess((i*100/m_filePaths.size()));
        if (i < m_filePaths.size() - 1) {
            printer.newPage();  // Start a new page for the next image
        }
    }

    painter.end();
```
QPrinter::PrinterMode
ScreenResolution:
作用：设置打印的分辨率与屏幕的分辨率相同，通常较低。适用于快速预览和草稿打印。
PrinterResolution:
作用：设置打印机的默认分辨率。这通常是打印机所能处理的最佳分辨率，适用于正式文档和高质量打印。
## 2.4 Qt拖拽更新列表顺序
```cpp
#include "MyTableView.h"
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QPainter>
#include <QHeaderView>

MyTableView::MyTableView(QWidget *parent)
    : QTableView(parent)
{
    setAcceptDrops(true);

	mLabel = new QLabel(this);
	mLabel->setFixedHeight(2);
    mLabel->setGeometry(1, 0, width(), 2);
	mLabel->setStyleSheet("border: 1px solid #8B7500;");
	mLabel->hide();

    mModel = NULL;
    mRowHeight = 30;
    mValidPress = false;
    mRowFrom = 0;
    mRowTo = 0;
}

void MyTableView::SetModel(QStandardItemModel *model)
{
    mModel = model;
    QTableView::setModel(model);
}

void MyTableView::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        QModelIndex index = indexAt(e->pos());
        if (index.isValid())
        {
            mValidPress = true;
            mDragPoint = e->pos();
            mDragText = mModel->item(index.row(),1)->text();
            mDragPointAtItem = mDragPoint - QPoint(0,index.row()*mRowHeight);
            mRowFrom = index.row();
        }
    }
    QTableView::mousePressEvent(e);
}

void MyTableView::mouseMoveEvent ( QMouseEvent * e )
{
    if (!mValidPress)
	{
		return;
	}
    if (!(e->buttons() & Qt::LeftButton))
        return;

    if ((e->pos() - mDragPoint).manhattanLength()  < QApplication::startDragDistance())
        return;

    mLabel->show();

    DoDrag();           //开始拖拽，完成拖拽后才会继续往下走

    mLabel->hide();
    mValidPress = false;
}

void  MyTableView::dragEnterEvent ( QDragEnterEvent * e )
{
    if (e->mimeData()->hasText())
	{
        e->acceptProposedAction();
	}
	else
	{
        e->ignore();
        QTableView::dragEnterEvent(e);
	}
}

void  MyTableView::dragMoveEvent ( QDragMoveEvent * e )
{
    if(e->mimeData()->hasText())
    {
		int nCurRow = 0;
        QModelIndex index = indexAt(e->pos());
        if (index.isValid())
        {
            if(e->pos().y() - index.row()*mRowHeight >= mRowHeight/2)
            {
                nCurRow = index.row() + 1;
            }
            else
            {
                nCurRow = index.row();
            }
        }
        else
        {
			nCurRow = mModel->rowCount();
        }
//        if (nCurRow != mRowFrom)
		{
            mRowTo = nCurRow;
			mLabel->setGeometry(1, mRowTo*mRowHeight, width() - 2, 2);
		}
        e->acceptProposedAction();
		return;
    }

    e->ignore();
    QTableView::dragMoveEvent(e);
}

void  MyTableView::dropEvent ( QDropEvent * e )
{
    if(e->mimeData()->hasText())
    {
        if (mRowTo != mRowFrom)
        {
            MoveRow(mRowFrom, mRowTo);
		}
        e->acceptProposedAction();
		return;
    }

    e->ignore();
    QTableView::dropEvent(e);
}

void MyTableView::DoDrag()
{
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    mimeData->setText(mDragText);
    drag->setMimeData(mimeData);

    // 设置拖拽图片
    QPixmap drag_img(width(), mRowHeight);
    drag_img.fill(QColor(255,255,255,100));
    QPainter painter(&drag_img);
    painter.setPen(QColor(0, 0, 0, 200));
    painter.drawText(QRectF(40,0,width(),mRowHeight), mDragText, QTextOption(Qt::AlignVCenter));

    drag->setPixmap(drag_img);
    drag->setHotSpot(mDragPointAtItem);

    //***注意：此句会阻塞，进入drag的拖拽消息循环，会依次触发dragEnterEvent、dragMoveEvent、dropEvent消息
    if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
    {
        // 确认移动操作
    }
}

void MyTableView::ResetOrder()
{
	for (int i = 0; i < mModel->rowCount(); i++)
	{
        mModel->item(i, 0)->setText( QString::number(i + 1) );
	}
}

void MyTableView::MoveRow(int from, int to)
{
	if (from == to)
	{
		return;
	}
	QStandardItem * item = mModel->item(from, 1);
	if (item)
	{
        QString strText = item->text();

		QList<QStandardItem*> items;
		QStandardItem* item0 = new QStandardItem("");
        QStandardItem* item1 = new QStandardItem(strText);
		items.append(item0);
		items.append(item1);
        item0->setTextAlignment(Qt::AlignCenter);

        //移动行思路：就是先在要移动到的位置新建同样内容的行，然后删除被移动的行
		mModel->insertRow(to, items);
		if (from < to)
		{
			mModel->removeRow(from);
			selectRow(to-1);
		}
		else
		{
			mModel->removeRow(from+1);
			selectRow(to);
		}
		ResetOrder();
        emit sigRowChange(mRowFrom,mRowTo);
		
	}
	
}
```
# 3.效果显示
## 3.1 拖拽加载图片
## 3.2 切换选择图片的暂时
## 3.3 拖拽更新列表顺序
## 3.4 生成PDF文件

# 4. 应用发布
+ 1.打开qt环境控制台
![在这里插入图片描述](https://img-blog.csdnimg.cn/direct/a54223cca0994b92ba8a16e63bf717e7.png)
+ 2. 新建文件夹test
将编译好的exe文件放置
![在这里插入图片描述](https://img-blog.csdnimg.cn/direct/fb60b1c7a3e9468eb3ae47c524b0e66b.png)
+ 3. 使用qt发布工具加载依赖dll
```cpp
windeployqt ImgToPDF.exe --release
```

![在这里插入图片描述](https://img-blog.csdnimg.cn/direct/8f3c284d6cc246a7aa77ed2c03ea6625.png)
![在这里插入图片描述](https://img-blog.csdnimg.cn/direct/1d8abda00d6d4adb958644ae464b494d.png)

直接发布压缩文件
# 5.源码地址
# 5.1 源码编译环境说明
# 5.2 源码地址
