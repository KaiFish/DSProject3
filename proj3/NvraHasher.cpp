#include "Hasher.h"
#include "NvraRecord.h"
#include "NvraHasher.h"
#include <iostream>
using namespace std;
    int i = 0;

unsigned long NvraHasher::hash(const NvraRecord& item) const
{
    unsigned long id = (unsigned long)item.getNum(0);
    i++;
    return id;
}
