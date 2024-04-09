#include "mainwindow.h"
#include "Main.h"
#include "./ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    displayTreeLabels();
    rootFAT = addRoot("FAT","Partition", "","");
    rootNTFS = addRoot("NTFS","Partition", "","");
    rootBIN = addRoot("BIN","Recycle Bin", "","");


    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);
}

MainWindow::~MainWindow()
{
    delete ui;

}
void MainWindow::displayTreeLabels(){
    ui->treeWidget->setColumnCount(4);
    QStringList lables;
    lables << "Name" << "Type"<< "Date Created"<< "Size";
    ui->treeWidget->setHeaderLabels(lables);
}
QTreeWidgetItem * MainWindow::addRoot(QString filename, QString type, QString time, QString size){
    QTreeWidgetItem *root = new QTreeWidgetItem(ui->treeWidget);
    root->setText(0, filename);
    if(type == "Recycle Bin")
        root->setIcon(0,QIcon("D:/imgBin.png"));
    else root->setIcon(0,QIcon("D:/imgDisk2.png"));
    root->setText(1, type);
    root->setText(2, time);
    root->setText(3, size);
    ui->treeWidget->addTopLevelItem(root);
    return root;
}
QTreeWidgetItem * MainWindow::addChild(QTreeWidgetItem *&root, QString filename, QString type, QString time, QString size,bool isFolder)
{
    QTreeWidgetItem *child = new QTreeWidgetItem();
    child->setText(0, filename);
    if(isFolder == 1)
        child->setIcon(0,QIcon("D:/imgFolder.png"));
    else if(filename.contains(".pdf", Qt::CaseInsensitive))
        child->setIcon(0,QIcon("D:/imgPDF.png"));
    else if(filename.contains(".png", Qt::CaseInsensitive))
        child->setIcon(0,QIcon("D:/imgPNG.png"));
    else child->setIcon(0,QIcon("D:/imgFile.png"));

    child->setText(1, type);
    child->setText(2, time);
    child->setText(3, size);
    root->addChild(child);
    return child;
}

void MainWindow::displayFile(vector<File> listFile, int parentFolderID, QTreeWidgetItem *parentItem)
{
    for(const auto &file : listFile)
    {
        if(file.parentID == parentFolderID && (file.isUsing == 1 || file.isUsing == 3)){
            QString name = QString::fromStdString(file.Name);
            QString attribute = QString::fromStdString(file.Attribute);
            QString creationTime = QString::fromStdString(file.CreationTime);
            QString size = QString::number(file.Size);


            if(attribute == "Folder")
            {
                QTreeWidgetItem *item = addChild(parentItem, name, attribute, creationTime, size,1); // Thêm thư mục hoặc tệp vào cây thư mục
                // Nếu là thư mục, gọi đệ quy để in các tệp và thư mục con của nó
                displayFile(listFile, file.ID, item);
            }
            else
                QTreeWidgetItem *item = addChild(parentItem, name, attribute, creationTime, size,0);
        }
    }
}
void MainWindow::displayFAT(Folder* f,vector<Item*>m,QTreeWidgetItem *parentItem)
{
    if(f==NULL)
    {
        for(auto x:m)
        {
            if(dynamic_cast<Folder*>(x))
            {
                QTreeWidgetItem *item=addChild(parentItem,QString::fromStdString(x->name),QString::fromStdString(AttributetoString(x->a)),QString::fromStdString(TimetoString(x->time,DaytoString(x->day))),QString::number(x->size),1);
                displayFAT(dynamic_cast<Folder*>(x),m,item);
            }
            else
                QTreeWidgetItem *item=addChild(parentItem,QString::fromStdString(x->name),QString::fromStdString(AttributetoString(x->a)),QString::fromStdString(TimetoString(x->time,DaytoString(x->day))),QString::number(x->size),0);
        }
    }
    else
    {
        for (auto x : f->items)
        {
            if (dynamic_cast<Folder*>(x))
            {
                QTreeWidgetItem *item=addChild(parentItem,QString::fromStdString(x->name),QString::fromStdString(AttributetoString(x->a)),QString::fromStdString(TimetoString(x->time,DaytoString(x->day))),QString::number(x->size),1);
                displayFAT(dynamic_cast<Folder*>(x),m,item);

            }
            else
                QTreeWidgetItem *item=addChild(parentItem,QString::fromStdString(x->name),QString::fromStdString(AttributetoString(x->a)),QString::fromStdString(TimetoString(x->time,DaytoString(x->day))),QString::number(x->size),0);
        }
    }
}
void MainWindow::updateProgram(Program p)
{
    P=p;
}
void MainWindow::displayFileContent(QString filename)
{
    for(auto file : this->WinListFile)
    {
        if(filename == QString::fromStdString(file.Name))
            ui->textBrowser->setText(QString::fromStdString(file.Data));
    }
}
QTreeWidgetItem * getRoot( QTreeWidgetItem * item){
    QTreeWidgetItem *parentItem = item->parent(); // Lấy cha trực tiếp của item

    while (parentItem && parentItem->parent()) {
        parentItem = parentItem->parent(); // Lặp để lấy cha cao nhất
    }
    return parentItem;
}
void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    // Lấy tên của item được double-clicked
    QString itemName = item->text(0);
     QTreeWidgetItem *root = getRoot(item);
    if(root->text(1) != "Recycle Bin")
        displayFileContent(itemName);
}
void MainWindow::showContextMenu(const QPoint &pos)
{
    QTreeWidgetItem *item = ui->treeWidget->itemAt(pos);
    if (!item)
        return;

    // Kiểm tra xem mục cha của mục được chọn có loại là "Recycle Bin" hay không
    QTreeWidgetItem *parentItem = item->parent();
    QTreeWidgetItem *root = getRoot(item);
    qDebug() << root->childCount();
    if (parentItem && root && root->text(1) == "Recycle Bin") {
        // Nếu mục cha có tên là "Recycle Bin", chỉ hiển thị tùy chọn "Restore"
        QMenu menu;
        QAction *restoreAction = menu.addAction("Restore");
        QAction *selectedAction = menu.exec(ui->treeWidget->mapToGlobal(pos));

        QString itemName = item->text(0);
        if (selectedAction == restoreAction) {
            qDebug() << "Restore action triggered for:" << item->text(0);
            for(auto it : deletedItemList){
                if(it.first == item){
                    int index = parentItem->indexOfChild(item);
                    parentItem->takeChild(index);
                    it.second->addChild(item);
                    for(auto &file : WinListFile){
                        if(QString::fromStdString(file.Name) == itemName)
                        {
                            if(file.isFAT == 0  && it.second->text(0) == QString::fromStdString(getParentItemName(WinListFile, file.ID)))
                            {
                               // restoreFile_NTFS(file.FirstOffset, file.isUsing);
                            }
                            else if(file.isFAT == 1)
                            {
                                string s;
                                string t=it.second->text(0).toStdString();
                                GetPname(P.m,t,s,file.FirstOffset);
                                if(s.empty())
                                    s=t;
                                if(it.second->text(0) == QString::fromStdString(s))
                                {
                                    //P.RecycleBin(itemName.toStdString());
                                }
                            }
                            deletedItemList.erase(remove_if(deletedItemList.begin(), deletedItemList.end(), [item,parentItem](const pair<QTreeWidgetItem *, QTreeWidgetItem *> &p){
                                                      return p.first == item && p.second == parentItem;
                                                  }), deletedItemList.end());
                        }

                    }
                }
            }
        }
    }
    else if(parentItem && item->text(1) != "Partition") {
        QString itemName = item->text(0);
        if (itemName.contains(".txt", Qt::CaseInsensitive) || (itemName.contains("TXT", Qt::CaseInsensitive))) {
            // Nếu chứa ".txt", hiển thị tất cả các tùy chọn menu
            QMenu menu;
            QAction *openAction = menu.addAction("Open");
            QAction *deleteAction = menu.addAction("Delete");

            QAction *selectedAction = menu.exec(ui->treeWidget->mapToGlobal(pos));
            if (selectedAction == openAction) {
                on_treeWidget_itemDoubleClicked(item, 0);
            }
            else if (selectedAction == deleteAction) {
                qDebug() << "Delete action triggered for:" << item->text(0);
                // Thêm xử lý xóa tệp txt ở đây
                for(auto &file : WinListFile)
                {
                    if(file.isUsing == 1){
                        QString parentItemName = parentItem->text(0);
                        if(QString::fromStdString(file.Name) == itemName)
                        {
                            //Delete NTFS File.txt
                            if(parentItemName == QString::fromStdString(getParentItemName(WinListFile, file.ID)) && file.isFAT == 0)
                            {
                                deletedItemList.push_back(make_pair(item, parentItem));
                                int index = parentItem->indexOfChild(item);
                                parentItem->takeChild(index);
                                rootBIN->addChild(item);
                                //deleteFile_NTFS(file.FirstOffset,file.isUsing);
                            }
                            //Delete FAT File.txt
                            else if(file.isFAT == 1)
                            {
                                string s;
                                qDebug()<<parentItemName;
                                string t=parentItemName.toStdString();
                                GetPname(P.m,t,s,file.FirstOffset);
                                if(s.empty())
                                    s=t;
                                if(parentItemName == QString::fromStdString(s))
                                {
                                    deletedItemList.push_back(make_pair(item, parentItem));
                                    int index = parentItem->indexOfChild(item);
                                    parentItem->takeChild(index);
                                    rootBIN->addChild(item);
                                   // P.deleteItem(file.Name,file.FirstOffset);
                                }
                            }
                        }
                    }
                }
            }
        } else {
            // Nếu không chứa ".txt", chỉ hiển thị tùy chọn "Delete"
            QMenu menu;
            QAction *deleteAction = menu.addAction("Delete");

            QAction *selectedAction = menu.exec(ui->treeWidget->mapToGlobal(pos));
            if (selectedAction == deleteAction) {
                qDebug() << "Delete action triggered for:" << item->text(0);
                // Thêm xử lý xóa tệp/thư mục ở đây
                for(auto &file : WinListFile)
                {
                    if(file.isUsing == 1 || file.isUsing == 3){
                        QString parentItemName = parentItem->text(0);
                        if(QString::fromStdString(file.Name) == itemName)
                        {
                            //Delete NTFS File/Folder
                            if(parentItemName == QString::fromStdString(getParentItemName(WinListFile, file.ID)) && file.isFAT == 0)
                            {
                                deletedItemList.push_back(make_pair(item, parentItem));
                                int index = parentItem->indexOfChild(item);
                                parentItem->takeChild(index);
                                rootBIN->addChild(item);
                                //deleteFile_NTFS(file.FirstOffset,file.isUsing);
                            }
                            //Delete FAT File/Folder
                            else if(file.isFAT == 1)
                            {
                                string s;
                                string t="FAT";
                                GetPname(P.m,t,s,file.FirstOffset);
                                if(parentItemName == QString::fromStdString(s))
                                {
                                    deletedItemList.push_back(make_pair(item, parentItem));
                                    int index = parentItem->indexOfChild(item);
                                    parentItem->takeChild(index);
                                    rootBIN->addChild(item);
                                   // P.deleteItem(file.Name,file.FirstOffset);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
