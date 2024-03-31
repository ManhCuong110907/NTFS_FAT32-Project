#pragma once
#include  "Main.h"
#pragma pack(push, 1)
struct VBR{
    uint16_t BytesPerSector;
    uint8_t SectorsPerCluster;
    uint16_t SectorsPerTrack;
    uint16_t NumberOfHeads;
    uint32_t TotalSectors;
    uint32_t MFTCluster;
    uint32_t MFTMirrCluster;
    uint32_t BytesPerEntry;
};
struct HeaderMFTEntry{
    char Signature[4];
    uint16_t FirstAttrOffset;
    uint16_t Flags;
    uint32_t UsedSize;
    uint32_t EntrySize;
    uint32_t ID;
};
class Content{
public:
    uint32_t Type;
};
class StandardContent: public Content{
public:
    char CreationTime[8];
    char ModificationTime[8];
};
class FileNameContent: public Content{
public:
    uint64_t ParentDirectory;
    uint32_t Attribute;
    uint8_t NameLength;
    string Name;
};
struct Attribute{
    uint32_t Type;
    uint32_t Length;
    uint8_t NonResFlag;
    Content* Content;
};
struct MFTEntry{
    HeaderMFTEntry Header;
    Attribute ListAttribute[100];
};
struct MFT{
    int nMFTEntry;
    MFTEntry* listMFTEntry;
};
//static vector<pair<int,int>> listFile = {}; 
//Get
int getSizePerEntry(VBR VBR);
int64_t getBytes(BYTE* sector, int offset, int length);
void getTime(const char CreationTime[8]);
//Read
void readVBR(HANDLE hDrive, VBR &VBR);
void readHeaderMFTEntry(BYTE* MFTEntry, HeaderMFTEntry &HeaderMFTEntry);
void readMFTEntry(BYTE* Buffer_MFTEntry, MFTEntry &MFTEntry);
void readAttributeContent(BYTE* MFTEntry, HeaderMFTEntry HeaderMFTEntry, Content &content, int ContentAttributeOffset);
void readAttribute(BYTE* MFTEntry,HeaderMFTEntry HeaderMFTEntry, Attribute &attribute, int HeaderAttributeOffset, File &file);
void readMFT(HANDLE hDrive, MFT &MFT, VBR VBR);

//Print
void printVBR(VBR &VBR);
void printHeaderMFTEntry(HeaderMFTEntry &HeaderMFTEntry);
//void printFolderAndFile(MFT &MFT);
void printFolderAndFile(MFT &MFT, int parentFolderID, int level );
struct Time{
    int Year;
    int Month;
    int Day;
    int Hour;
    int Minute;
    int Second;
};
struct File{
    int ID;
    string Name;
    int ParentID;
    Time CreationTime;
    Time ModificationTime;
    int Attribute;
    int TotalSize;
};
static vector<File> listFile = {};





#pragma pack(pop)
