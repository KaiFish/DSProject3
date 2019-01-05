#include <iostream>
using namespace std;

#include "NvraRecord.h"

ostream& operator<<(ostream& os, const NvraRecord& record)
{
  int i = 0;                                                                    //counter
  int num = 0;                                                                  //tracks numeric index
  int str = 0;                                                                  //tracks string index

  while(i < 23)                                                                 //24 columns, but last column is special case
  {
    if (i != 3 && i != 11 && i != 12)                                           //these columns have text in them
    {
      os << record.nums[num] << ';';                                            //prints numeric data
      num++;                                                                    //increment num index
    }
    else
    {
      os << record.strings[str] << ';';                                         //add string data to rec
      str++;                                                                    //increment str index
    }
    i++;
  }
  os << record.nums[num];                                                       //last item is not followed by ';' in example output
  return os;
}

void NvraRecord::addNum(unsigned int num)                                       // puts num in array, increments counter
{
  nums[numCtr] = num;
  numCtr++;
}

void NvraRecord::addString(string string)                                       // puts string in array, increments counter
{
  strings[strCtr] = string;
  strCtr++;
}

unsigned int NvraRecord::getNum(unsigned int index) const                       // returns (copy of) num at index in array
{
  return nums[index];
}

string NvraRecord::getString(unsigned int index)  const                         // returns (copy of) string at index in array
{
  return strings[index];
}

void NvraRecord::setNum(unsigned int num, unsigned int index)                   // sets nums[index] to num, doesn't change counter
{
  nums[index] = num;
}

void NvraRecord::setString(string string, unsigned int index)                   // sets strings[index] to string, doesn't change counter
{
  strings[index] = string;
}
