#include <iostream>
#include <time.h>
#include <random>
#include <set>
#include <cstring>
#include <string>
#include <algorithm>
using namespace std;
set<string> s;
int main(){

	FILE *wf = fopen("syn_out.txt", "w");
    
    int syn_num = 1<<21;

	srand(time(NULL));
    unsigned char ch[13];
    int cnt = 0;
    for(;;){
    	for(int i = 0; i < 13; ++i)
    		ch[i] = rand() % 256;
    	string str((char *)ch);
    	if(!s.count(str)){
    		s.insert(str);
    		cnt++;
	    	for(int i = 0; i < 13; ++i)
	    		fprintf(wf, "%c", ch[i]);
    		if(cnt == syn_num)
    			break;
    	}
    }
    fclose(wf);

    // char five_tuple[5];
    // FILE *fin = fopen("syn_out.txt", "rb");
    // cnt = 0; 
    // while(fread((void *)five_tuple, sizeof(char), 4, fin) == 4){
    // 	for(int i = 0; i < 4; ++i)
    // 		cout << (int)five_tuple[i] << ' ';
    // 	cnt++;
    // 	cout << '\n';
    // }
    // cout << cnt;
    // fclose(fin);
}
