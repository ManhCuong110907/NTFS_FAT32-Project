#include"Main.h"
#include "mainwindow.h"
#include "NTFS.h"
#include "ui_mainwindow.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    const char* drive = "\\\\.\\E:"; // Đường dẫn đến ổ đĩa D
    HANDLE hDrive = CreateFileA(drive, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    if (hDrive == INVALID_HANDLE_VALUE) {
        std::cerr << "Can't open disk" << std::endl;
        return 1;
    }
    VBR VBR;
    readVBR(hDrive, VBR);
    MFT MFT;
    readMFT(hDrive, MFT, VBR);
    vector<File> listFile;
    getlistFile(MFT, listFile);
    updateSize(listFile, 5);

    printFolderAndFile(listFile, 5, 0);
    CloseHandle(hDrive);


    BootSector bs;
    bs.ReadBootSector();
    FAT f;
    f.ReadFAT(bs.NumberSector_BFAT * bs.Size_Sector,bs.NumberSector_FAT*bs.Size_Sector*bs.Number_FAT);
    RDET rdet;
    rdet.Readdata((bs.NumberSector_BFAT + bs.NumberSector_FAT * bs.Number_FAT) * bs.Size_Sector);
    Program p(f,bs,rdet);
    vector<int> v;
    p.ReadItem(NULL,rdet.subEntry,v);
    p.updateFoldersize();
    w.updateProgram(p);
    vector<File> listFile2;
    updateListFile(p.m,listFile2);
    listFile.insert(listFile.end(),listFile2.begin(),listFile2.end());
    w.WinListFile = listFile;
    w.displayTreeLabels();
    QItem it;
    it.name = "FAT";
    it.attribute = "Partition";
    it.creationTime = "";
    it.size = "";
    QTreeWidgetItem *root = w.addRoot(it);
    w.displayFAT(p.m,root);
    it.name = "FAT";
    QTreeWidgetItem *root1 = w.addRoot(it);
    it.name = "Recycle Bin";
    QTreeWidgetItem *root2 = w.addRoot(it);
    root2->setIcon(0,QIcon("D:/RecycleBin.png"));
    QTreeWidgetItem *item = w.addChild(root2,it,0);
    w.displayFile(listFile, 5,root1);
    w.updateProgram(p);
    w.show();
    return a.exec();
}
