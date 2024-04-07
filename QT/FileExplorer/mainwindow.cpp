#include "mainwindow.h"
#include "Main.h"
#include "./ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
QTreeWidgetItem * MainWindow::addRoot(QItem item){
    QTreeWidgetItem *root = new QTreeWidgetItem(ui->treeWidget);
    root->setText(0, item.name);
    root->setIcon(0,QIcon("D:/imgDisk2.png"));
    root->setText(1, item.attribute);
    root->setText(2, item.creationTime);
    root->setText(3, item.size);
    ui->treeWidget->addTopLevelItem(root);
    return root;
}
QTreeWidgetItem * MainWindow::addChild(QTreeWidgetItem *&root,QItem item,bool isFolder)
{
    QTreeWidgetItem *child = new QTreeWidgetItem();
    child->setText(0, item.name);
    if(isFolder==1)
        child->setIcon(0,QIcon("D:/imgFolder.png"));
    else
        child->setIcon(0,QIcon("D:/imgFile.png"));
    child->setText(1, item.attribute);
    child->setText(2, item.creationTime);
    child->setText(3, item.size);
    root->addChild(child);
    return child;
}

void MainWindow::displayFile(vector<File> listFile, int parentFolderID, QTreeWidgetItem *parentItem)
{
    for(const auto &file : listFile)
    {
        if(file.parentID == parentFolderID && (file.isUsing == 1 || file.isUsing == 3)){
            QItem it;
            it.name = QString::fromStdString(file.Name);
            it.attribute = QString::fromStdString(file.Attribute);
            it.creationTime = QString::fromStdString(file.CreationTime);
            it.size = QString::number(file.Size);


            if(it.attribute == "Folder")
            {
                QTreeWidgetItem *item = addChild(parentItem,it,1); // Thêm thư mục hoặc tệp vào cây thư mục
                // Nếu là thư mục, gọi đệ quy để in các tệp và thư mục con của nó
                displayFile(listFile, file.ID, item);
            }
            else
                QTreeWidgetItem *item = addChild(parentItem,it,0);
        }
    }
}
void MainWindow::displayFAT(vector<Item*>m,QTreeWidgetItem *parentItem)
{
        for(auto x:m)
        {
            Folder *f=dynamic_cast<Folder*>(x);
            QItem it;
            it.name = QString::fromStdString(x->name);
            it.attribute = QString::fromStdString(AttributetoString(x->a));
            it.creationTime = QString::fromStdString(TimetoString(x->time,DaytoString(x->day)));
            it.size = QString::number(x->size);
            if(f)
            {

                QTreeWidgetItem *item=addChild(parentItem,it,1);
                displayFAT(f->items,item);
            }
            else
                QTreeWidgetItem *item=addChild(parentItem,it,0);
        }
    }

void MainWindow::displayFileContent(QString filename)
{
    for(auto file : this->WinListFile)
    {
        if(filename == QString::fromStdString(file.Name))
            ui->textBrowser->setText(QString::fromStdString(file.Data));
    }
}


void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    // Lấy tên của item được double-clicked
    QString itemName = item->text(0); // Giả sử tên tệp được lưu trong cột đầu tiên (có chỉ số là 0)
    displayFileContent(itemName);
}
void MainWindow::showContextMenu(const QPoint &pos)
{
    QTreeWidgetItem *item = ui->treeWidget->itemAt(pos);
    if (!item)
        return;

    // Kiểm tra xem mục cha của mục được chọn có tên là "Recycle Bin" hay không
    QTreeWidgetItem *parentItem = ui->treeWidget->topLevelItem(ui->treeWidget->indexOfTopLevelItem(item));
    QTreeWidgetItem *parentItem =item->parent();
    parentItem->text(0);
    if (parentItem && parentItem->text(0) == "Recycle Bin") {
        // Nếu mục cha có tên là "Recycle Bin", chỉ hiển thị tùy chọn "Restore"
        QMenu menu;
        QAction *restoreAction = menu.addAction("Restore");

        QAction *selectedAction = menu.exec(ui->treeWidget->mapToGlobal(pos));
        if (selectedAction == restoreAction) {
            qDebug() << "Restore action triggered for:" << item->text(0);
            // Thêm xử lý khôi phục tệp/thư mục ở đây
        }
    } else {
        // Nếu không phải "Recycle Bin", kiểm tra xem mục có chứa ".txt" không
        QString itemName = item->text(0);
        if (itemName.contains(".txt", Qt::CaseInsensitive)) {
            // Nếu chứa ".txt", hiển thị tất cả các tùy chọn menu
            QMenu menu;
            QAction *openAction = menu.addAction("Open");
            QAction *deleteAction = menu.addAction("Delete");

            QAction *selectedAction = menu.exec(ui->treeWidget->mapToGlobal(pos));
            if (selectedAction == openAction) {
                // Gọi hàm xử lý tương tự như khi double-click
                on_treeWidget_itemDoubleClicked(item, 0); // 0 là chỉ số cột, có thể thay đổi nếu cần
            }
            else if (selectedAction == deleteAction) {
                string s=item->text(0).toStdString();
                if(parentItem->text(0) == "FAT")
                    P.FindItem(P.m,s);
                // Thêm xử lý xóa tệp/thư mục ở đây

            }
        } else {
            // Nếu không chứa ".txt", chỉ hiển thị tùy chọn "Delete"
            QMenu menu;
            QAction *deleteAction = menu.addAction("Delete");

            QAction *selectedAction = menu.exec(ui->treeWidget->mapToGlobal(pos));
            if (selectedAction == deleteAction) {
                qDebug() << "Delete action triggered for:" << item->text(0);
                // Thêm xử lý xóa tệp/thư mục ở đây
            }
        }
    }
}
void MainWindow::updateProgram(Program p)
{
    P=p;
}


