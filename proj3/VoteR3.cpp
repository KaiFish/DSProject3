#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

#include "NvraHasher.h"
#include "HashTableEnumerator.h"
#include "HashTable.h"
#include "Hasher.h"
#include "NvraRecord.h"
#include "TemplatedArray.h"
#include "Comparator.h"
#include "Exceptions.h"
#include "NvraComparator.h"
#include "Search.h"
#include "Sorter.h"
#include "Enumerator.h"
#include "OULink.h"
#include "OULinkedList.h"
#include "OULinkedListEnumerator.h"

int rowCount = 0;                                                               //counts all rows
int valid = 0;                                                                  //only counts valid rows
vector<int> IDs(0);
NvraComparator* c = new NvraComparator(0);
OULinkedList<NvraRecord>* list = new OULinkedList<NvraRecord>(c);
Sorter<NvraRecord>* s = new Sorter<NvraRecord>();
TemplatedArray<NvraRecord>* arr = new TemplatedArray<NvraRecord>();
bool isSorted = true;                                                          //flag for sort status
int sortField = 0;


void loadData();
void menu();
void outputData();
void sortData();
void findData();
void purgeData();
void records();
void hashTable();

NvraRecord* getItems(string& row);                                              //delimits string row by ',' and puts results in PrintVector
bool isDuplicate(vector<int>& fileIDs, int item);                               //checks for duplicate IDs, return false if duplicate
bool isValid(NvraRecord& rec);                                                  //checks vector for values less than 0, returns false if invalid
bool isInvalidData(NvraRecord& rec, int& line, vector<int>& fileIDs);           //checks for any invalid data, prints when found
void transfer(string fileName, fstream& inF);                                   //retrieves records from files
void list2array();
int extract();
int convert(int num);                                                           //converts standard column to index needed for NvraRecord
NvraRecord* fakeRec(int col, string search);                                    //converts search query into a whole array for comparator
NvraRecord* fakeRec(int col, int search);                                       //same as above but int.
long long linear(TemplatedArray<NvraRecord>* a, NvraRecord* rec, NvraComparator& c, int low, int high);        //linear search
string stats(int size);


int main()
{
    loadData();
    if(list->getSize() == 0)
    {
        return 42;
    }
    menu();
    return 0;
}

void loadData()
{
    fstream voteFile;
    string fname;
    bool done = false;
    while(!done)
    {
        cout << "Enter data file name: ";
        getline(cin, fname);
        if(fname == "")                                                             //indicates done with data entry loop
        {
            done = true;
        }
        else
        {
            voteFile.open(fname);                                                     //open file specified
        }
        
        if(!done)                                                                   //if not already done at this point in the loop
        {
            if(!voteFile.is_open())                                                   //can't read something that doesn't exist
            {
                cout << "File is not available." << endl;
            }
            else
            {
                transfer(fname, voteFile);                                              //actual place where data is read
                done = true;
            }
        }
    }
    list2array();
    return;
}

void menu()
{
    string choice;
    cout << "Enter (o)utput, (s)ort, (f)ind, (m)erge, (p)urge, (r)ecords, (h)ash table, or (q)uit: ";
    getline(cin, choice);
    if(choice == "o" || choice == "output")
    {
        outputData();
    }
    else if(choice == "s" || choice == "sort")
    {
        sortData();
    }
    else if(choice == "f" || choice == "find")
    {
        findData();
    }
    else if(choice == "m" || choice == "merge")
    {
        loadData();
        menu();
    }
    else if(choice == "p" || choice == "purge")
    {
        purgeData();
    }
    else if(choice == "r" || choice == "records")
    {
        records();
    }
    else if(choice == "h" || choice == "hash table")
    {
        hashTable();
    }
    else if(choice == "q" || choice == "quit")
    {
        cout << endl << "Thanks for using VoteR." << endl;
        return;
    }
    else
    {
        cout << "something went wrong" << endl;
    }
}

void outputData()
{
    string fname;
    cout << "Enter output file name: ";
    getline(cin, fname);
    int memSize = static_cast<int>(arr->getSize());
    if(fname == "")
    {
        cout << *arr << stats(memSize) << endl;
    }
    else
    {
        ofstream outFile;
        outFile.open(fname);
        outFile << *arr << stats(memSize) << endl;
        outFile.close();
    }
    menu();
}

void sortData()
{
    list2array();
    cout << "Enter sort field (0-23): " << endl;
    int sortValue = extract();
    if(sortValue < 0 || sortValue > 23)
    {
        menu();                                                                    //return to data manipulation loop if invalid
    }
    NvraComparator b = NvraComparator(sortValue);
    s->sort(*arr, b);                                                               //go to Sorter.h to see the magic
    isSorted = true;
    sortField = sortValue;
    menu();
}

void findData()
{
    cout << "Enter search field (0-23): " << endl;
    int searchField = extract();
    if(searchField < 0 || searchField > 23)
    {
        menu();                                                                    //return to data manipulation loop if invalid
    }
    
    TemplatedArray<NvraRecord>* found = new TemplatedArray<NvraRecord>();         //holds matches found
    NvraComparator* c = new NvraComparator(searchField);
    NvraRecord* rec = new NvraRecord();
    
    if (searchField != 3 && searchField != 11 && searchField != 12)               //if it is an int value
    {
        cout << "Enter non-negative field value: " << endl;
        int val = extract();
        rec = fakeRec(searchField, val);
    }
    else                                                                          //if it is a string value
    {
        string str;
        cout << "Enter exact text on which to search: " << endl;
        getline(cin, str);
        rec = fakeRec(searchField, str);
    }
    long long index;
    bool binary = false;
    int size = static_cast<int>(arr->getSize())-1;
    if(isSorted && searchField == sortField)                                      //if already sorted the way we need it, use binary
    {
        index = binarySearch(*rec, *arr, *c);
        binary = true;
        
    }
    else                                                                          //otherwise use linear search
    {
        index = linear(arr, rec, *c, 0, size);
    }
    
    if(index == -1)                                                               //if nothing found
    {
        cout << "NVRA records found: 0." << endl;
    }
    else                                                                          //make sure there are no extra matching records
    {
        NvraRecord ord;
        if(binary)
        {
            while(index!=-1)                                                          //if sorted, matches are all next to each other
            {
                ord = arr->get(static_cast<unsigned long>(index));
                found->add(&ord);
                index = linear(arr, rec, *c, static_cast<int>(index+1), size);
            }
        }
        else                                                                        //if not sorted, matches could be anywhere
        {
            while(index<=size && index != -1)
            {
                ord = arr->get(static_cast<unsigned long>(index));
                found->add(&ord);
                index = linear(arr, rec, *c, static_cast<int>(index+1), size);
            }
        }
        int i = 0;
        int x = static_cast<int>(found->getSize());
        while(i < x)                                                                //printing lines in regular order
        {
            ord = found->get(i);
            cout << ord << endl;                                                      //print the current record
            i++;                                                                      //step forward for next record
        }
        cout << "NVRA records found: " << x << "." << endl;
    }
    menu();
}

void purgeData()
{
    fstream voteFile;
    string fname;
    bool done = false;
    while(!done)
    {
        cout << "Enter data file name: ";
        getline(cin, fname);
        if(fname == "")
        {
            done = true;
        }
        else
        {
            voteFile.open(fname);
        }
        if(!done)
        {
            if(!voteFile.is_open())
            {
                cout << "File is not available." << endl;
            }
            else
            {
                string row;
                NvraRecord* rec;
                OULinkedListEnumerator<NvraRecord> e = list->enumerator();
                getline(voteFile, row);
                while(getline(voteFile, row))
                {
                    rec = getItems(row);
                    rowCount++;
                    if(isValid(*rec))
                    {
                        list->remove(rec);
                        valid++;
                    }
                }
                done = true;
            }
        }
    }
    list2array();
    menu();
}

void records()
{
    string fname;
    cout << "Enter output file name: ";
    getline(cin, fname);
    int size = static_cast<int>(list->getSize());
    if (fname == "")
    {
        cout << *list << stats(size) << endl;
    }
    else
    {
        ofstream outFile;
        outFile.open(fname);
        outFile << *list << stats(size) << endl;
        outFile.close();
    }
    menu();
}

void hashTable()
{
    NvraHasher h = NvraHasher();
    unsigned long size = arr->getSize();
    unsigned long i = 0;
    HashTable<NvraRecord>* hashT = new HashTable<NvraRecord>(c, &h, size);
    
    while(i<size)
    {
        NvraRecord item = arr->get(i);
        NvraRecord *ptr = &item;
        
        cout << item << endl;
        cout << ptr << endl;
        cout << *ptr << endl;
        
        hashT->insert(ptr);
        i++;
    }
//    while(i<size)
//    {
//        NvraRecord *item = new NvraRecord(arr->get(i));
//
//        cout << item << endl;
//        cout << *item << endl;
//
//        hashT->insert(item);
//        i++;
//    }
    int hashSize = static_cast<int>(hashT->getSize());
    string fname;
    cout << "Enter output file name: ";
    getline(cin, fname);
    if (fname == "")
    {
        cout << *hashT << endl << stats(hashSize) << endl;
    }
    else
    {
        ofstream outFile;
        outFile.open(fname);
        outFile << *hashT << endl << stats(hashSize) << endl;
        outFile.close();
    }
    menu();
}

NvraRecord* getItems(string &row)
{
    string token;                         //token is the delimited item
    NvraRecord* rec = new NvraRecord();     //this nvraRecord will hold all the tokens
    
    row.push_back(',');                   //with the ending ',' getline will still read the last column with the ',' delimiter
    istringstream buf(row);               //turns string into istringstream so that it can be delimited
    
    int i = 0;                            //keep track of iterations
    while (i<24)                          //data file has 24 columns, so 24 can be hardcoded
    {
        getline(buf, token, ',');           //splits each column at ','
        if (i != 3 && i != 11 && i != 12)   //these columns have text in them, don't try to make them ints
        {
            rec->addNum(stoi(token));         //add numeric data to rec
        }
        else
        {
            rec->addString(token);            //add string data to rec
        }
        i++;                               //increments counter
    }
    return rec;
}

bool isDuplicate(vector<int>& vec, int item)
{
    if(vec.empty())                             //size of empty vector != 0
    {
        return false;                             //if nothing is there, it can't be the same
    }
    else
    {
        int size = static_cast<int>(vec.size());
        int i=0;
        while(i<size)
        {
            if(vec.at(i)==item)                     //searching for duplicates
            {
                return true;                          //return true if found
            }
            else
            {
                i++;                                  //keep looking if not found
            }
        }
        return false;                             //return false if not found
    }
    return false;                               //return outside the loop/if statements for good practice
}

bool isValid(NvraRecord& rec)
{
    int i=0;                                    //index counter
    while (i<21)                                //only 21 numeric elements
    {
        if(static_cast<int>(rec.getNum(i)) < 0)  //false if less than 0
        {
            return false;
        }
        i++;                                     //compare all values
    }
    return true;                               //if not failed, return true
}

bool isInvalidData(NvraRecord& rec, int& line, vector<int>& fileIDs)
{
    int recID = rec.getNum(0);
    if(isDuplicate(fileIDs, recID))
    {
        cout << "Duplicate record ID " << recID << " at line " << line << "." << endl;
        line++;                                   //line count increases regardless of validity
        valid++;                                  //valid lines increase with duplicates
        return true;
    }
    else
    {
        if(isValid(rec))
        {
            fileIDs.push_back(recID);                //valid IDs are added to the IDs vector
            line++;
            valid++;                                 //valid lines increase only in this case
            return false;
        }
        else
        {
            cout << "Invalid data at line " << line << "." << endl;
            line++;
            return true;
        }
    }
    return false;                               //to make the compiler happy
}

void transfer(string fileName, fstream& inF)
{
    if(!inF.is_open())
    {
        inF.open(fileName);
    }
    
    vector<int> fileIDs(0);                                                       //vector that keeps track of record IDs in this file only
    string row;                                                                   //inF reads one line to row
    NvraRecord* rec;                                                              //nvraRecord that holds divided column data
    getline(inF, row);                                                            //first row doesn't count
    int line = 1;
    while(getline(inF, row))                                                      //each loop gets next line
    {
        rec = getItems(row);                                                        //row is divided into column data in rec
        rowCount++;
        if (!isInvalidData(*rec, line, fileIDs))                                    //checks if data is invalid, returns false if valid
        {
            int recID = rec->getNum(0);
            if(isDuplicate(IDs, recID))
            {
                list->replace(rec);                                                  //only replaces duplicates from different files
            }
            else
            {
                list->insert(rec);                                               //adds the current record to the recordArray data
                IDs.push_back(recID);                                                   //IDs added to global list
            }
        }
    }
    inF.close();                                                                  //closing things is important
    return;
}

void list2array()
{
    int size = static_cast<int>(list->getSize());
    
    TemplatedArray<NvraRecord>* new_arr = new TemplatedArray<NvraRecord>(size);
    if (size==0)
    {
        arr = new_arr;
        return;
    }
    
    OULinkedListEnumerator<NvraRecord> en = list->enumerator();
    while(en.hasNext())
    {
        NvraRecord rec = *en.currentData();
        new_arr->add(&rec);
        en.next();
    }
    NvraRecord rec = *en.currentData();
    new_arr->add(&rec);
    
    
    arr = new_arr;
}

int extract()                                                                   //this code is also used in NvraComparator
{
    int num;
    string input;
    getline(cin, input);
    try
    {
        num = stoi(input);                                                          //turn input into int if possible
    }
    catch (invalid_argument &excpt)
    {
        menu();                                                                    //return to data manipulation loop if invalid
    }
    if(num<0)
    {
        num = num * -1;                                                             //only return positive numbers
    }
    return num;
}

int convert(int num)                                                  //adjusts 0-23 column number to number needed to get right num or string index
{
    int out = 0;
    if(num < 3)
    {
        out = num;
    }
    else if(num == 3)
    {
        out = 0;
    }
    else if(num > 3 && num < 11)
    {
        out = num - 1;
    }
    else if(num == 11)
    {
        out = 1;
    }
    else if(num == 12)
    {
        out = 2;
    }
    else if(num > 12)
    {
        out = num - 3;
    }
    return out;
}

NvraRecord* fakeRec(int col, string search)                                     //can only compare full records, so make a fake one
{
    string empty = "0,0,0, ,0,0,0,0,0,0,0, , ,0,0,0,0,0,0,0,0,0,0,0";
    NvraRecord* rec = getItems(empty);
    
    int index = convert(col);
    rec->setString(search, index);
    return rec;
}

NvraRecord* fakeRec(int col, int search)                                        //can only compare full records, so make a fake one
{
    string empty = "0,0,0, ,0,0,0,0,0,0,0, , ,0,0,0,0,0,0,0,0,0,0,0";
    NvraRecord* rec = getItems(empty);
    
    int index = convert(col);
    rec->setNum(search, index);
    return rec;
}

long long linear(TemplatedArray<NvraRecord>* a, NvraRecord* rec, NvraComparator& c, int low, int high)         //it's a linear search
{
    long long i;
    i = static_cast<long long>(low);
    NvraRecord ord;
    while(i <=high)
    {
        ord = a->get(i);
        if(c.compare(*rec, ord) == 0)
        {
            return i;
        }
        else
        {
            i++;
        }
    }
    return -1;
}

string stats(int size)
{
    string str = "Data lines read: " + to_string(rowCount) + "; Valid NVRA records read: " + to_string(valid) + "; NVRA records in memory: " + to_string(size);
    return str;
}
