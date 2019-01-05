#include "Comparator.h"
#include "NvraRecord.h"
#include "NvraComparator.h"
#include "iostream"

using namespace std;

NvraComparator::NvraComparator(unsigned int column)
{
  this->column = column;
}

int NvraComparator::compare(const NvraRecord& item1, const NvraRecord& item2) const
{
  bool str = false;
  int val = 42;
  if(column < 3)                                                                //ensuring correct column comparison
  {
    val = column;
  }
  else if(column == 3)
  {
    val = 0;
    str = true;
  }
  else if(column > 3 && column < 11)
  {
    val = column - 1;
  }
  else if(column == 11)
  {
    val = 1;
    str = true;
  }
  else if(column == 12)
  {
    val = 2;
    str = true;
  }
  else if(column > 12)
  {
    val = column - 3;
  }

  if (!str)                                                                     //comparing two int values
  {
    int one = item1.getNum(val);
    int two = item2.getNum(val);
    if (one < two)
    {
      return -1;
    }
    else if (one == two)
    {
      return 0;
    }
    else if (one > two)
    {
      return 1;
    }
  }
  else                                                                          //comparing two string values
  {
    string one = item1.getString(val);
    string two = item2.getString(val);
    if (one < two)
    {
      return -1;
    }
    else if (one == two)
    {
      return 0;
    }
    else if (one > two)
    {
      return 1;
    }
  }
  return 2;                                                                     //in case something goes terribly wrong.
}
