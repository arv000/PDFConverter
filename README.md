# һ������˵��
����ڻ�����������Ҫ�����е�ͼƬ���������֤����ҵ֤����Ϣ����Ҫ�ϲ���һ��pdf�С�����������ּ�������Ҫ�շѣ��ɴ���Լ�дһ�����ˡ�
+ ����˵�����£�
	1. ֧������϶�����ͼƬ�����Դ����潫ͼƬ���͵��ļ��ϵ�����У�һ��������150�š�
	2. ֧�ֵ���˵��е���Ӱ�ť���ͼƬ��
	3. ֧�ֲ˵���ѡ��Ŀ¼��Ŀ¼������ͼƬ��ӵ�����У�������150�š�
	4. ���м��ص�ͼƬ��ַ��Ҫ���б����չʾ��
	5. ����ƶ����б��ĵ�ַ����ʱ����Ҫչʾ������ͼƬ��ַ��
	6. ֧��ͨ������϶��л�ͼƬ��˳��
	7. ֧���Ҽ�ɾ��ѡ�е�ͼƬ��
	8. �������ܹ��������ѡ���ͼƬ
	9. ѡ�е�ͼƬ����߽���ͼƬչʾ������չʾ��
	10. ����pdfͼƬʱ��Ҫ�н�������ʾ��
	11. ����PDF�ļ��������ʾ������б�

�����λ�������󣬿������������������������������һᾡ�����㡣


# ������Ҫ�����з�˵��
## 2.1 QTableView�Ҽ��͵�
1. �������������Ĳ˵�����Ϊ�Զ���
```cpp
setContextMenuPolicy(Qt::CustomContextMenu);
```
2. ���������Ĳ˵������ź�
```cpp
connect(this, &QTableView::customContextMenuRequested, this, &ImageListWidget::slotShowContextMenu);
 ```
3. �����˵����Ұ󶨲˵��ź�
```cpp
void ImageListWidget::slotShowContextMenu(const QPoint &pos)
{
    // �����Ҽ��˵�
    QMenu contextMenu(tr("Context Menu"), this);

    // ��Ӳ˵���
    QAction action1("ɾ��", this);
    connect(&action1, &QAction::triggered, this, &ImageListWidget::slotDeleteImageItem);
    contextMenu.addAction(&action1);

    // ������Ҽ������λ����ʾ�˵�
    contextMenu.exec(mapToGlobal(pos));
}

```
4. ɾ���źŴ���
�Ȼ�ȡ��ǰ��ѡ����У�Ȼ����model��ɾ����Ӧ���С�
ͨ��currentIndex()��ȡ��ǰѡ�е��С�
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
## 2.2 Qt��ʾ������
��������ʾ��Ҫʹ�õ�Qt��ԭ���ĵ�������ʾ
+ ��ʼ��������
```cpp
    qProgressDialog = new QProgressDialog("Processing...", "Cancel", 0, 100, this);
    qProgressDialog->show();
```
����Ϣ�� ��������һ������ʱ֪ͨ���������½���
```cpp
	// ���������ʱ֪ͨ����������ɣ�
    connect(pdfSaveThread,&QThread::finished,this,&MainWidget::slotDoFinished);
	connect(pdfSaveThread,&PdfSaveThread::sigUpdateProcess,this,&MainWidget::slotupdateProcess);
```
+ ���ý���
```cpp
void MainWidget::slotupdateProcess(int value)
{
    if(qProgressDialog){
        qProgressDialog->setValue(value);
    }
}
```
+ ������ɣ����ȵ���100���������ֵΪ100.����ɾ�����ȵ���Ȼ��ظ���ť״̬��
```cpp
void MainWidget::slotDoFinished()
{
    QMessageBox::information(this,"��ʾ","�ļ����ɳɹ�");
    if(qProgressDialog){
        qProgressDialog->setValue(100);
        delete qProgressDialog;
        qProgressDialog = nullptr;
    }
    btnDoSavePDF->setDisabled(false);
}

```
## 2.3 QtͼƬ����pdf
pdf��������Ҫʹ�õ�printer����������ڹ����ļ�����Ҫ����
```shll
QT       += core gui printsupport
```
����pdfͼƬ����
```cpp
// ���ø����ӡ
  QPrinter printer(QPrinter::PrinterMode::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(m_savePDFFileName);

    QPainter painter(&printer);
	// ����ͼƬ·��
    for (int i = 0; i < m_filePaths.size(); ++i) {
        QString filePath = m_filePaths.at(i);
        filePath.replace("file:///","");
        // ���ر���ͼƬ
        QImage image(filePath);
        if (image.isNull()) {
            continue;  // Skip invalid images
        }

        // Scale the image to fit the page while maintaining aspect ratio
        QSize imageSize = image.size();
        imageSize.scale(printer.pageRect().size(), Qt::KeepAspectRatio);
		// ���þ���չʾ
        QPoint center = printer.pageRect().center() - QPoint(imageSize.width() / 2, imageSize.height() / 2);
        painter.drawImage(center, image.scaled(imageSize, Qt::KeepAspectRatio));
        // ֪ͨ����
        emit sigUpdateProcess((i*100/m_filePaths.size()));
        if (i < m_filePaths.size() - 1) {
            printer.newPage();  // Start a new page for the next image
        }
    }

    painter.end();
```
QPrinter::PrinterMode
ScreenResolution:
���ã����ô�ӡ�ķֱ�������Ļ�ķֱ�����ͬ��ͨ���ϵ͡������ڿ���Ԥ���Ͳݸ��ӡ��
PrinterResolution:
���ã����ô�ӡ����Ĭ�Ϸֱ��ʡ���ͨ���Ǵ�ӡ�����ܴ������ѷֱ��ʣ���������ʽ�ĵ��͸�������ӡ��
## 2.4 Qt��ק�����б�˳��
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

    DoDrag();           //��ʼ��ק�������ק��Ż����������

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

    // ������קͼƬ
    QPixmap drag_img(width(), mRowHeight);
    drag_img.fill(QColor(255,255,255,100));
    QPainter painter(&drag_img);
    painter.setPen(QColor(0, 0, 0, 200));
    painter.drawText(QRectF(40,0,width(),mRowHeight), mDragText, QTextOption(Qt::AlignVCenter));

    drag->setPixmap(drag_img);
    drag->setHotSpot(mDragPointAtItem);

    //***ע�⣺�˾������������drag����ק��Ϣѭ���������δ���dragEnterEvent��dragMoveEvent��dropEvent��Ϣ
    if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
    {
        // ȷ���ƶ�����
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

        //�ƶ���˼·����������Ҫ�ƶ�����λ���½�ͬ�����ݵ��У�Ȼ��ɾ�����ƶ�����
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
# 3.Ч����ʾ
## 3.1 ��ק����ͼƬ
## 3.2 �л�ѡ��ͼƬ����ʱ
## 3.3 ��ק�����б�˳��
## 3.4 ����PDF�ļ�

# 4. Ӧ�÷���
+ 1.��qt��������̨
![���������ͼƬ����](https://img-blog.csdnimg.cn/direct/a54223cca0994b92ba8a16e63bf717e7.png)
+ 2. �½��ļ���test
������õ�exe�ļ�����
![���������ͼƬ����](https://img-blog.csdnimg.cn/direct/fb60b1c7a3e9468eb3ae47c524b0e66b.png)
+ 3. ʹ��qt�������߼�������dll
```cpp
windeployqt ImgToPDF.exe --release
```

![���������ͼƬ����](https://img-blog.csdnimg.cn/direct/8f3c284d6cc246a7aa77ed2c03ea6625.png)
![���������ͼƬ����](https://img-blog.csdnimg.cn/direct/1d8abda00d6d4adb958644ae464b494d.png)

ֱ�ӷ���ѹ���ļ�
# 5.Դ���ַ
# 5.1 Դ����뻷��˵��
# 5.2 Դ���ַ
