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
    lables << "Name" << "Type" << "DateCreated" << "Size";
    ui->treeWidget->setHeaderLabels(lables);
}
void MainWindow::addRoot(QTreeWidgetItem *&root, QString filename, QString type, QString time, QString size){
    root->setText(0, filename);
    root->setText(1, type);
    root->setText(2, time);
    root->setText(3, size);
    ui->treeWidget->addTopLevelItem(root);
}
void MainWindow::addChild(QTreeWidgetItem *&root, QString filename, QString type, QString time, QString size){
    QTreeWidgetItem *child = new QTreeWidgetItem();
    child->setText(0, filename);
    child->setText(1, type);
    child->setText(2, time);
    child->setText(3, size);
    root->addChild(child);
}
// void MainWindow::displayFile(MFT &MFT, int parentFolderID){
//     for(auto it : listFile)
//     {
//         if(it.second == parentFolderID){
//             QTreeWidgetItem *root = new QTreeWidgetItem(ui->treeWidget);
//             addRoot(root, QString::fromStdString(getFileName(MFT, it.first)),QString::fromStdString(getFileAttribute(MFT, it.first)),QString::fromStdString(getCreationTime(MFT, it.first)), QString::number(getSize(MFT, it.first)));
//             //displayFile(MFT, it.second);
//         }
//     }
// }
void MainWindow::displayFile(MFT &MFT, int parentFolderID, QTreeWidgetItem *parentItem){
    for(auto it : listFile)
    {
        if(it.second == parentFolderID){
            QTreeWidgetItem *item;
            if (parentItem != nullptr) {
                item = new QTreeWidgetItem(parentItem);
            } else {
                item = new QTreeWidgetItem(ui->treeWidget);
            }
            // if(getFileAttribute(MFT, it.first) == "Folder") ///Bugggg
            // {
                addRoot(item, QString::fromStdString(getFileName(MFT, it.first)),QString::fromStdString(getFileAttribute(MFT, it.first)),QString::fromStdString(getCreationTime(MFT, it.first)), QString::number(getSize(MFT, it.first)));
                displayFile(MFT, it.first, item);
            //}
            if(getFileAttribute(MFT, it.first) == "File")
                addChild(item, QString::fromStdString(getFileName(MFT, it.first)),QString::fromStdString(getFileAttribute(MFT, it.first)),QString::fromStdString(getCreationTime(MFT, it.first)), QString::number(getSize(MFT, it.first)));
        }
    }
}
