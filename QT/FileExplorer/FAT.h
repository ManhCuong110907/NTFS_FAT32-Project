#pragma once
#include<iostream>
#include<Windows.h>
#include<fstream>
#include<sstream>
#include<iomanip>
#include <vector>
#include<map>
#include <locale>
#include <codecvt>
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
    string name;
    int size;
    int startSector;
    attribute a;
    TimeF time;
    Day day;
    virtual ~Item() {}
    Item(string name, int size, int startSector, attribute a,TimeF time,Day day)
        :name(name),size(size),startSector(startSector),a(a),time(time),day(day) {}
};
class FileF : public Item
{
private:
    string data;
public:
    FileF(string name, int size, int startSector,attribute a, TimeF time, Day day,string data)
        :Item(name, size, startSector, a, time, day) {
        this->data = data;
    }
    int getSize() { return Item::size; }

};
class Folder : public Item
{
public:
    vector<Item*> items;
    Folder(string name, int size, int startSector, attribute a, TimeF time, Day day) :Item(name,size,startSector,a,time,day) {}
    void addItem(Item* item);
    int getsize();
};
class FAT {
public:
    vector<uint8_t>data;
    map<int,int>Cluster;
    void ReadFAT(int index,int size);
};
class Entry {
public:
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
class Program
{
public:
    FAT f;
    BootSector bs;
    RDET rdet;
    Program(FAT f, BootSector bs, RDET rdet):f(f),bs(bs),rdet(rdet){};
    vector<Item*>m;
    vector<int> GetNextCluster(int index);
    vector<uint8_t>ReadCluster(vector<int>v);
    void updateFoldersize();
    void ReadItem(Folder* f, vector<Entry>entry, vector<int>check);

};
string uint8ToHex(uint8_t number);
int GetDec(int id, int size, vector<uint8_t>data);
string GetName(int id, int size, vector<uint8_t>data);
int HextoDec(string);
string DecToBinary(unsigned int decimal);
attribute ReadAttribute(string s);
TimeF GetTime(int dec);
Day GetDay(int dec);
string GetNameItem(map<int, vector<uint8_t>> data,int index);
string readData(vector<uint8_t>& data);
vector<Entry> ReadEntry(vector<uint8_t>d,vector<int>check);
void displayFAT(Folder* f, vector<Item*>m);
string DaytoString(Day d);
string TimetoString(TimeF t,string d);
string  AttributetoString(attribute a);
long power(int base, int exponent);
