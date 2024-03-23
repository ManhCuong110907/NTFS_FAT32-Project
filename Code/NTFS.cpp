#include "NTFS.h"
void readVBR(ifstream &drive, VBR &VBR) {
    drive.seekg(11, ios::beg);
    drive.read((char*)&VBR.BytesPerSector, sizeof(VBR.BytesPerSector));
    drive.read((char*)&VBR.SectorsPerCluster, sizeof(VBR.SectorsPerCluster));
    drive.seekg(24, ios::beg);
    drive.read((char*)&VBR.SectorsPerTrack, sizeof(VBR.SectorsPerTrack));
    drive.read((char*)&VBR.NumberOfHeads, sizeof(VBR.NumberOfHeads));
    drive.seekg(40, ios::beg);
    drive.read((char*)&VBR.TotalSectors, sizeof(VBR.TotalSectors));
    drive.seekg(48, ios::beg);
    drive.read((char*)&VBR.MFTCluster, sizeof(VBR.MFTCluster));
    drive.seekg(56, ios::beg);
    drive.read((char*)&VBR.MFTMirrCluster, sizeof(VBR.MFTMirrCluster));
    uint8_t temp = 0;
    drive.seekg(64, ios::beg);
    drive.read((char*)&temp, sizeof(temp));
    VBR.BytesPerEntry = 2^temp; //chua xu ly
}
void readMFTEntry(ifstream &drive, HeaderMFTEntry &HeaderMFTEntry) {
    streampos MFTEntryStart = drive.tellg();
    drive.seekg(MFTEntryStart);
    drive.seekg(0, ios::cur);
    drive.read((char*)&HeaderMFTEntry.Signature, sizeof(HeaderMFTEntry.Signature));
    drive.seekg(MFTEntryStart);
    drive.seekg(20, ios::cur);
    drive.read((char*)&HeaderMFTEntry.FirstAttrOffset, sizeof(HeaderMFTEntry.FirstAttrOffset));
    drive.read((char*)&HeaderMFTEntry.Flags, sizeof(HeaderMFTEntry.Flags));
    drive.read((char*)&HeaderMFTEntry.UsedSize, sizeof(HeaderMFTEntry.UsedSize));
    drive.read((char*)&HeaderMFTEntry.EntrySize, sizeof(HeaderMFTEntry.EntrySize));
    drive.seekg(MFTEntryStart);
    drive.seekg(44, ios::cur);
    drive.read((char*)&HeaderMFTEntry.ID, sizeof(HeaderMFTEntry.ID));
}
// void readMFT(ifstream &drive, MFT &MFT) {
//     while(drive.tellg() != )
// }