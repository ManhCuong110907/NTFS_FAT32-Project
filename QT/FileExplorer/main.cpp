#include"Main.h"
#include "mainwindow.h"
#include "NTFS.h"
#include "ui_mainwindow.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    const char* drive = "\\\\.\\D:"; // Đường dẫn đến ổ đĩa D
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
    w.WinListFile = listFile;
    File NTFS={5,0,"NTFS","","",0,"",0,-1};
    w.WinListFile.erase(w.WinListFile.begin() + 5);
    w.WinListFile.push_back(NTFS);
    //printFolderAndFile(listFile, 5, 0);
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

    w.displayFAT(NULL,p.m,w.rootFAT);
    w.displayFile(listFile, 5,w.rootNTFS);

    w.show();
    return a.exec();
}
