#include "mainwindow.h"
#include "Main.h"
#include "./ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    root->setIcon(0,QIcon("D:/imgDisk2.png"));
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
    if(isFolder==1)
        child->setIcon(0,QIcon("D:/imgFolder.png"));
    else
        child->setIcon(0,QIcon("D:/imgFile.png"));
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

