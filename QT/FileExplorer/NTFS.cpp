#include "NTFS.h"
//Check
bool EndMFTEntry(BYTE* MFTEntry, int offset) {
    return getBytes(MFTEntry, offset, 4) == 0xffffffff;
}
bool isMFTEntry(MFTEntry MFTEntry) {
    return MFTEntry.Header.Signature[0] == 'F' && MFTEntry.Header.Signature[1] == 'I' && MFTEntry.Header.Signature[2] == 'L' && MFTEntry.Header.Signature[3] == 'E';
}
bool isBitSet(int num, int bitPosition) {
    // Dịch 1 sang trái để tạo một mặt nạ (mask) với bit tại vị trí cụ thể được đặt
    int mask = 1 << bitPosition;
    // Thực hiện phép AND giữa số nguyên và mặt nạ để kiểm tra bit tại vị trí cụ thể
    // Nếu kết quả khác 0, bit tại vị trí đó được đặt
    return (num & mask) != 0;
}
//Get
int64_t getBytes(BYTE* sector, int offset, int length) {
    int64_t result = 0;
    memcpy(&result, sector+offset, length);
    return result;
}
long long getSizeVolume(VBR VBR) {
    return VBR.TotalSectors * VBR.BytesPerSector;
}
void getTime(const char CreationTime[8], Time& time) {
    FILETIME ftCreationTime;
    memcpy(&ftCreationTime, CreationTime, sizeof(FILETIME));

    SYSTEMTIME stCreationTime;
    FileTimeToSystemTime(&ftCreationTime, &stCreationTime);

    time.Year = stCreationTime.wYear;
    time.Month = stCreationTime.wMonth;
    time.Day = stCreationTime.wDay;
    time.Hour = stCreationTime.wHour;
    time.Minute = stCreationTime.wMinute;
    time.Second = stCreationTime.wSecond;
}
string getFileName(MFT &MFT, int ID) {
    for(int i = 0; i < MFT.nMFTEntry; i++) {
        if(MFT.listMFTEntry[i].Header.ID == ID) {
            return MFT.listMFTEntry[i].Header.Filename;
        }
    }
    return "";
}
string getFileAttribute(MFT &MFT, int ID) {
    for(int i = 0; i < MFT.nMFTEntry; i++) {
        if(MFT.listMFTEntry[i].Header.ID == ID) {
            if(MFT.listMFTEntry[i].Header.FileAttribute == 0)
                return "File";
            else if(MFT.listMFTEntry[i].Header.FileAttribute == 1)
                return "Folder";
        }
    }
    return "";
}
string getCreationTime(MFT &MFT, int ID) {
    string s="";
    for(int i = 0; i < MFT.nMFTEntry; i++) {
        if(MFT.listMFTEntry[i].Header.ID == ID) {
            Time time;
            time = MFT.listMFTEntry[i].Header.CreationTime;
            stringstream ss;
            ss << setw(2) << setfill('0') << time.Hour << ":"
               << setw(2) << setfill('0') << time.Minute << ":"
               << setw(2) << setfill('0') << time.Second << "    "
               << setw(2) << setfill('0') << time.Day << "/"
               << setw(2) << setfill('0') << time.Month << "/"
               << setw(4) << setfill('0') << time.Year;

            s = ss.str();
        }
    }
    return s;
}
double getSize(MFT &MFT, int ID){
    for(int i = 0; i < MFT.nMFTEntry; i++) {
        if(MFT.listMFTEntry[i].Header.ID == ID) {
            return MFT.listMFTEntry[i].Header.UsedSize;
        }
    }
    return 0;
}
void getlistFile(MFT &MFT, vector<File> &listFile){
    for(int i = 0; i < MFT.nMFTEntry; i++) {
        File file;
        file.ID = MFT.listMFTEntry[i].Header.ID;
        file.parentID = MFT.listMFTEntry[i].Header.ParentID;
        file.Name = MFT.listMFTEntry[i].Header.Filename;
        file.Attribute = getFileAttribute(MFT, MFT.listMFTEntry[i].Header.ID);
        file.CreationTime = getCreationTime(MFT, MFT.listMFTEntry[i].Header.ID);
        file.Size = getSize(MFT, MFT.listMFTEntry[i].Header.ID);
        listFile.push_back(file);
    }
}
//Read
void readVBR(HANDLE hDrive, VBR &VBR) {
    BYTE Buffer_VBR[512];
    DWORD bytesRead;
    if (!ReadFile(hDrive, Buffer_VBR, sizeof(Buffer_VBR), &bytesRead, NULL)) {
        cerr << "Can't read bootsector" << std::endl;
        //CloseHandle(hDrive);
        return;
    }
    VBR.BytesPerSector = getBytes(Buffer_VBR, 0x0B, 2);
    VBR.SectorsPerCluster = getBytes(Buffer_VBR, 0x0D, 1);
    VBR.SectorsPerTrack = getBytes(Buffer_VBR, 0x18, 2);
    VBR.NumberOfHeads = getBytes(Buffer_VBR, 0x1A, 2);
    VBR.TotalSectors = getBytes(Buffer_VBR, 0x28, 8);
    VBR.MFTCluster = getBytes(Buffer_VBR, 0x30, 4);
    VBR.MFTMirrCluster = getBytes(Buffer_VBR, 0x38, 4);
    int8_t temp = getBytes(Buffer_VBR, 0x40, 1);
    int32_t exponent = (~temp + 1);
    VBR.BytesPerEntry = pow(2, exponent);
}
void readHeaderMFTEntry(BYTE* MFTEntry, HeaderMFTEntry &HeaderMFTEntry) {
    HeaderMFTEntry.Signature[0] = MFTEntry[0];
    HeaderMFTEntry.Signature[1] = MFTEntry[1];
    HeaderMFTEntry.Signature[2] = MFTEntry[2];
    HeaderMFTEntry.Signature[3] = MFTEntry[3];
    HeaderMFTEntry.FirstAttrOffset = getBytes(MFTEntry, 0x14, 2);
    HeaderMFTEntry.Flags = getBytes(MFTEntry, 0x16, 2);
    HeaderMFTEntry.UsedSize = getBytes(MFTEntry, 0x18, 4);
    HeaderMFTEntry.EntrySize = getBytes(MFTEntry, 0x1C, 4);
    HeaderMFTEntry.ID = getBytes(MFTEntry, 0x2C, 4);  
}
void readAttributeContent(BYTE* MFTEntry, HeaderMFTEntry &HeaderMFTEntry, Content &content, int ContentAttributeOffset) {
    //StandardContent
    if(content.Type == 0x10) {
        StandardContent* standardContent = new StandardContent;
        memcpy(standardContent->CreationTime, MFTEntry + ContentAttributeOffset, 8);
        memcpy(standardContent->ModificationTime, MFTEntry + ContentAttributeOffset + 0x08, 8);
        content = *standardContent;
        getTime(standardContent->CreationTime, HeaderMFTEntry.CreationTime);
    }
    //FileNameContent
    else if(content.Type == 0x30) {
        FileNameContent* fileNameContent = new FileNameContent;
        fileNameContent->ParentDirectory = getBytes(MFTEntry, ContentAttributeOffset, 6);
        fileNameContent->Attribute = getBytes(MFTEntry, ContentAttributeOffset + 0x38, 4);
        fileNameContent->NameLength = MFTEntry[ContentAttributeOffset + 0x40];
        fileNameContent->Name = string((char*)(MFTEntry + ContentAttributeOffset + 0x42), fileNameContent->NameLength*2); //UTF-16 mỗi ký tự có 2 bytes
        content = *fileNameContent;
        if((isBitSet(fileNameContent->Attribute, 5) || isBitSet(fileNameContent->Attribute, 28)) && HeaderMFTEntry.ID >=37)
        {
            HeaderMFTEntry.ParentID = fileNameContent->ParentDirectory;
            HeaderMFTEntry.Filename = fileNameContent->Name;
            if(isBitSet(fileNameContent->Attribute, 28))
                HeaderMFTEntry.FileAttribute = 1; //Folder
            else if(isBitSet(fileNameContent->Attribute, 5))
                HeaderMFTEntry.FileAttribute = 0;  //File
        }
             
        //cout << fileNameContent->Name << endl;
    }
    else if(content.Type == 0x80) {
        //cout << "Data" << endl;
    }
}
void readAttribute(BYTE* MFTEntry,HeaderMFTEntry &HeaderMFTEntry, Attribute &attribute, int HeaderAttributeOffset) {
    //Header
    attribute.Type = getBytes(MFTEntry, HeaderAttributeOffset, 4);
    attribute.Length = getBytes(MFTEntry, HeaderAttributeOffset + 0x04, 4);
    attribute.NonResFlag = MFTEntry[HeaderAttributeOffset + 0x08];
    //Content
    Content* content = new Content;
    content->Type = attribute.Type;
    int ContentAttributeOffset = HeaderAttributeOffset + getBytes(MFTEntry, HeaderAttributeOffset + 0x14, 2);
    readAttributeContent(MFTEntry, HeaderMFTEntry, *content, ContentAttributeOffset);
    attribute.Content = content;
}
void readMFTEntry(BYTE* Buffer_MFTEntry, MFTEntry &MFTEntry) {
    readHeaderMFTEntry(Buffer_MFTEntry, MFTEntry.Header);
    int HeaderAttributeOffset = MFTEntry.Header.FirstAttrOffset;
    int nAttribute = 0;
    while(!EndMFTEntry(Buffer_MFTEntry, HeaderAttributeOffset)){
        readAttribute(Buffer_MFTEntry, MFTEntry.Header, MFTEntry.ListAttribute[nAttribute], HeaderAttributeOffset);
        HeaderAttributeOffset += MFTEntry.ListAttribute[nAttribute].Length;
        nAttribute++;
    }
}
void readMFT(HANDLE hDrive, MFT &MFT, VBR VBR) {
    // Di chuyển con trỏ vị trí đầu tiền của MFT
    LARGE_INTEGER liDistanceToMove;
    liDistanceToMove.QuadPart = VBR.MFTCluster  * VBR.SectorsPerCluster * VBR.BytesPerSector;
    LARGE_INTEGER liNewFilePointer;
    if (!SetFilePointerEx(hDrive, liDistanceToMove, &liNewFilePointer, FILE_BEGIN)) {
        cerr << "Failed to set file pointer!" << endl;
        //CloseHandle(hDrive);
        return ;
    }
    const int sizePerEntry = VBR.BytesPerEntry;
    int nMFTEntry = 0;
    int numMFTEntry = -1;
    //$MFT
    BYTE* Buffer_MFTEntry = new BYTE[sizePerEntry]; 
    DWORD bytesRead;
    if (!ReadFile(hDrive, Buffer_MFTEntry, sizePerEntry, &bytesRead, NULL)) {
        cerr << "Can't read MFTEntry " << nMFTEntry << endl;
        //CloseHandle(hDrive);
        return;
    }
    MFTEntry TempMFTEntry;
    readMFTEntry(Buffer_MFTEntry, TempMFTEntry);
    if(nMFTEntry == 0){ //$MFT 
        int TotalClustersOfMFT = getBytes(Buffer_MFTEntry, 0x141, 1);
        numMFTEntry =  TotalClustersOfMFT*VBR.BytesPerSector*VBR.SectorsPerCluster / VBR.BytesPerEntry;
        MFT.listMFTEntry = new MFTEntry[numMFTEntry];
    }


    for(int i = 0;i < numMFTEntry;i++){
        BYTE* Buffer = new BYTE[sizePerEntry]; 
        DWORD Read;
        if (!ReadFile(hDrive, Buffer, sizePerEntry, &Read, NULL)) {
            cerr << "Can't read MFTEntry " << nMFTEntry << endl;
            //CloseHandle(hDrive);
            return;
        }
        if(Buffer[0] == 'F' && Buffer[1] == 'I' && Buffer[2] == 'L' && Buffer[3] == 'E')
            readMFTEntry(Buffer, MFT.listMFTEntry[nMFTEntry++]);
        liDistanceToMove.QuadPart += VBR.BytesPerEntry ;
        if (!SetFilePointerEx(hDrive, liDistanceToMove, &liNewFilePointer, FILE_BEGIN)) {
            cerr << "Failed to set file pointer!" << endl;
            //CloseHandle(hDrive);
            return ;
        }
    }
    MFT.nMFTEntry = nMFTEntry;
}
//Print
void printVBR(VBR &VBR) {
    cout << "VBR: " << endl;
    cout << " BytesPerSector: " << VBR.BytesPerSector << endl;
    cout << " SectorsPerCluster: " <<(int) VBR.SectorsPerCluster<< endl;
    cout << " SectorsPerTrack: " << VBR.SectorsPerTrack << endl;
    cout << " NumberOfHeads: " << VBR.NumberOfHeads << endl;
    cout << " TotalSectors: " << VBR.TotalSectors << endl;
    cout << " MFTCluster: " << VBR.MFTCluster << endl;
    cout << " MFTMirrCluster: " << VBR.MFTMirrCluster << endl;
    cout << " BytesPerEntry: " << (int)VBR.BytesPerEntry << endl;
}
void printFolderAndFile(vector<File> listFile, int parentFolderID, int level = 0) {
    for(auto& it : listFile) {
        if(it.parentID == parentFolderID) {
            for(int i = 0; i < level; i++) {
                cout << "\t";
            }
            cout <<it.Name << " " << it.Attribute << " " << it.CreationTime << " " << it.Size << endl;
            printFolderAndFile(listFile, it.ID, level + 1);
        }
    }
}

