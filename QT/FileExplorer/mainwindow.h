#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QDebug>
#include "NTFS.h"
#include"FAT.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Ui::MainWindow *ui;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTreeWidgetItem * addRoot(QString filename, QString type, QString time, QString size);
    QTreeWidgetItem * addChild(QTreeWidgetItem *&root,QString filename, QString type, QString time, QString size,bool isFolder);
    void displayFile(vector<File> listFile, int parentFolderID, QTreeWidgetItem *parentItem);
    void displayTreeLabels();
    void displayFAT(Folder* f,vector<Item*>m,QTreeWidgetItem *parentItem);

};
#endif // MAINWINDOW_H
