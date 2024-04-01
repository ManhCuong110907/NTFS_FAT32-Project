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
struct Time {
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
protected:
    string name;
    int size;
    int startSector;
    attribute a;
    Time time;
    Day day;
public:
    Item(string name, int size, int startSector, attribute a,Time time,Day day)
        :name(name),size(size),startSector(startSector),a(a),time(time),day(day) {}
    string getName() { return name; };
};
class File : public Item
{
private:
    string data;
public:
    File(string name, int size, int startSector,attribute a, Time time, Day day,string _data)
        :Item(name, size, startSector, a, time, day) {
        data = data;
    }
    int getSize() { return Item::size; }

};
class Folder : public Item
{
private:
    vector<Item*> items;
public:

    Folder(string name, int size, int startSector, attribute a, Time time, Day day) :Item(name,size,startSector,a,time,day) {}
    void addItem(Item* item);
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
    Time t;
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
    void ReadItem(Folder* f, vector<Entry>entry, vector<int>check);

};
vector<uint8_t> ReadBootSector();
string uint8ToHex(uint8_t number);
int GetDec(int id, int size, vector<uint8_t>data);
string GetName(int id, int size, vector<uint8_t>data);
int HextoDec(string);
string DecToBinary(unsigned int decimal);
attribute ReadAttribute(string s);
Time GetTime(int dec);
Day GetDay(int dec);
string GetNameItem(map<int, vector<uint8_t>> data,int index);
string readData(vector<uint8_t>& data);
vector<Entry> ReadEntry(vector<uint8_t>d,vector<int>check);
