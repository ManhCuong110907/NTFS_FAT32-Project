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
struct QItem
{
    QString name;
    QString attribute;
    QString creationTime;
    QString size;
};
class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Program P;
public:
    void updateProgram(Program p);
    Ui::MainWindow *ui;
    vector<File> WinListFile;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTreeWidgetItem * addRoot(QItem item);
    QTreeWidgetItem * addChild(QTreeWidgetItem *&root,QItem item,bool isFolder);
    void displayFile(vector<File> listFile, int parentFolderID, QTreeWidgetItem *parentItem);
    void displayTreeLabels();
    void displayFAT(vector<Item*>m,QTreeWidgetItem *parentItem);
    void displayFileContent(QString filename);

private slots:

    void showContextMenu(const QPoint &pos);
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
};


#endif
