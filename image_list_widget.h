#ifndef IMAGELISTWIDGET_H
#define IMAGELISTWIDGET_H

#include <QTableWidget>
#include <QDragMoveEvent>
#include <QLabel>

class QStandardItemModel;
class ImageListWidget : public QTableView {
    Q_OBJECT
   public:
    ImageListWidget(QWidget *_parent_widget = nullptr);
    void insertData(QString fileName);
    /**
     * @brief 获取文件列表
     * @return
     */
    QVector<QString> getFileLists();

   private:
    QStandardItemModel *db_table_model_;
    QLabel *draggingLine;
    int mRowHeight;  //表格的行高

    bool mValidPress;  //在鼠标移动时，判断之前按下时是否是在有效行上
    int mRowFrom;  //拖动起始行
    int mRowTo;  //拖动时(还没松开时)鼠标位置代表的行，会绘制一条指示线，类似QQ好友列表拖拽效果

    QString mDragText;  //拖拽过程中跟随鼠标显示的内容
    QPoint mDragPoint;  //拖拽起点
    QPoint
        mDragPointAtItem;  //记录按下时鼠标相对该行的位置，在拖动过程中保持该相对位置
   private:
    /**
     * @brief 文件是否已经存在
     * @return
     */
    bool isFileExited(QString filePath);
    void initUI();
    void initConnect();
    bool isPicture(QString filePath);
    QString getFileName(QString filePath);
    void DoDrag();                   //执行拖拽
    void ResetOrder();               //重置第一列序号
    void MoveRow(int from, int to);  //真正执行移动行的功能
    // QWidget interface
   protected:
    // 根据鼠标事件开启拖拽
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    //拖拽响应事件
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
   signals:
    //拖拽结束后会发出此信号，可绑定槽函数实现你的其他功能，比如把新的顺序存到文件
    void sigRowChange(int from, int to);
    void sigSelectFilePathChange(QString filePath);

   protected slots:
    void selectionChanged(const QItemSelection &selected,
                          const QItemSelection &deselected) override;
    void slotShowContextMenu(const QPoint &pos);

    void slotDeleteImageItem();
};

#endif  // IMAGELISTWIDGET_H
