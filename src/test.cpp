#include "cuckoo.h"
#include "ECF.h"
#include "cuckooFilter.h"
#include "dynamicCuckoo.h"
#include "hash/hash_function.h"

#include <vector>
#include <string>
#include <cstring>
#include <time.h>
#include <algorithm>
#include <set>
using namespace std;

const int max_num = 1<<18; 
int cnt = 0;
const int Len = 4;

char caidaFilename[4][100] = {"../dat/caida/0.dat", "../dat/caida/1.dat", "../dat/caida/2.dat", "../dat/caida/3.dat"};
char synFilename[100] = "../dat/syn/syn_out.txt";
string ele[2 * max_num]; // store different string
set<string> s;

bool readDataset(bool isReal) 
{
    FILE *fin = NULL;
    // generate real dataset 
    if (isReal) {
        cnt = 0;
        char five_tuple[13];
        for(int i = 0; i < 4; ++i){
            fin = fopen(caidaFilename[i], "rb");
            if(fin == NULL)
                break;
            while(fread((void *)five_tuple, sizeof(char), 13, fin) == 13){
                string str = string(five_tuple, Len);
                if(!s.count(str)){ // assure difference
                    ele[cnt++] = string(five_tuple, Len);
                    if(cnt == (int)(1.0 * max_num))
                        break;
                    s.insert(str);
                }
            }
            fclose(fin);
            if(cnt == (int)(1.0 * max_num))
                break;
        }
    }
    // generate synthetic dataset
    else {
        char syn[13];
        fin = fopen(synFilename, "rb");
        if(fin == NULL){
            printf("File not found. Generate synthetic datasets first.\n");
            return false;
        }
        cnt = 0; 
        string str;
        while(fread((void *)syn, sizeof(char), 13, fin) == 13){
            string str = string(syn, Len);
            if(!s.count(str)){
                ele[cnt++] = string(syn, Len);
                if(cnt == (int)(1.0 * max_num))
                    break;
                s.insert(str);
            }
        }
        fclose(fin);
    }
    return true;
}

void testMaxInsertNumber(FILE *wf) 
{
    double Ringsize[21] = {1.0, 1.05, 1.1, 1.15, 1.2, 1.25, 1.3, 1.35, 1.4, 1.45, 1.5
        , 1.55, 1.6, 1.65, 1.7, 1.75, 1.8, 1.85, 1.9, 1.95, 2.0};

    int P = (1<<12);
    int insert_success_num = 0;

    for (int j = 0; j < 21; ++j) {
        
        int tmp = (int)(Ringsize[j] * P);
        ECF *cR = new ECF(tmp, 4, BOB1, BOB2, BOB3);
        insert_success_num = 0;

        for(int i = 0; i < tmp; ++i) {
            if(cR->insert(ele[i]))
                insert_success_num++;
        }
        
        fprintf(wf, "insert number of (ECF) %d\n", insert_success_num);
        fprintf(wf, "bucket number of (ECF) %.2lf*(2^12)\n", Ringsize[j]);
    }
    fprintf(wf, "\n");

    cuckooFilter *cF;
    cF = new cuckooFilter(P, 4, BOB1, BOB2);
    insert_success_num = 0;
    for(int i = 0; i < P; ++i) {
            if(cF->insert(ele[i]))
                insert_success_num++;
        }
    
    fprintf(wf, "insert number of (cuckooFilter) %d\n", insert_success_num);
    fprintf(wf, "bucket number of (cuckooFilter) is 2^12\n");

    cF = new cuckooFilter(P<<1, 4, BOB1, BOB2);
    insert_success_num = 0;
    for(int i = 0; i < (P<<1); ++i) {
            if(cF->insert(ele[i]))
                insert_success_num++;
        }
    
    fprintf(wf, "insert number of (cuckooFilter) %d\n", insert_success_num);
    fprintf(wf, "bucket number of (cuckooFilter) is 2^13\n");

    fprintf(wf, "\n");   
}

void testInsertThp(FILE *wf)
{
    timespec time3, time4;
    ECF *cR;
    cuckooFilter *cF;
    long long sum_time = 0;
    int test_num = 20;
    int insert_success_num = 0;
    int size=(1<<15) ;//* 3/4;
    int tCnt = (1<<17) * 0.8 ;//* 3 / 4;
    int delta = 250 ;//* 3 / 4;

    for (int r = 0; r < test_num; ++r) {
        int beginCnt = 0;//r * tCnt;
        cR = new ECF(size, 4, BOB1, BOB2, BOB3);
        for (int i = beginCnt; i < beginCnt + tCnt - delta; ++i)
            if(cR->insert(ele[i]))
                insert_success_num++;
        clock_gettime(CLOCK_MONOTONIC, &time3);
        for (int i = beginCnt + tCnt -delta; i < beginCnt + tCnt; ++i)
            if(cR->insert(ele[i]))
                insert_success_num++;
        clock_gettime(CLOCK_MONOTONIC, &time4);
        sum_time += (long long)(time4.tv_sec - time3.tv_sec) * 1000000000LL + (time4.tv_nsec - time3.tv_nsec);
        // sum_time += cR->Get_Hop_Num();
        // mem_w += cR->Get_mem_w();
        // mem_r += cR->Get_mem_r();
        delete cR;
    }

    fprintf(wf, "ECF\n");
    fprintf(wf, "insert throughput: %.6lfKips\n", (double)1000000 * test_num * delta / sum_time);
    fprintf(wf, "\n");
               // return;
    sum_time = 0;
    insert_success_num = 0;

    for (int r = 0; r < test_num; ++r) {
        int beginCnt = 0;//r * tCnt;
        cF = new cuckooFilter(size, 4, BOB1, BOB2);
        for (int i = beginCnt; i < beginCnt + tCnt - delta; ++i)
            if(cF->insert(ele[i]))
                insert_success_num++;
        clock_gettime(CLOCK_MONOTONIC, &time3);
        for (int i = beginCnt + tCnt -delta; i < beginCnt + tCnt; ++i)
            if(cF->insert(ele[i]))
                insert_success_num++;
        clock_gettime(CLOCK_MONOTONIC, &time4);
        sum_time += (long long)(time4.tv_sec - time3.tv_sec) * 1000000000LL + (time4.tv_nsec - time3.tv_nsec);
        delete cF;
    }

    fprintf(wf, "cuckooFilter\n");
    fprintf(wf, "insert throughput: %.6lfKips\n", (double)1000000 * test_num * delta / sum_time);
    fprintf(wf, "\n");
}

// half insert and query all
void testLookupThp(FILE *wf)
{
    timespec time3, time4;
    ECF *cR;
    cuckooFilter *cF;
    long long sum_time = 0;
    int test_num = 20;
    int lookup_success_num;
    int size = 1<<14;
    int tCnt = (1<<17) * 0.7;

    lookup_success_num=0;
    for (int r = 0; r < test_num; ++r) {
        int beginCnt = 0;//r * tCnt;
        cR = new ECF(size, 4, BOB1, BOB2, BOB3);
        for (int i = beginCnt; i < beginCnt + tCnt / 2; ++i)
            cR->insert(ele[i]);

        clock_gettime(CLOCK_MONOTONIC, &time3);
        for (int i = beginCnt; i < beginCnt + tCnt; ++i)
            if(cR->lookup(ele[i]))
                lookup_success_num++;
        clock_gettime(CLOCK_MONOTONIC, &time4);
        sum_time += (long long)(time4.tv_sec - time3.tv_sec) * 1000000000LL + (time4.tv_nsec - time3.tv_nsec);
        delete cR;
    }

    fprintf(wf, "ECF\n");
    fprintf(wf, "lookup throughput: %.6lfKips, lookup_suc:%d\n", (double)1000000 * test_num * tCnt / sum_time, lookup_success_num/test_num);
    fprintf(wf, "\n");

    sum_time = 0;

    lookup_success_num = 0;
    for (int r = 0; r < test_num; ++r) {
        int beginCnt = 0;//r * tCnt;
        cF = new cuckooFilter(size, 4, BOB1, BOB2);
        for (int i = beginCnt; i < beginCnt + tCnt / 2; ++i)
            cF->insert(ele[i]);

        clock_gettime(CLOCK_MONOTONIC, &time3);
        for (int i = beginCnt; i < beginCnt + tCnt; ++i)
            if(cF->lookup(ele[i]))
                lookup_success_num++;
        clock_gettime(CLOCK_MONOTONIC, &time4);
        sum_time += (long long)(time4.tv_sec - time3.tv_sec) * 1000000000LL + (time4.tv_nsec - time3.tv_nsec);
        delete cF;
    }

    fprintf(wf, "cuckooFilter\n");
    fprintf(wf, "lookup throughput: %.6lfKips, lookup_suc:%d\n", (double)1000000 * test_num * tCnt / sum_time, lookup_success_num / test_num);
    fprintf(wf, "\n");
}

void testLoadRatio(FILE *wf)
{
    ECF *cR;
    cuckooFilter *cF;
    int test_num = 20;
    int insert_success_num = 0;
    int size = 1<<15;
    int maxFailure=10;
    
    int tCnt = 1<<17;
    for (int r = 0; r < test_num; ++r) {
        int failure=0;
        cR = new ECF(size, 4, BOB1, BOB2, BOB3);
        for(int i = 0; i < tCnt; ++i)
            if(cR->insert(ele[i]))
                insert_success_num++;
            else
            {
                failure++;
                if(failure>=maxFailure)
                    break;
            }

        delete cR;
    }

    fprintf(wf, "ECF\n");
    fprintf(wf, "load ratio: %.6lf\n", (double)insert_success_num / (test_num * size * 4));
    fprintf(wf, "\n");

    insert_success_num = 0;

    for (int r = 0; r < test_num; ++r) {
        int failure = 0;
        cF = new cuckooFilter(size, 4, BOB1, BOB2);
        for(int i = 0; i < tCnt; ++i)
            if(cF->insert(ele[i]))
                insert_success_num++;
            else
            {
                failure++;
                if(failure>=maxFailure)
                    break;
            }
        delete cF;
    }

    fprintf(wf, "cuckooFilter\n");
    fprintf(wf, "load ratio: %.6lf\n", (double)insert_success_num / (test_num * size * 4));
    fprintf(wf, "\n");
}
void testMemoryAccess(FILE *wf)
{
    ECF *cR;
    cuckooFilter *cF;
    int mem_w = 0;
    int mem_r = 0;
    int hop = 0;
    int test_num = 20;
    int size = 1<<15;
    int tCnt= (1<<17) * 0.7;

    for (int r = 0; r < test_num; ++r) {
        cR = new ECF(size, 4, BOB1, BOB2, BOB3);
        for(int i = 0; i < tCnt; ++i)
            cR->insert(ele[i]);
        hop += cR->Get_Hop_Num();
        mem_w += cR->Get_mem_w();
        mem_r += cR->Get_mem_r();
        delete cR;
    }

    fprintf(wf, "ECF\n");
    fprintf(wf, "hop num: %.2lf\n", (double)hop/ test_num);
    fprintf(wf, "memory write: %.2lf\n", (double)mem_w/ test_num);
    fprintf(wf, "memory read: %.2lf\n", (double)mem_r/ test_num);
    fprintf(wf, "\n");

    mem_w = 0;
    mem_r = 0;
    hop = 0;

    for (int r = 0; r < test_num; ++r) {
        cF = new cuckooFilter(size, 4, BOB1, BOB2);
        for(int i = 0; i < tCnt; ++i)
            cF->insert(ele[i]);
        hop += cF->Get_Hop_Num();
        mem_w += cF->Get_mem_w();
        mem_r += cF->Get_mem_r();
        delete cF;
    }

    fprintf(wf, "cuckooFilter\n");
    fprintf(wf, "hop num: %.2lf\n", (double)hop/ test_num);
    fprintf(wf, "memory write: %.2lf\n", (double)mem_w/ test_num);
    fprintf(wf, "memory read: %.2lf\n", (double)mem_r/ test_num);
    fprintf(wf, "\n");
}

void testResizeLookupThp(FILE *wf)
{
    int sum_time1 = 0;
    int sum_time2 = 0;
    timespec time1, time2;
    timespec time3, time4;
    int test_num = 20;
    int size = 1<<15;
    int tCnt= (1<<17) * 0.95;
    int lookup_success_num1=0;
    int lookup_success_num2=0;


    for (int r = 0; r < test_num; ++r) {
        ECF *cR = new ECF(size, 4, BOB1, BOB2, BOB3);
        for(int i = 0; i < tCnt; ++i)
            cR->insert(ele[i]);
        clock_gettime(CLOCK_MONOTONIC, &time1);
        for(int i = 0; i < tCnt * 2; ++i)
            if(cR->lookup(ele[i]))
                lookup_success_num1++;
        clock_gettime(CLOCK_MONOTONIC, &time2);
        sum_time1 += (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
        if(!cR->resize(size * 3))
            fprintf(wf, "failure expansion\n");
        clock_gettime(CLOCK_MONOTONIC, &time3);
        for(int i = 0; i < tCnt * 2; ++i)
            if(cR->lookup(ele[i]))
                lookup_success_num2++;
        clock_gettime(CLOCK_MONOTONIC, &time4);
        sum_time2 += (long long)(time4.tv_sec - time3.tv_sec) * 1000000000LL + (time4.tv_nsec - time3.tv_nsec);
        delete cR;
    }

    fprintf(wf, "lookup throughput before extension: %.6lfKips\n", (double)1000000 * test_num * (tCnt * 2) / sum_time1);
    fprintf(wf, "lookup throughput after extension: %.6lfKips\n", (double)1000000 * test_num * (tCnt * 2) / sum_time2);
    fprintf(wf, "lookup:%d, %d\n",lookup_success_num1, lookup_success_num2);
    fprintf(wf, "\n");

    tCnt = (1<<17) * 0.45;

    lookup_success_num1=lookup_success_num2=0;
    sum_time1 = sum_time2 = 0;
    for (int r = 0; r < test_num; ++r) {
        ECF *cC = new ECF(size, 4, BOB1, BOB2, BOB3);
        for(int i = 0; i < tCnt; ++i)
            cC->insert(ele[i]);
        clock_gettime(CLOCK_MONOTONIC, &time1);
        for(int i = 0; i < tCnt * 2; ++i)
            if(cC->lookup(ele[i]))
                lookup_success_num1++;
        clock_gettime(CLOCK_MONOTONIC, &time2);
        sum_time1 += (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
        if(!cC->resize(size/2))// compression
            fprintf(wf, "failure shrinkage\n");
        clock_gettime(CLOCK_MONOTONIC, &time3);
        for(int i = 0; i < tCnt * 2; ++i)
            if(cC->lookup(ele[i]))
                lookup_success_num2++;
        clock_gettime(CLOCK_MONOTONIC, &time4);
        sum_time2 += (long long)(time4.tv_sec - time3.tv_sec) * 1000000000LL + (time4.tv_nsec - time3.tv_nsec);
        delete cC;
    }

    fprintf(wf, "lookup throughput before compression: %.6lfKips\n", (double)1000000 * test_num * (tCnt * 2) / sum_time1);
    fprintf(wf, "lookup throughput after compression: %.6lfKips\n", (double)1000000 * test_num * (tCnt * 2) / sum_time2);
    fprintf(wf, "lookup:%d, %d\n",lookup_success_num1, lookup_success_num2);
    fprintf(wf, "\n");
}
string genStr()
{
    char ch[13];
    for(int j=0;j < 13;j++)
        ch[j] = rand() % 256;
    string item = string(ch, Len);
    return item;
}
void testSingleResizePFRate(ECF*cR, set<string>&si, set<string>&sn, int testSetCnt, double&fr, double&tp)
{
    set<string> st,sfp;
    int stC;
    st.clear();
    sfp.clear();
    stC=0;      
    while(true)
    {
        string str=genStr();
        if(!st.count(str) && !si.count(str))
        {
            st.insert(str);
            stC++;
            if(stC==testSetCnt)
                break;
        }
    }
    int fptestNum=100000;
    for(int i=0;i<fptestNum;i++)
    {
        string str=genStr();
        if(!sfp.count(str) && !si.count(str))
        {
            sfp.insert(str);
        }
    }
    int fpNum=0;
    for(auto iter=sfp.begin();iter!=sfp.end();iter++)
    {
        if(cR->lookup(*iter))
            fpNum++;
    }
    fr = (double)fpNum/fptestNum;
    timespec time1, time2;
    int sum_time=0;
    for(auto iter=si.begin();iter!=si.end();iter++)
    {
        string str=*iter;
        if(!st.count(str) && !sn.count(str))
        {
            st.insert(str);
            stC++;
            if(stC==testSetCnt * 2)
                break;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &time1);
    for(auto iter = st.begin(); iter != st.end() ; iter++)
        cR->lookup(*iter);
    clock_gettime(CLOCK_MONOTONIC, &time2);
    sum_time = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
    tp = (double)1000000 * stC / sum_time;
}
void testShrinkageFPRate(FILE * wf)
{
    int test_num = 20;
    int size = 1<<14;
    int tCnt = size * 4;
    double lr = 0.9;
    int testSetCnt=(1<<16) * lr;

    set<string> si;
    set<string> sn;
    double _tp, _fr;

    double tp1,tp2,tp3;
    double fr1,fr2,fr3;
    int shrink=2;
    tp1=tp2=tp3=0;
    fr1=fr2=fr3=0;
    for(int r = 0; r < test_num; ++r)
    {
        si.clear();
        sn.clear();
        // original
        ECF *cR = new ECF(size, 4, BOB1, BOB2, BOB3);
        for(int i=0; i<tCnt*lr; i++)
        {
            cR->insert(ele[i]);
            si.insert(ele[i]);
        }
        testSingleResizePFRate(cR, si, sn, testSetCnt, _fr, _tp);
        tp1 += _tp/test_num;
        fr1 += _fr/test_num;

        // delete
        for(int i=0; i<tCnt*lr; i++)
        {
            if(i%shrink==0)
                continue;
            cR->del(ele[i]);
            sn.insert(ele[i]);
        }
        testSingleResizePFRate(cR, si, sn, testSetCnt/shrink, _fr, _tp);
        tp2 += _tp/test_num;
        fr2 += _fr/test_num;

        //shrink
        int tmp=1;
        while(tmp!=shrink)
        {
            tmp*=2;
            if(!(cR->resize(size/tmp)))
                fprintf(wf, "failure shrinkage%d\n", tmp);
        }
        testSingleResizePFRate(cR, si, sn, testSetCnt/shrink, _fr, _tp);
        tp3 += _tp/test_num;
        fr3 += _fr/test_num;


        delete cR;
    }
    fprintf(wf, "elastic\n");
    fprintf(wf, "tp: %.6f, %.6f, %.6f\n", tp1, tp2, tp3);
    fprintf(wf, "fr: %.6f, %.6f, %.6f\n", fr1, fr2, fr3); 
    fprintf(wf, "\n");
}

void testExtensionFPRate(FILE *wf) {
    int unitlen = 2048;
    int insertsize = 7373;

    ECF *cR;
    dynamicCuckoo *dcF;
    int cRfalse[4], dcFfalse[4];

    cR = new ECF(unitlen, 4, BOB1, BOB2, BOB3);
    dcF = new dynamicCuckoo(unitlen, 4, BOB1, BOB2);

    for(int i = 0; i < 4; i++) {

        int startidx = (i==0)? 0 : (1<<(i-1));
        int endidx = 1<<i;
        startidx *= insertsize;
        endidx *= insertsize;

        //cout << startidx << " " << endidx << endl; 

        int rcnt;

        int bias = (endidx - startidx) / 3;
        int teststart = 10000 * (i + 6);
        int testend = 10000 * (i + 7);

        //insert in cR
        for(int j = 0; j < 3; j++) {
            bool insert_OK = true;
            int biasstart = startidx + j*bias;
            int biasend = startidx + (j+1)*bias;
            for(int k = biasstart; k < biasend; k++) {
                if(!cR->insert(ele[k])) {
                    insert_OK = false;
                    break;
                }
            }
            cRfalse[i] = -1;
            if(insert_OK) {
                cRfalse[i] = 0;
                for(int k = teststart; k < testend; k++) {
                    if(cR->lookup(ele[k])) {
                        cRfalse[i]++;
                    }
                }
            }
            //cout << cRfalse[i] << " ";
            
        }
        rcnt = cR->rnum;
        for(int j = startidx; j < endidx; j++) {
            cR->lookup(ele[j]);
        }
        rcnt = cR->rnum - rcnt;
        //cout << rcnt << endl;

        //insert in dcF
        for(int j = 0; j < 3; j++) {
            bool insert_OK = true;
            int biasstart = startidx + j*bias;
            int biasend = startidx + (j+1)*bias;
            for(int k = biasstart; k < biasend; k++)
                if(!dcF->insert(ele[k])) {
                    insert_OK = false;
                    break;
            }
            dcFfalse[i] = -1;
            if(insert_OK) {
                
                dcFfalse[i] = 0;
                for(int k = teststart; k < testend; k++) {
                    if(dcF->lookup(ele[k])) {
                        dcFfalse[i]++;
                    }
                }
            }
            //cout << dcFfalse[i] << " ";
        }
        rcnt = dcF->Get_rnum();
        for(int j = startidx; j < endidx; j++) {
            dcF->lookup(ele[j]);
        }
        rcnt = dcF->Get_rnum() - rcnt;
        //cout << rcnt << endl;

        //cout << "print" << endl;
        fprintf(wf, "Insert %d elements\n", endidx - startidx);
        //ffprintf(wf, wf, "ECF time: %d\n", cRtime[i]);
        //ffprintf(wf, wf, "DCF time: %d\n", dcFtime[i]);
        fprintf(wf, "ECF false num: %d\n", cRfalse[i]);
        fprintf(wf, "DCF false num: %d\n", dcFfalse[i]);
        //ffprintf(wf, wf, "DCF size: %d %d\n", dcFsize1, dcFsize2);
        //cout << cRfalse[i] << " " << dcFfalse[i] << endl;
        //cout << rmcnt << endl;
        fprintf(wf, "\n");

        int newsize = unitlen * (1<<(i+1));
        cR->expand(newsize);

        cRfalse[i] = 0;
        for(int k = teststart; k < testend; k++) {
            if(cR->lookup(ele[k])) {
                cRfalse[i]++;
            }
        }
        //cout << cRfalse[i] << endl;
    }
}


int main()
{
    srand(time(NULL));
    if(!readDataset(false)) // true: caida dataset, false: synthetic dataset
        return(0);
    cout << "cuckoo size = " << max_num << '\n' << "insert num = " << cnt << '\n';

    FILE *wf = fopen("out.txt", "a");
    //testMaxInsertNumber(wf);
    //testInsertThp(wf);
    //testLookupThp(wf);
    //testLoadRatio(wf);
    //testMemoryAccess(wf);
    //testResizeLookupThp(wf);
    //testExtensionFPRate(wf);
    testShrinkageFPRate(wf);
    fclose(wf); 
    return(0);
}
