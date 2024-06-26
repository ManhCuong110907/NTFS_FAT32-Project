﻿#include"Header.h"
string readData(vector<uint8_t>& data) {
    string result = "";
    int tmp = 0;
    while (data[tmp] != 0x00) {
        result += (char)data[tmp++];
    }
    return result;
}
attribute ReadAttribute(string s)
{
    attribute temp;
    temp.ReadOnly = s[5] - 48;
    temp.Hidden = s[4] - 48;
    temp.System = s[3] - 48;
    temp.VolLabel = s[2] - 48;
    temp.directory = s[1] - 48;
    temp.archive = s[0] - 48;
    return temp;
}
string uint8ToHex(uint8_t number) {
    std::stringstream stream;
    stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(number);
    return stream.str();
}
int binaryToDecimal(string binary) {
    int decimal = 0;
    int power = 0;
    for (int i = binary.length() - 1; i >= 0; --i) {
        if (binary[i] == '1') {
            decimal += pow(2, power);
        }
        ++power;
    }

    return decimal;
}
Time GetTime(int dec)
{
    Time t;
    string binary = "";
    while (dec > 0) {
        binary = std::to_string(dec % 2) + binary;
        dec /= 2;
    }
    while (binary.length() < 24) {
        binary = "0" + binary;
    }
    string s = binary;
    t.hour = binaryToDecimal(s.substr(0,5));
    t.minute = binaryToDecimal(s.substr(5, 6));
    t.second = binaryToDecimal(s.substr(11, 6));
    t.milisecond = binaryToDecimal(s.substr(17, 7));
    return t;
}
Day GetDay(int dec)
{
    Day d;
    string binary = "";
    while (dec > 0) {
        binary = std::to_string(dec % 2) + binary;
        dec /= 2;
    }
    while (binary.length() < 16) {
        binary = "0" + binary;
    }
    string s = binary;
    d.year = binaryToDecimal(s.substr(0, 7)) + 1980;
    d.month = binaryToDecimal(s.substr(7, 4));
    d.day = binaryToDecimal(s.substr(11,5));
    return d;
}
int HextoDec(string s)
{
    int k = 0;
    int result = 0;
    for (int i = s.size() - 1; i >= 0; i--)
    {
        if (s[i] <= 57)
            result += (s[i] - 48) * pow(16, k++);
        else
            result += (s[i] - 87) * pow(16, k++);
    }
    return result;
}
string DecToBinary(unsigned int decimal) {
    string binary = "";

    if (decimal == 0)
        return "00000000";

    while (decimal > 0) {
        binary = std::to_string(decimal % 2) + binary;
        decimal /= 2;
    }
    while (binary.length() < 6) {
        binary = "0" + binary;
    }

    return binary;
}

string GetName(int id, int size, vector<uint8_t>data)
{
    string tmp;
    for (int i = id; i <= id+size-1; i++)
        tmp += uint8ToHex(data[i]);
    string asciiString;
    for (size_t i = 0; i < tmp.length(); i += 2)
    {
        string byteString = tmp.substr(i, 2);
        int byte = stoi(byteString, nullptr, 16);
        asciiString += static_cast<char>(byte);
    }
    int check = 0;
    for (int i = asciiString.size() - 1; i >= 0; i--)
    {
        if (static_cast<int>(asciiString[i]) < 0)
            check++;
    }
    return asciiString.substr(0, asciiString.size()-check);

}
string GetNameItem(map<int, vector<uint8_t>> data,int index)
{
    string temp = "";
    if (index != 0 && uint8ToHex(data[index - 1][0x0B]) == "0f")
    {
        index--;
        while (uint8ToHex(data[index][0x0B]) == "0f")
        {
            temp += GetName(0x01, 10, data[index]) + GetName(0x0E, 12, data[index]) + GetName(0x1C, 4, data[index]) + " ";
            index--;
        }
    }
    else
        temp += GetName(0x00, 8, data[index]) + GetName(0x08, 3, data[index]);
    temp.erase(remove(temp.begin(), temp.end(), ' '), temp.end());
    return temp;

}

int GetDec(int id, int size,vector<uint8_t>data)
{
    string tmp;
    for (int i = id + size - 1; i >= id; i--)
        tmp += uint8ToHex(data[i]);
    return HextoDec(tmp);
}
vector<uint8_t> ReadBootSector()
{
    const char* drive = "\\\\.\\F:";
    HANDLE hDrive = CreateFileA(drive, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    if (hDrive == INVALID_HANDLE_VALUE) {
        cout << "Can't open disk" << std::endl;
    }
    vector<uint8_t> temp(512);
    DWORD bytesRead;
    if (!ReadFile(hDrive, temp.data(), 512, &bytesRead, NULL)) {
        std::cerr << "Can't read bootsector" << std::endl;
        CloseHandle(hDrive);
    }
    CloseHandle(hDrive);
    return temp;
}
void BootSector::updateData(vector<uint8_t> data)
{
    this->data = data;
}
void BootSector::updateInf()
{
    Size_Sector = GetDec(0x0B, 2, data);
    NumberSector_Clusters = GetDec(0x0D, 1, data);
    NumberSector_BFAT = GetDec(0x0E, 2, data);
    Number_FAT = GetDec(0x10,1, data);
    Size_Volumes = GetDec(0x20,4, data);
    NumberSector_FAT = GetDec(0x24,4, data);
    BeginCluster_RDET = GetDec(0x2C,4, data);
    Type_FAT = GetName(0x52, 8, data);
    /*cout << Size_Sector;
    cout << endl << NumberSector_Clusters;
    cout << endl << NumberSector_BFAT;
    cout << endl << Number_FAT;
    cout << endl << Size_Volumes;
    cout << endl << NumberSector_FAT;
    cout << endl << BeginCluster_RDET;
    cout << endl << Type_FAT;*/
}
void FAT::ReadFAT(int index, int size)
{
    const char* drive = "\\\\.\\F:";
    HANDLE hDrive = CreateFileA(drive, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    if (hDrive == INVALID_HANDLE_VALUE) {
        cout << "Can't open disk" << std::endl;
    }
    vector<uint8_t> temp(size);
    DWORD bytesRead;
    uint64_t sectorOffset = index;
    // Seek to the beginning of the sector
    LARGE_INTEGER liOffset;
    liOffset.QuadPart = sectorOffset;
    if (!SetFilePointerEx(hDrive, liOffset, NULL, FILE_BEGIN)) {
        cerr << "Failed to seek to sector" << endl;
        exit(1);
    }
    if (!ReadFile(hDrive, temp.data(),size, &bytesRead, NULL)) {
        std::cerr << "Can't read bootsector" << std::endl;
        CloseHandle(hDrive);
    }
    CloseHandle(hDrive);
    data = temp;
    int k = 0;
    for (int i = 0;; i += 4)
    {
        vector<uint8_t> t;
        for (int j = i; j < i + 4; j++)
            t.push_back(data[j]);
        if (GetDec(0, 4, t)==0)
            break;
        Cluster[k] = GetDec(0, 4, t);
        k++;
    }
    cout << endl;
}
vector<int> Program::GetNextCluster(int index)
{
    vector<int>v;
    v.push_back(index);
    int tmp = f.Cluster[index];
    while (tmp < f.Cluster.size())
    {
        v.push_back(tmp);
        tmp = f.Cluster[tmp];
    }
    return v;
}
vector<Entry> ReadEntry(vector<uint8_t>d,vector<int> check)
{
    vector<Entry>entry;
    map<int, vector<uint8_t>> data;
    string ck = "00";
    int cnt = 0;
    for (int i = 0;; i += 32)
    {
        if (uint8ToHex(d[i]) == ck)
            break;
        if (uint8ToHex(d[i]) != "e5")
        {

            for (int j = i; j < i + 32; j++)
            {

                data[cnt].push_back(d[j]);
            }
            cnt++;
        }
    }
    for (auto pair : data)
    {
        if (uint8ToHex(pair.second[0x0B]) != "0f")
        {
            Entry e;
            e.a = ReadAttribute(DecToBinary(GetDec(0x0B, 1, pair.second)));
            e.BeginCluster = GetDec(0x1A, 2, pair.second);
            e.t = GetTime(GetDec(0x0D, 3, pair.second));
            e.d = GetDay(GetDec(0x10, 2, pair.second));
            e.size = GetDec(0x1C, 4, pair.second);
            e.name = GetNameItem(data, pair.first);
            int ck = 0;
            for (auto x : check)
            {
                if (e.BeginCluster == x)
                    ck += 1;
            }
            if(e.BeginCluster!=0 && ck==0)
                entry.push_back(e);
        }
    }
    return entry;
}
void RDET::Readdata(int offset)
{
    vector<Entry>entry;
    const char* drive = "\\\\.\\F:";
    HANDLE hDrive = CreateFileA(drive, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    if (hDrive == INVALID_HANDLE_VALUE) {
        cout << "Can't open disk" << std::endl;
    }
    vector<uint8_t> temp(1024);
    DWORD bytesRead;
    uint64_t sectorOffset = offset;
    // Seek to the beginning of the sector
    LARGE_INTEGER liOffset;
    liOffset.QuadPart = sectorOffset;
    if (!SetFilePointerEx(hDrive, liOffset, NULL, FILE_BEGIN)) {
        cerr << "Failed to seek to sector" << endl;
        exit(1);
    }
    if (!ReadFile(hDrive, temp.data(),1024, &bytesRead, NULL)) {
        std::cerr << "Can't read bootsector" << std::endl;
        CloseHandle(hDrive);
    }
    CloseHandle(hDrive);
    data = temp;
    vector<int>v;
    subEntry = ReadEntry(data,v);
}
vector<uint8_t>Program::ReadCluster(vector<int>v)
{
    const char* drive = "\\\\.\\F:";
    HANDLE hDrive = CreateFileA(drive, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    if (hDrive == INVALID_HANDLE_VALUE) {
        cout << "Can't open disk" << std::endl;
    }
    vector<uint8_t>result;
    DWORD bytesRead;
    for (auto x : v)
    {
        int size = bs.NumberSector_Clusters * bs.Size_Sector;
        vector<uint8_t> temp(size);
        uint64_t sectorOffset = (bs.NumberSector_BFAT + bs.NumberSector_FAT * bs.Number_FAT + (x-
            bs.BeginCluster_RDET) * bs.NumberSector_Clusters) * bs.Size_Sector;
        // Seek to the beginning of the sector
        LARGE_INTEGER liOffset;
        liOffset.QuadPart = sectorOffset;
        if (!SetFilePointerEx(hDrive, liOffset, NULL, FILE_BEGIN)) {
            cerr << "Failed to seek to sector" << endl;
            exit(1);
        }
        if (!ReadFile(hDrive, temp.data(),size, &bytesRead, NULL)) {
            std::cerr << "Can't read bootsector" << std::endl;
            CloseHandle(hDrive);
        }
        result.insert(result.end(), temp.begin(), temp.end());
    }
    return result;
}
void Program::ReadItem(Folder* f,vector<Entry>entry,vector<int>check)
{
    for (auto e : entry)
    {
        if (e.a.directory == 1 && e.a.System == 0 || e.a.archive == 1)
        {
            vector<int>v;
            v = GetNextCluster(e.BeginCluster);
            check.insert(check.end(), v.begin(), v.end());
            if (e.a.directory == 1)
            {
                vector<uint8_t>cc = ReadCluster(v);
                vector<Entry>sdet = ReadEntry(cc,check);
                cout << endl << e.name << ":" << e.BeginCluster << endl;
                Folder* folder = new  Folder(e.name,e.size,e.BeginCluster,e.a,e.t,e.d);
                if (f == NULL)
                    m.push_back(folder);
                else
                    f->addItem(folder);
                ReadItem(folder, sdet,check);
            }
            if (e.a.archive == 1)
            {
                vector<uint8_t>cc = ReadCluster(v);
                string s = readData(cc);
                cout << endl << e.name << ":" << e.BeginCluster << endl;
                File* file = new File(e.name,e.size,e.BeginCluster,e.a,e.t,e.d,s);
                if (f == NULL)
                    m.push_back(file);
                else
                    f->addItem(file);
            }

        }
   }
}
void Folder::addItem(Item* item)
{
    items.push_back(item);
}

