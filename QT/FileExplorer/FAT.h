#pragma once
#include<iostream>
#include <algorithm>
#include <cctype>
#include<Windows.h>
#include <fcntl.h>
#include <vector>
#include <windows.h>
#include <stdint.h>
#include<fstream>
#include<sstream>
#include<iomanip>
#include<map>
#include <locale>
#include <codecvt>
#include<fstream>
#include<fileapi.h>
#include <Winioctl.h>
#include <aclapi.h>
#include"NTFS.h"
using namespace std;
struct attribute {
    bool ReadOnly;
    bool Hidden;
    bool System;
    bool VolLabel;
    bool directory;
    bool archive;
};
struct TimeF {
    int hour;
    int minute;
    int second;
    int milisecond;
};
struct Day {
    int day;
    int month;
    int year;
};
class BootSector {
public:
    vector<uint8_t> data;
    int Size_Sector;//offset:OB,2b
    int NumberSector_Clusters;//offset:0D,1b
    int NumberSector_BFAT;//offset:0E,2b
    int Number_FAT;//offset:10,1b
    int Size_Volumes;//offset:20,4b
    int NumberSector_FAT;//offset:24,4b
    int BeginCluster_RDET;//offset:2C,4b
    string Type_FAT;//offset:52,8b
    void ReadBootSector();
    void updateData(vector<uint8_t>);
    void updateInf();
};
class Item
{
public:
    int offset;
    string name;
    int size;
    int startSector;
    attribute a;
    TimeF time;
    Day day;
    virtual ~Item() {}
    Item(int offset,string name, int size, int startSector, attribute a, TimeF time, Day day)
        :offset(offset),name(name), size(size), startSector(startSector), a(a), time(time), day(day) {}
};
class FileF : public Item
{

public:
    string data;
    FileF(int offset,string name, int size, int startSector, attribute a, TimeF time, Day day, string data)
        :Item(offset,name, size, startSector, a, time, day) {
        this->data = data;
    }
    int getSize() { return Item::size; }

};
class Folder : public Item
{
public:
    vector<Item*> items;
    Folder(int offset,string name, int size, int startSector, attribute a, TimeF time, Day day) :Item(offset,name, size, startSector, a, time, day) {}
    void addItem(Item* item);
    int getsize();
};
class FAT {
public:
    vector<uint8_t>data;
    map<int, int>Cluster;
    void ReadFAT(int index, int size);
};
class Entry {
public:
    int offsetBegin;
    string name;
    Day d;
    TimeF t;
    attribute a;
    int BeginCluster;
    int size;
};
class RDET {
public:
    vector<uint8_t>data;
    vector<Entry>subEntry;
    void Readdata(int offset);
};
struct DeletedItems
{
    string name;
    int offset;
    uint8_t data;
};
class Program
{
public:
    FAT f;
    BootSector bs;
    RDET rdet;
    Program(){}
    Program(FAT f, BootSector bs, RDET rdet) :f(f), bs(bs), rdet(rdet) {}
    Program(const Program& other)
        : f(other.f), bs(other.bs), rdet(other.rdet), m(other.m), List_DI(other.List_DI) {}
    vector<Item*>m;
    vector<int> GetNextCluster(int index);
    vector<uint8_t>ReadCluster(vector<int>v);
    vector<DeletedItems>List_DI;
    void updateFoldersize();
    void ReadItem(Folder* f,vector<Entry>entry, vector<int>check);
    void FindItem(vector<Item*>m,string name);
    void deleteItem(string name,int offset);
    void RecycleBin(string name);
    void RestoreItems(int offset,uint8_t data);
};
string uint8ToHex(uint8_t number);
int GetDec(int id, int size, vector<uint8_t>data);
string GetName(int id, int size, vector<uint8_t>data);
int HextoDec(string);
string DecToBinary(unsigned int decimal);
attribute ReadAttribute(string s);
TimeF GetTime(int dec);
Day GetDay(int dec);
string GetNameItem(map<int, vector<uint8_t>> data, int index);
string readData(vector<uint8_t>& data);
vector<Entry> ReadEntry(int offset,vector<uint8_t>d, vector<int>check);
string DaytoString(Day d);
string TimetoString(TimeF t, string d);
string  AttributetoString(attribute a);
long power(int base, int exponent);
void updateListFile(vector<Item*>m, vector<File>& Fi);
