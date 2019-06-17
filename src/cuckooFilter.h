#ifndef __CUCKOO_FILTER_H__
#define __CUCKOO_FILTER_H__

#include "hash/hash_function.h"
#include "cuckoo.h"

using namespace std;
#include <cstring>
#include <string>
#include <iostream>
#include <time.h>

class cuckooFilter: public cuckoo{
public:
	cuckooFilter(int _L, int _slot, hashFunction _hfp, hashFunction _hpos):
	L(_L), slot(_slot), hfp(_hfp), hpos(_hpos), mem_r(0), mem_w(0), 
	hop_num(0){
        int tSize=L * slot;
        buf = new uint[tSize];
        memset(buf, 0, tSize * sizeof(uint));
        valid = new bool[tSize];
        memset(valid, 0, tSize * sizeof(bool));
	}
	~cuckooFilter(){
	    delete [] buf;
        delete [] valid;
    }

    inline int getIndex(int p,int s)
    {
        return p*slot+s;
    }

	void Write(int p, int t, uint fp, int mem_w_n){
        int bufPos = getIndex(p, t);
		valid[bufPos]=true;
		buf[bufPos]=fp;
		mem_w += mem_w_n;
	}
	bool EmptyAlternative(int p, int t, int &alter_pos){
		int bufPos = getIndex(p, t);
        uint fp = buf[bufPos];
		int y = (hpos((char*)&fp, 4)^p) % L;
        int bufPosY = getIndex(y, 0);
		for(int i = 0; i < slot; ++i){
            mem_r++;
			if(valid[bufPosY+i] == 0){
				alter_pos = i;
				return true;
			}
		}
        return false;		
	}
 	bool insert(string key){
 		uint fp = hfp(key.c_str(), 4);
 		//fp &= ((1<<8)-1);
 		int p1 = hpos(key.c_str(), 4) % L;
 		int p2 = (p1 ^ hpos((char*)&fp, 4)) % L;
        int bufPos1=getIndex(p1, 0);
        int bufPos2=getIndex(p2, 0);
#if 0
 		// already exist
 		for(int i = 0; i < slot; ++i){
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
		// find empty slot
		for(int i = 0; i < slot; ++i){
            mem_r++;
            if(!valid[bufPos1+i]){
				Write(p1, i, fp, 2);
				return true;
			}
            mem_r++;
			if(!valid[bufPos2+i]){
				Write(p2, i, fp, 2);
				return true;
			}
		}
// optimize 
#if 0	
		int alter_pos = 0;
		for(int i = 0; i < slot; ++i){
			if(EmptyAlternative(p1, i, alter_pos)){
				uint _fp = buf[bufPos1+i];
				int x = alter_pos;
				int y = (hpos((char*)&_fp, 4)^p1) % L;
                int bufPos = getIndex(y, x);
				mem_w += 4;
                buf[bufPos]=_fp;
                valid[bufPos] = 1;
				buf[bufPos1+i] = fp;
				valid[bufPos1+i] = 1;
				hop_num++;
				return true;
			}
			if(EmptyAlternative(p2, i, alter_pos)){
				uint _fp = buf[bufPos2+i];
				int x = alter_pos;
				int y = (hpos((char*)&_fp, 4)^p2) % L;
				int bufPos = getIndex(y, x);
				mem_w += 4;
                buf[bufPos] = _fp;
                valid[bufPos] = 1;
				buf[bufPos2+i] = fp;
                valid[bufPos2+i] = 1;
				hop_num++;
				return true;
			}
		}

// 2 hop check

/*
		for(int i = 0; i < slot; ++i){
			uint _fp1 = bucket[p1]->fp[i];
			int x2 = (hpos((char*)&_fp1, 4)^p1) % L;
			for(int j = 0; j < slot; ++j){
				uint _fp2 = bucket[x2]->fp[j];
				int x3 = (hpos((char*)&_fp2, 4)^x2) % L;
				if(EmptyAlternative(x2, j, alter_pos)){
					int x = alter_pos;
					bucket[x3]->fp[x] = _fp2;
					bucket[x3]->valid[x] = 1;
					bucket[x2]->fp[j] = _fp1;
					bucket[p1]->fp[i] = fp;
					return true;
				}
			}
		}
		for(int i = 0; i < slot; ++i){
			uint _fp1 = bucket[p2]->fp[i];
			int x2 = (hpos((char*)&_fp1, 4)^p2) % L;
			for(int j = 0; j < slot; ++j){
				uint _fp2 = bucket[x2]->fp[j];
				int x3 = (hpos((char*)&_fp2, 4)^x2) % L;
				if(EmptyAlternative(x2, j, alter_pos)){
					int x = alter_pos;
					bucket[x3]->fp[x] = _fp2;
					bucket[x3]->valid[x] = 1;
					bucket[x2]->fp[j] = _fp1;
					bucket[p2]->fp[i] = fp;
					return true;
				}
			}
		}
// 3 hop check
		for(int i = 0; i < slot; ++i){
			uint _fp1 = bucket[p1]->fp[i];
			int x2 = (hpos((char*)&_fp1, 4)^p1) % L;
			for(int j = 0; j < slot; ++j){
				uint _fp2 = bucket[x2]->fp[j];
				int x3 = (hpos((char*)&_fp2, 4)^x2) % L;
				for(int t = 0; t < slot; ++t){
					uint _fp3 = bucket[x3]->fp[t];
					int x4 = (hpos((char*)&_fp3, 4)^x3) % L;
					if(EmptyAlternative(x3, t, alter_pos)){
						int x = alter_pos;
						bucket[x4]->fp[x] = _fp3;
						bucket[x4]->valid[x] = 1;
						bucket[x3]->fp[t] = _fp2;
						bucket[x2]->fp[j] = _fp1;
						bucket[p1]->fp[i] = fp;
						return true;
					}
				}
			}
		}
		for(int i = 0; i < slot; ++i){
			uint _fp1 = bucket[p2]->fp[i];
			int x2 = (hpos((char*)&_fp1, 4)^p2) % L;
			for(int j = 0; j < slot; ++j){
				uint _fp2 = bucket[x2]->fp[j];
				int x3 = (hpos((char*)&_fp2, 4)^x2) % L;
				for(int t = 0; t < slot; ++t){
					uint _fp3 = bucket[x3]->fp[t];
					int x4 = (hpos((char*)&_fp3, 4)^x3) % L;
					if(EmptyAlternative(x3, t, alter_pos)){
						int x = alter_pos;
						bucket[x4]->fp[x] = _fp3;
						bucket[x4]->valid[x] = 1;
						bucket[x3]->fp[t] = _fp2;
						bucket[x2]->fp[j] = _fp1;
						bucket[p2]->fp[i] = fp;
						return true;
					}
				}
			}
		}
*/
#endif

		// kick
        //srand((unsigned)time(NULL));
        int _fp, t;
        int kickP = rand() % 2 == 0 ? p1 : p2;
		for(int i = 0; i < max_kick; ++i){
			hop_num++;
			t = rand() % slot;
			_fp = buf[getIndex(kickP,t)];
            mem_r++;
			Write(kickP, t, fp, 1);
			// find alternative postion
			kickP = (hpos((char*)&_fp, 4)^kickP) % L;
			int kickBufPos=getIndex(kickP,0);
            fp = _fp;
			// optimize
			// if(i <= 0) continue;
			for(int j = 0; j < slot; ++j){
			    mem_r++;
				if(!valid[kickBufPos+j]){
					Write(kickP, j, fp, 2);
					return true;
				}
			}
		}
		return false;
 	}
 	bool lookup(string key){
 		uint fp = hfp(key.c_str(), 4);
 		//fp &= ((1<<8)-1);
 		int p1 = hpos(key.c_str(), 4) % L;
 		int p2 = (p1 ^ hpos((char*)&fp, 4)) % L;
 		int bufPos1=getIndex(p1, 0);
        int bufPos2=getIndex(p2, 0);
        for(int i = 0; i < slot; ++i)
        {
            mem_r++;
 			if(valid[bufPos1+i]){
                mem_r++;
                if(buf[bufPos1+i]==fp)
 				    return true;
            }
            mem_r++;
            if(valid[bufPos2+i]) {
                mem_r++;
                if(buf[bufPos2+i]==fp)
 				    return true;
 		    }
        }
 		return false;
 	}
	bool del(string key){}
	bool resize(int len){}
	int Get_mem_r(){
		return mem_r;
	}
	int Get_mem_w(){
		return mem_w;
	}
	int Get_Hop_Num(){
		return hop_num;
	}

private:
	int L, slot;
	int mem_r;
	int mem_w;
	int hop_num;
	uint *buf;
    bool *valid;
    hashFunction hfp;
	hashFunction hpos;
	int max_kick = 250;
};

#endif
