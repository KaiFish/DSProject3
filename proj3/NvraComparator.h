// Must be used as is.

#ifndef NVRA_COMPARATOR_H
#define NVRA_COMPARATOR_H

#include "Comparator.h"
#include "NvraRecord.h"

class NvraComparator : public Comparator<NvraRecord> {
private:
    unsigned int column = 0;
public:
    NvraComparator(unsigned int column);

    // returns -1 is item1 < item2, 0 if item1 == item2, +1 if item1 > item2
    int compare(const NvraRecord& item1, const NvraRecord& item2) const;
};

// You must create a corresponding NvraComparator.cpp file to implement compare().

#endif
