#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QDebug>
#include "NTFS.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void addRoot(QTreeWidgetItem *&root, QString filename, QString type, QString time, QString size);
    void addChild(QTreeWidgetItem *&root,QString filename, QString type, QString time, QString size);
    void displayFile(MFT &MFT, int parentFolderID, QTreeWidgetItem *parentItem);
    void displayTreeLabels();

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
