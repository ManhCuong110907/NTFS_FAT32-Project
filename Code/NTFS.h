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
    uint8_t BytesPerEntry;
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
    uint32_t Type;
};
class StandardContent: public Content{
    uint64_t CreationTime;
    uint64_t ModificationTime;
};
class FileNameContent: public Content{
    uint64_t ParentDirectory;
    uint32_t Attribute;
    uint8_t NameLength;
    uint8_t Namespace;
};
struct Attribute{
    uint32_t Type;
    uint32_t Length;
    uint8_t NonResFlag;
    Content* content;
};
struct MFTEntry{
    HeaderMFTEntry Header;
    Attribute *listAttribute;
};
struct MFT{
    MFTEntry* listMFTEntry;
};
void readVBR(ifstream &drive, VBR &VBR);
void readMFTEntry(ifstream &drive, MFTEntry &MFTEntry);
void readMFT(ifstream &drive, MFT &MFT);
#pragma pack(pop)