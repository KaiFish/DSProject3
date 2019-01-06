# DSProject3
## To Build:
### Build with clang:
```
clang++ NvraComparator.cpp NvraHasher.cpp NvraRecord.cpp VoteR3.cpp IntComparator.cpp -o a.out
```
### Build with g++:
```
g++ NvraComparator.cpp NvraHasher.cpp NvraRecord.cpp VoteR3.cpp IntComparator.cpp -o a.out
```

## Runtime instructions:
The program will prompt for data file, included here is el.csv. It then reads the file and goes to the menu, and option h will go into the hash table function.
It will look like this:
```
Enter data file name: el.csv
Enter (o)utput, (s)ort, (f)ind, (m)erge, (p)urge, (r)ecords, (h)ash table, or (q)uit: h
```
