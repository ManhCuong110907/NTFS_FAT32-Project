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
    ui->treeWidget->setColumnCount(2);
    QStringList lables;
    lables << "Name" << "Type" << "Date Created" << "Size";
    ui->treeWidget->setHeaderLabels(lables);
}
QTreeWidgetItem * MainWindow::addRoot(QString filename, QString type, QString time, QString size){
    QTreeWidgetItem *root = new QTreeWidgetItem(ui->treeWidget);
    root->setText(0, filename);
    root->setText(1, type);
    root->setText(2, time);
    root->setText(3, size);
    ui->treeWidget->addTopLevelItem(root);
    return root;
}
QTreeWidgetItem * MainWindow::addChild(QTreeWidgetItem *&root, QString filename, QString type, QString time, QString size){
    QTreeWidgetItem *child = new QTreeWidgetItem();
    child->setText(0, filename);
    child->setText(1, type);
    child->setText(2, time);
    child->setText(3, size);
    root->addChild(child);
    return child;
}
void MainWindow::displayFile(vector<File> listFile, int parentFolderID, QTreeWidgetItem *parentItem){
    for(const auto &file : listFile)
    {
        if(file.parentID == parentFolderID){
            QString name = QString::fromStdString(file.Name);
            QString attribute = QString::fromStdString(file.Attribute);
            QString creationTime = QString::fromStdString(file.CreationTime);
            QString size = QString::number(file.Size);

            QTreeWidgetItem *item = addChild(parentItem, name, attribute, creationTime, size); // Thêm thư mục hoặc tệp vào cây thư mục
            if(attribute == "Folder")
            {
                // Nếu là thư mục, gọi đệ quy để in các tệp và thư mục con của nó
                displayFile(listFile, file.ID, item);
            }
        }
    }
}
