#include "Hasher.h"
#include "NvraRecord.h"
#include "NvraHasher.h"
using namespace std;

unsigned long NvraHasher::hash(const NvraRecord& item) const
{
    unsigned long id = static_cast<unsigned long>(item.getNum(0));
    return id;
}
