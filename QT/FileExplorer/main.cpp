#include"Main.h"
// #include "mainwindow.h"
#include "NTFS.h"
// #include "ui_mainwindow.h"
// #include <QApplication>
int main(int argc, char *argv[])
{
    // QApplication a(argc, argv);
    // MainWindow w;

    const char* drive = "\\\\.\\E:"; // Đường dẫn đến ổ đĩa D
    HANDLE hDrive = CreateFileA(drive, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    if (hDrive == INVALID_HANDLE_VALUE) {
        std::cerr << "Can't open disk" << std::endl;
        return 1;
    }
    VBR VBR;
    readVBR(hDrive, VBR);
    //printVBR(VBR);
    MFT MFT;
    readMFT(hDrive, MFT, VBR);
    vector<File> listFile;
    getlistFile(MFT, listFile);
    updateSize(listFile,5);
    //printFolderAndFile(listFile, 5, 0);
    CloseHandle(hDrive); // Đóng handle của ổ đĩa
    // w.displayTreeLabels();
    // QTreeWidgetItem *root = w.addRoot("NTFS","Partition", "","");
    // w.displayFile(listFile, 5, root);
    // w.show();
    // return a.exec();
}
