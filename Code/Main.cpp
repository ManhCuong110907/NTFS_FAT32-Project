#include"Main.h"
int main(){
    const char* drive = "\\\\.\\D:"; // Đường dẫn đến ổ đĩa D
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
    printFolderAndFile(MFT, 5, 0);
    CloseHandle(hDrive); // Đóng handle của ổ đĩa
}
