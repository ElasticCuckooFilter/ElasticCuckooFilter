#ifndef __CUCKOO_H__
#define __CUCKOO_H__

typedef unsigned int(*hashFunction)(const char*str, unsigned int len);
typedef unsigned int uint;
typedef unsigned char uchar;

using namespace std;
#include <string>
#include <cstring>

class cuckoo
{
public:
    virtual bool insert(string key) = 0;
    virtual bool lookup(string key) = 0;
    virtual bool del(string key) = 0;
    virtual bool resize(int len) = 0;
    virtual int Get_Hop_Num() = 0;
    virtual~cuckoo(){};
};



#endif
