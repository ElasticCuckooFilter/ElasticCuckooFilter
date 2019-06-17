#ifndef __ELASTIC_CUCKOO_FILTER_H__
#define __ELASTIC_CUCKOO_FILTER_H__

#include "hash/hash_function.h"
#include "cuckoo.h"

using namespace std;
#include <memory.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <time.h>
#include <assert.h>

class ECF:public cuckoo
{
public:
    ECF(int _len, int _slot,
        hashFunction _hFP, hashFunction _hOffset, hashFunction _hc
        , int _hop_check = 0)
    :bLen(_len)
    ,bSlot(_slot)
    ,hFP(_hFP)
    ,hOffset(_hOffset)
    ,hc(_hc)
    ,mem_r(0)
    ,mem_w(0)
    ,hop_num(0)
    {
        srand((unsigned)time(NULL));
        int tSize = _len * _slot;
        buf = new uint[tSize];
        memset(buf, 0, tSize * sizeof(uint));
        valid = new bool[tSize];
        memset(valid, 0, tSize * sizeof(bool));
        copy=NULL;

        hop_check = _hop_check;
        if(hop_check<0)hop_check=0;

        stat = INI;
        copyNum = 1;

        hLen=1;
        hPower=0;
        while(hLen<=bLen)
        {
            hLen*=2;
            hPower++;
        }
        hLen/=2;
        hPower--;

        cbLen=bLen;
        comLv=1;
        hashPower=hPower;
        rnum=0;
    }

    ~ECF()
    {
        delete [] buf;
        delete [] valid;
        if(copy!=NULL)
            delete [] copy;
    }

    inline int getIndex(int pos, int slot)
    {
        return pos*bSlot + slot;
    }

    int ring(int pos)
    {
        if(pos>=bLen)
            pos-=bLen;
        else if(pos<0)
            pos+=bLen;
        return pos;
    }

    void getPosByKey(string key, uint&fp, int&p1, int&p2)
    {
        fp=hFP(key.c_str(),4) & 0x00000fff;
        uint hashing=hOffset((char*)&fp,4);//using hOffset twice
        int start=(hashing>>hashPower)%cbLen;
        //int start=hOffset((char*)&fp,4)%bLen;
        int hashk=hc(key.c_str(),4)&(hLen-1);
        int hashfk=hashing&(hLen-1);
        //int hashfk=hc((char*)&fp,4)&(hLen-1);
        
        start/=comLv;
        hashk/=comLv;
        hashfk/=comLv;
        p1=ring(start+hashk);
        p2=ring(start+(hashk^hashfk));
   }

    int getAnotherPos(int pos, uint fp)
    {
        uint hashing=hOffset((char*)&fp,4);//using hOffset twice
        int start=(hashing>>hashPower)%cbLen;
        int hashfk=hashing&(hLen-1);
        //int start=hOffset((char*)&fp,4)%bLen;
        //int hashfk=hc((char*)&fp,4)&(hLen-1);
        start/=comLv;
        hashfk/=comLv;
        return ring(start+(ring(pos-start)^hashfk));
    }

    void removeAllCopy()
    {
        if(stat!=EXP)
            return;
        for(int i=0;i<bLen;i++)
            removeCopy(i);
    }
    void removeCopy(int pos)
    {
        if(stat!=EXP)
            return;
        assert(copy!=NULL);
        mem_r++;
        if(copy[pos])
            return;

        int bufPos=getIndex(pos, 0);
        for(int i=0;i<bSlot;i++)
        {
            mem_r++;
            if(!valid[bufPos+i])
                continue;
            uint fp=buf[bufPos+i];
            int start=(hOffset((char*)&fp,4)>>hashPower)%cbLen;
            start/=comLv;
            int offset=ring(pos-start);
            if(offset>=hLen)
                valid[bufPos+i]=false; 
        }
        copy[pos]=true;
        mem_w++;
    }

    bool alternative(int p, int t, int &alter_pos, int &alter_slot){ 
        mem_r++;
        uint fp = buf[getIndex(p,t)];
        int y = getAnotherPos(p, fp);
        int bufPosY = getIndex(y, 0);
        for(int i = 0; i < bSlot; ++i)
        {
            mem_r++;
            if(!valid[bufPosY+i]){ 
                alter_slot = i;
                alter_pos = y;
                return true;  
            }
        }
        return false;
    }

    bool insert(string key)
    {
        uint fp;
        int p1, p2;
        getPosByKey(key, fp, p1, p2);
        return insertWithoutKey(fp, p1, p2);
    }
    bool insertWithoutKey(uint fp, int p1, int p2)
    {
        int bufPos1=getIndex(p1, 0);
        int bufPos2=getIndex(p2, 0);
        removeCopy(p1);
        removeCopy(p2);

#if 0
        // already exist
        for(int i=0;i<bSlot;i++)
        {
            mem_r++;
            if(valid[bufPos1+i]){
                mem_r++;
                if(buf[bufPos1+i] == fp)
                    return true;
            }
            mem_r++;
            if(valid[bufPos2+i]){
                mem_r++;
                if(buf[bufPos2+i] == fp)
                    return true;
            }
        }
#endif
        //find empty slot
        for(int i=0;i<bSlot;i++)
        {
            mem_r++;
            if(!valid[bufPos1+i])
            {
                buf[bufPos1+i]=fp;
                valid[bufPos1+i]=true;
                mem_w+=2;
                return true;
            }
            mem_r++;
            if(!valid[bufPos2+i])
            {
                buf[bufPos2+i]=fp;
                valid[bufPos2+i]=true;
                mem_w+=2;
                return true;
            }
        }

        if(hop_check>0)
        {
            //random kick with one hop check in first kick
            int alter_pos = 0;
            int alter_slot = 0;
            for(int i = 0; i < bSlot; ++i){
                if(alternative(p1, i, alter_pos, alter_slot)){
                    uint _fp = buf[bufPos1+i];
                    int bufPos=getIndex(alter_pos,alter_slot);
                    buf[bufPos] = _fp;
                    valid[bufPos] = true;
                    buf[bufPos1+i] = fp;
                    valid[bufPos1+i] = true;  
                    mem_w+=4;
                    hop_num++;
                    return true;
                }
                if(alternative(p2, i, alter_pos, alter_slot)){
                    uint _fp = buf[bufPos2+i];
                    int bufPos=getIndex(alter_pos,alter_slot);
                    buf[bufPos] = _fp;
                    valid[bufPos] = true;
                    buf[bufPos2+i] = fp;
                    valid[bufPos2+i] = true; 
                    mem_w+=4;
                    hop_num++;
                    return true;
                }
            }
        }

        //kick
        int kickP=rand()%2==0?p1:p2;
        uint kickFp=fp;
        for(int i=0;i<MAXKICK;i++)
        {
            hop_num++;
#if 0
            //random kick with one hop check
            int kickSlot=rand()%bSlot;
            int tmpKickP;

            int offset=rand()%bSlot;
            for(int j=0;j<bSlot;j++,offset++)
            {
                if(offset>=bSlot)offset=0;
                int p2=getAnotherPos(kickP, buf[getIndex(kickP, offset)]);
                removeCopy(p2); 
                int bufPos=getIndex(p2, 0);
                memory_access_num++;

                if(kickSlot==offset)
                    tmpKickP=p2;
                for(int k=0;k<bSlot;k++)
                {
                    if(!valid[bufPos+k])
                    {
                        buf[bufPos+k]=buf[getIndex(kickP, offset)];
                        valid[bufPos+k]=true;
                        buf[getIndex(kickP, offset)]=kickFp;
                        memory_access_num += 4;
                        return true;
                    }
                    else
                        memory_access_num++;
                }
            }
            int tmp=buf[getIndex(kickP, kickSlot)];
            buf[getIndex(kickP,kickSlot)]=kickFp;
            memory_access_num += 2;
            kickFp=tmp;
            kickP = tmpKickP;
#else
            //random kick
            int kickSlot=rand()%bSlot;
            
            int tmp=buf[getIndex(kickP, kickSlot)];
            mem_r++;
            int kickP2=getAnotherPos(kickP, tmp);
            //remove copy
            removeCopy(kickP2);

            buf[getIndex(kickP, kickSlot)]=kickFp;
            mem_w++;

            kickFp=tmp;
            kickP = kickP2;
            int kickBufPos=getIndex(kickP, 0);
            //check if there is an empty slot
            for(int j=0;j<bSlot;++j)
            {
                mem_r++;
                if(!valid[kickBufPos+j])
                {
                    buf[kickBufPos+j]=kickFp;
                    valid[kickBufPos+j]=true;
                    mem_w+=2;
                    return true;
                }
            }
            //one hop check
            if(i+1<hop_check)
            {
                int kPos=getIndex(kickP,0);
                for(int j = 0; j < bSlot; ++j)
                {
                    int alter_pos,alter_slot;
                    if(alternative(kickP, j, alter_pos, alter_slot))
                    {
                        mem_r++;
                        uint _fp = buf[kPos+j];
                        int bufPos=getIndex(alter_pos,alter_slot);
                        buf[bufPos] = _fp;
                        valid[bufPos] = true;
                        buf[kPos+j] = kickFp;
                        valid[kPos+j] = true;  
                        mem_w+=4;
                        return true;
                    }
                }
            }
#endif
        }
        return false;
    }

    int Get_mem_r(){return mem_r;}
    int Get_mem_w(){return mem_w;}
    int Get_Hop_Num(){
        return hop_num;
    }
    bool lookup(string key)
    {
        uint fp;
        int p1, p2;
        getPosByKey(key, fp, p1, p2);

        //remove copy
        //removeCopy(p1);
        //removeCopy(p2);

        //find
        int bufPos1=getIndex(p1, 0);
        int bufPos2=getIndex(p2, 0); 
        rnum++;
        for(int i=0;i<bSlot;i++)
        {
            mem_r++;
            if(valid[bufPos1+i]){
                mem_r++;
                if(buf[bufPos1+i]==fp)
                    return true;
            }
        }
        rnum++;
        for(int i=0;i<bSlot;i++)
        {
            mem_r++;
            if(valid[bufPos2+i]){
                mem_r++;
                if(buf[bufPos2+i]==fp)
                    return true;
            }
        }
        return false;
    }

    bool del(string key)
    {
        uint fp;
        int p1, p2;
        getPosByKey(key, fp, p1, p2);

        //remove copy
        //removeCopy(p1);
        //removeCopy(p2);

        //find
        int bufPos1=getIndex(p1, 0);
        int bufPos2=getIndex(p2, 0);  
        for(int i=0;i<bSlot;i++)
        {
            mem_r++;
            if(valid[bufPos1+i]){
                mem_r++;
                if(buf[bufPos1+i]==fp)
                {
                    valid[bufPos1+i]=false;
                    mem_w++;
                    return true;
                }
            }
            mem_r++;
            if(valid[bufPos2+i]){
                mem_r++;
                if(buf[bufPos2+i]==fp)
                {
                    valid[bufPos2+i]=false;
                    mem_w++;
                    return true;
                }
            }
        }
        return false;
    }

    void calcPower(int len, int&size, int&power)
    {
        if(len<=0)
            return;
        size=1, power=0;
        while(size<=len)
        {
            size*=2;
            power++;
        }
        size=size/2;
        power--;
    }

    bool resize(int len)
    {
        if(len>bLen && len%bLen==0)
            return expand(len);
        else if(len<bLen && len>1 && len==(bLen+1)/2)
            return compress();
        return false;
    }

    bool compress()
    {
        //save backup info
        int saveBLen=bLen;
        int saveHLen=hLen;
        uint*saveBuf=buf;
        bool*saveValid=valid;
       
        comLv*=2;
        
        //new space
        int len=(bLen+1)/2;
        bLen=len; 
        int tSize=len * bSlot;
        buf = new uint[tSize];
        memset(buf, 0, tSize * sizeof(uint));
        valid = new bool[tSize];
        memset(valid, 0, tSize * sizeof(bool));

        //transfer
        bool suc=true;
        for(int i=0;i<saveBLen;i++)
        {
            int bufPos=getIndex(i, 0);
            for(int j=0;j<bSlot;j++)
            {
                if(saveValid[bufPos+j])
                { 
                    uint transFp=saveBuf[bufPos+j];
                    int p1,p2;
                    if(i%2==1)
                        p1=i/2;
                    else
                    {
                        int hashing = hOffset((char*)&transFp,4);
                        int o=(hashing>>hashPower)%cbLen;
                        o=o/(comLv/2);
                        if(o%2==0)
                            p1=i/2;
                        else
                            p1=(i/2-1+bLen)%bLen;
                    }
                    p2=getAnotherPos(p1,transFp);
                    suc=insertWithoutKey(transFp,p1,p2);
                }
                if(!suc)
                    break;
            }
            if(!suc)
                break;
        }
        
        if(!suc)
        {
            delete buf;
            delete valid;
            buf = saveBuf;
            valid = saveValid;
            bLen = saveBLen;
            comLv/=2;
            return false;
        }
        if(copy!=NULL)
        {
            delete [] copy;
            copy = new bool[len];
            memset(copy, 0, len*sizeof(bool));
        }

        delete [] saveBuf;
        delete [] saveValid;
        return true;       
    }

    bool expand(int len)
    {
        //if(stat!=INI)
        //    return false;
        stat=EXP;
       
        int expandTimes = len/bLen;
        copyNum = copyNum * expandTimes;
        cbLen = cbLen * expandTimes;

        //save backup info
        int saveBLen=bLen;
        int saveHLen=hLen;
        int saveHPower=hPower;
        uint*saveBuf=buf;
        bool*saveValid=valid;
        bool*saveCopy=copy;
        
        //new space
        bLen=len;
        calcPower(len, hLen, hPower);
        if(hPower>saveHPower)
        {   //hPower cannot be larger than before
            hLen=saveHLen;
            hPower=saveHPower;
        }
        int tSize=len * bSlot;
        buf = new uint[tSize];
        valid = new bool[tSize];
        copy = new bool[len];

        //transfer
        int saveSize = saveBLen * bSlot;
        int s1 = saveSize * sizeof(uint), s2 = saveSize * sizeof(bool);
        for(int i=0;i<expandTimes;i++)
        {
            memcpy((char*)buf + s1*i, (char*)saveBuf, s1);
            memcpy((char*)valid + s2*i, (char*)saveValid, s2);
        }
        memset(copy, 0, len*sizeof(bool));
        delete [] saveBuf;
        delete [] saveValid;
        if(saveCopy!=NULL)
            delete [] saveCopy;
        return true;
    }

    void printBuf()
    {
        //return;
        cout<<"para:"<<bLen<<" "<<bSlot<<" "<<hLen<<endl;
        for(int i=0;i<bLen;i++)
        {
            for(int j=0;j<bSlot;j++)
            {
                if(valid[getIndex(i, j)])
                    cout<<buf[getIndex(i, j)]<<" ";
                else
                    cout<<"X ";
            }
            cout<<endl;
        }
    }
    int rnum;
protected:
    int bLen;
    int bSlot;
    int hLen;
    int hPower;
    int mem_r;
    int mem_w;
    int hop_num;
    uint *buf;
    bool *valid;
    bool *copy; //used if stat is EXP
    enum{INI, EXP} stat;
    int copyNum;
    int cbLen, comLv;
    int hashPower;

    //bool isExpanded;
    hashFunction hFP; //fingerprint
    hashFunction hOffset; //starting position
    hashFunction hc; //cuckoo hash  
    int hop_check;
    string strategyName;

    int MAXKICK = 250;

    //virtual int findEviction() = 0;
    //virtual void updatePosition(int pos) = 0;
};



#endif
