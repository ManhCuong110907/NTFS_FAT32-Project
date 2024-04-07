#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QDebug>
#include "NTFS.h"
#include"FAT32.h"
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
    vector<File> WinListFile;
    Program P;
    QTreeWidgetItem *rootNTFS = NULL;
    QTreeWidgetItem *rootFAT = NULL;
    QTreeWidgetItem *rootBIN = NULL;
    vector<pair<QTreeWidgetItem*, QTreeWidgetItem*>> deletedItemList; //first: item, second: oldparent
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateProgram(Program p);
    QTreeWidgetItem * addRoot(QString filename, QString type, QString time, QString size);
    QTreeWidgetItem * addChild(QTreeWidgetItem *&root,QString filename, QString type, QString time, QString size,bool isFolder);
    void displayFile(vector<File> listFile, int parentFolderID, QTreeWidgetItem *parentItem);
    void displayTreeLabels();
    void displayFAT(Folder* f,vector<Item*>m,QTreeWidgetItem *parentItem);
    void displayFileContent(QString filename);

private slots:

    void showContextMenu(const QPoint &pos);
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
};
#endif // MAINWINDOW_H
