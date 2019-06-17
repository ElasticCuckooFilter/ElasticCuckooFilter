#ifndef __DYNAMIC_CUCKOO_H__
#define __DYNAMIC_CUCKOO_H__

#include "hash/hash_function.h"
#include "cuckoo.h"

using namespace std;
#include <memory.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <set>

class dynamicCuckoo: public cuckoo
{
public:
    class LinkedCF
    {
    public:
        //static int MAXKICK;
        int nbucket;
        int bucketsize;
        int hLen;
        int count;
        hashFunction hFP;
        hashFunction hc;
        LinkedCF * nextCF;
        uint *buf;
        bool *valid;

        int rnum;
        LinkedCF(int _nbucket, int _bucketsize, hashFunction _hFP, hashFunction _hc, LinkedCF * _nextCF = NULL) 
        :nbucket(_nbucket)
        ,bucketsize(_bucketsize)
        ,hFP(_hFP)
        ,hc(_hc)
        ,nextCF(_nextCF) 
        {
            int tSize = _nbucket * _bucketsize;
            buf = new uint[tSize];
            memset(buf, 0, tSize * sizeof(uint));
            valid = new bool[tSize];
            memset(valid, 0, tSize * sizeof(bool));
            hLen=1;
            while(hLen<=nbucket)
                hLen*=2;
            hLen/=2;
            count = 0;
            rnum = 0;
        }
        virtual ~LinkedCF()
        {
            delete[] buf;
            delete[] valid;
        }
        inline int getIndex(int pos, int slot)
        {
            return pos*bucketsize + slot;
        }
    public:
        void getPosByKey(string key, uint&fp, int&p1, int&p2) 
        {
            fp = hFP(key.c_str(), KEYSIZE) & 0x00000fff;
            uint hs = hc(key.c_str(), KEYSIZE);
            p1 = hs & (hLen-1);
            p2 = (hs ^ hc((char*)&fp, KEYSIZE)) & (hLen-1);
        }
        int getAnotherPos(int pos, uint fp) 
        {
            return (pos ^ hc((char*)&fp, KEYSIZE)) & (hLen-1);
        }

        bool inquiry(string key) 
        {
            uint fp;
            int p1, p2;
            getPosByKey(key, fp, p1, p2);

            //find
            int bufPos1=getIndex(p1, 0);
            int bufPos2=getIndex(p2, 0);
            rnum++;
            for (int i = 0; i<bucketsize; i++) 
            {
                if(valid[bufPos1+i] && buf[bufPos1+i] == fp)
                    return true;
            }
            rnum++;
            for (int i = 0; i<bucketsize; i++) {
                if(valid[bufPos2+i] && buf[bufPos2+i] == fp)
                    return true;
            }
            return false;
        }
        bool del(string key)
        {
            uint fp;
            int p1, p2;
            getPosByKey(key, fp, p1, p2);

            //find
            int bufPos1=getIndex(p1, 0);
            int bufPos2=getIndex(p2, 0);
            for (int i = 0; i<bucketsize; i++) 
            {
                if(valid[bufPos1+i] && buf[bufPos1+i] == fp)
                {
                    valid[bufPos1+i]=false;
                    count--;
                    return true;
                }
                if(valid[bufPos2+i] && buf[bufPos2+i]==fp) 
                {
                    valid[bufPos2+i]=false;
                    count--;
                    return true;
                }
            }
            return false;
        }

        bool alternative(int p, int t, int &alter_pos, int &alter_slot){
            uint fp = buf[getIndex(p,t)];
            //memory_access_num++;
            int y = getAnotherPos(p, fp);
            int bufPosY = getIndex(y, 0);
            for(int i = 0; i < bucketsize; ++i)
                if(!valid[bufPosY+i]){
                    if(!i);
                        //memory_access_num++;
                    alter_slot = i;
                    alter_pos = y;
                    return true;
                }
                else
                    if(!i);
                        //memory_access_num++;
            return false;
        }

        bool insert(string key, uint & victimfp, int & victimpos) 
        {
            uint fp;
            int p1, p2;
            getPosByKey(key, fp, p1, p2);
            return rand()%2==0 ? insertWithoutKey(fp, p1, p2, victimfp, victimpos)
                                    :insertWithoutKey(fp, p2, p1, victimfp, victimpos);
        }

        bool insertWithoutKey(uint fp, int p1, int p2, uint & victimfp, int & victimpos)
        {
            int bufPos1=getIndex(p1, 0);
            int bufPos2=getIndex(p2, 0);
            //find empty slot
            for (int i = 0; i<bucketsize; i++)
            {
                if(!valid[bufPos1+i])
                {
                    buf[bufPos1+i]=fp;
                    valid[bufPos1+i]=true;
                    count++;
                    return true;
                }
                if(!valid[bufPos2+i])
                {
                    buf[bufPos2+i]=fp;
                    valid[bufPos2+i]=true;
                    count++;
                    return true;
                }
            }

#if 0
            //random kick with one hop check in first kick
            int alter_pos = 0;
            int alter_slot = 0;
            for(int i = 0; i < bucketsize; ++i){
                if(alternative(p1, i, alter_pos, alter_slot)){ 
                    uint _fp = buf[bufPos1+i];
                    int bufPos=getIndex(alter_pos,alter_slot);
                    buf[bufPos] = _fp;
                    valid[bufPos] = true; 
                    buf[bufPos1+i] = fp;
                    valid[bufPos1+i] = true;
                    //memory_access_num += 5;
                    //hop_num++;
                    return true; 
                }
                if(alternative(p2, i, alter_pos, alter_slot)){
                    uint _fp = buf[bufPos2+i];
                    int bufPos=getIndex(alter_pos,alter_slot);
                    buf[bufPos] = _fp;
                    valid[bufPos] = true;
                    buf[bufPos2+i] = fp;
                    valid[bufPos2+i] = true; 
                    //memory_access_num += 5;
                    //hop_num++;
                    return true;
                }
            }
#endif

            //kick
            int kickP = rand() % 2 == 0 ? p1 : p2;
            uint kickFp = fp;
            for (int k = 0; k<MAXKICK; k++) 
            {
#if 0
                //random kick with one hop check
                int kickSlot = rand() % bucketsize;
                int tmpKickP;

                int offset=rand()%bucketsize;
                for (int i = 0; i<bucketsize; i++, offset++) 
                {
                    if (offset>=bucketsize)offset=0;
                    int pos=getAnotherPos(kickP, buf[getIndex(kickP,offset)]);
                    int bufPos=getIndex(pos, 0);
                    if (kickSlot == offset)
                        tmpKickP = pos;
                    for (int j = 0; j<bucketsize; j++) 
                    {
                        if(!valid[bufPos=j])
                        {
                            buf[bufPos+j]=buf[getIndex(kickP, offset)];
                            valid[bufPos+j]=true;
                            buf[getIndex(kickP, offset)] = kickFp;
                            count++;
                            return true;
                        }
                    }
                }
                int tmp = buf[getIndex(kickP,kickSlot)];
                buf[getIndex(kickP, kickSlot)]=kickFp;
                kickFp = tmp;
                kickP = tmpKickP;
#else
                //random kick
                int kickSlot = rand() % bucketsize;

                int tmp = buf[getIndex(kickP,kickSlot)];
                buf[getIndex(kickP,kickSlot)]=kickFp;
                kickFp = tmp;

                kickP = getAnotherPos(kickP, kickFp);
                int kickBufPos=getIndex(kickP,0);
                for (int i = 0; i<bucketsize; i++) 
                {
                    if(!valid[kickBufPos+i])
                    {
                        buf[kickBufPos+i]=kickFp;
                        valid[kickBufPos+i]=true;
                        count++;
                        return true;
                    }
                }
#endif
            }
            victimfp = kickFp;
            victimpos = kickP;
            return false;
        }
        bool insertWithoutkick(uint fp, int idx) 
        {
            int bufPos=getIndex(idx,0);
            for (int i = 0; i < bucketsize; i++) 
            {
                if(!valid[bufPos+i])
                {
                    buf[bufPos+i]=fp;
                    valid[bufPos+i]=true;
                    count++;
                    return true;
                }
            }
            return false;
        }
    };
    class LinkedCFoperator 
    {
    public:
        bool operator() (const LinkedCF* lc, const LinkedCF* rc) 
        {
            return lc->count < rc->count;
        }
    };

protected:
    int nBucket;
    int BucketSize;
    LinkedCF * head;
    LinkedCF * curr;
    LinkedCF * next;
    hashFunction hFP; //fingerprint
    hashFunction hc; //cuckoo hash  

    //const static int MAXKICK = 250;
    const static int MAXKICK = 250;
    const static int KEYSIZE = 4;
    //const static int FPSIZE = sizeof(FPTYPE);
public:
    dynamicCuckoo(int _nBucket, int _BucketSize, hashFunction _hFP, hashFunction _hc)
    :nBucket(_nBucket)
    ,BucketSize(_BucketSize)
    ,hFP(_hFP)
    ,hc(_hc)
    {
        //LinkedCF(int _nbucket, int _bucketsize, hashFunction _hFP, hashFunction _hc, LinkedCF * _nextCF = NULL)
        next = NULL;
        curr = new LinkedCF(_nBucket, _BucketSize, _hFP, _hc);
        head = curr;
    }
    bool insert(string key) 
    {
        uint victimfp;
        int victimpos;
        bool token = curr->insert(key, victimfp, victimpos);
        if (token) 
        {
            return true;
        }
        else 
        {
            //clock_t time1, time2;
            //time1 = clock();
            //cout << time1 << ' ';
            curr->nextCF = new LinkedCF(nBucket, BucketSize, hFP, hc);
            //time2 = clock();
            //cout << time2 << ' ';
            //cout << "time2 - time1 = " << time2 - time1 << ' ';
            //return false;

            curr = curr->nextCF;
            token = curr->insertWithoutkick(victimfp, victimpos);
            if (!token) 
            {
                //err occurs here
                cerr << "insert failed" << endl;
            }
        }
        return true;
    }
    bool lookup(string key) 
    {
        LinkedCF * tmp = head;
        while (tmp != NULL) 
        {
            if (tmp->inquiry(key)) 
            {
                return true;
            }
            tmp = tmp->nextCF;
        }
        return false;
    }
    bool del(string key) 
    {
        LinkedCF * tmp = head;
        bool token = false;
        while(tmp != NULL) 
        {
            if (tmp->del(key)) 
            {
                token = true;
            }
            tmp = tmp->nextCF;
        }
        return token;
    }
    bool compress() 
    {
        multiset<LinkedCF *, LinkedCFoperator> CFQ;
        multiset<LinkedCF *, LinkedCFoperator> cfq;
        typename multiset<LinkedCF *, LinkedCFoperator>::iterator lit;
        typename multiset<LinkedCF *, LinkedCFoperator>::reverse_iterator rit;
        CFQ.clear();
        LinkedCF * tmp = head;
        while (tmp != NULL) 
        {
            CFQ.insert(tmp);
            tmp = tmp->nextCF;
        }
        //cout<<"queue created"<<endl;
        for (lit = CFQ.begin(); lit != CFQ.end(); lit++) 
        {
            //cout<<(*lit)->count<<endl;
            int failnum = 0;
            //search every bucket
            for (int i = 0; i < nBucket; i++) 
            {
                int bufPos=(*lit)->getIndex(i,0);
                //for each bucket, search every unit
                for (int j = 0; j < BucketSize; j++) 
                {
                    if((*lit)->valid[bufPos+j]) 
                    {
                        uint tmpfp = (*lit)->buf[bufPos+j];
                        bool succ = false;
                        //search from the end to the start
                        for (rit = CFQ.rbegin(); (*rit) != (*lit); rit++) 
                        {
                            //cout<<(*rit)->count<<endl;
                            succ = (*rit)->insertWithoutkick(tmpfp, i);
                            if (succ) 
                            {
                                (*lit)->buf[bufPos+j] = false;
                                (*lit)->count--;
                                break;
                            }
                        }
                        if (!succ)
                            failnum++;
                    }
                    if ((*lit)->count == failnum)
                        break;
                }
                if ((*lit)->count == failnum)
                    break;
            }
        }
        //cout << "insert finished" << endl;
        cfq.clear();
        for (lit = CFQ.begin(); lit != CFQ.end(); lit++) 
        {
            cfq.insert(*lit);
        }
        CFQ.clear();
        //adjust to the new DCF structure
        rit = cfq.rbegin();
        head = *rit;
        LinkedCF * prev = head;
        for (++rit; rit != cfq.rend(); rit++) 
        {
            if ((*rit)->count == 0) 
            {
                prev->nextCF = NULL;
                //delete[] (*rit);
                break;
            }
            else 
            {
                prev->nextCF = *rit;
                prev = *rit;
            }
        }
        curr = prev;
        return true;
    }
    void printBuf()
    {
        for(auto tmpp = head; tmpp != NULL;)
        {
            cout<<tmpp->count<<" ";
            tmpp = tmpp->nextCF;
            if(tmpp == NULL)
			    break;
        };
        cout<<endl;
    }
    bool resize(int len){}
    int Get_Memory_Access_Num(){}
    int Get_Hop_Num(){}
    int Get_Cf_Num(){
        LinkedCF * tmp = head;
        int cnt = 0;
        while(tmp != NULL) {
            cnt++;
            tmp = tmp->nextCF;
        }
        return cnt;
    }
    int Get_rnum() {
         LinkedCF * tmp = head;
        int cnt = 0;
        while(tmp != NULL) {
            cnt += tmp->rnum;
            tmp = tmp->nextCF;
        }
        return cnt;
    }
    ~dynamicCuckoo(){}
};



#endif
