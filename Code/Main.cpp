#include"Main.h"
int main()
{
    ifstream drive("D", ios::binary);
    if(!drive.is_open())
    {
        cout << "Can't open drive" << endl;
        return 0;
    }
    VBR VBR;
    readVBR(drive, VBR);
    cout << "Bytes per sector: " << VBR.BytesPerSector << endl;
    cout << "Sectors per cluster: " << (int)VBR.SectorsPerCluster << endl;
    cout << "Sectors per track: " << VBR.SectorsPerTrack << endl;
    cout << "Number of heads: " << VBR.NumberOfHeads << endl;
    cout << "Total sectors: " << VBR.TotalSectors << endl;
    cout << "MFT cluster: " << VBR.MFTCluster << endl;
    cout << "MFT mirror cluster: " << VBR.MFTMirrCluster << endl;
    cout << "Bytes per entry: " << (int)VBR.BytesPerEntry << endl;
}