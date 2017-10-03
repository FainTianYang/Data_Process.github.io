/*測試不同Buffer的讀檔速度(C++)*/
/*./<exe> <檔名> <Buffer Size>*/
/*<Buffer Size>填0 : 一次讀整個檔案*/
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <mutex>

#ifndef CONIO_H
#define CONIO_H
int getch( void );
int getche( void );
#endif // CONIO_H

using namespace std;

string strFileName = "../DataTest/Data/";

unsigned int BUFFER_SIZE = 16;
int alltime = 0;
int SubNum = 0;

int main(int argc, char* argv[])
{
	strFileName.append(argv[1]);
	BUFFER_SIZE = atoi(argv[2]);
	/*Read*/
	char* buffer;
	char* bufferCopy;
	ifstream ifs ;
	chrono::time_point<chrono::steady_clock> start, end;
	
	ifs.open(strFileName.c_str(), ifstream::in | ifstream::binary);

	filebuf* pbuf = ifs.rdbuf();

	size_t size = pbuf->pubseekoff (0,ifs.end,ifs.in);
	pbuf->pubseekpos (0,ifs.in);
	
	switch(BUFFER_SIZE){
		case 0: //一次讀整個檔案
			printf("Hold File\n");
			BUFFER_SIZE = size;
			buffer=new char[BUFFER_SIZE];
	
			start = chrono::steady_clock::now();
			
			pbuf->sgetn (buffer,BUFFER_SIZE);
			if(!ifs){
				printf("error");
				exit(1);
			}
				
			ifs.close();
			end = chrono::steady_clock::now();
			printf("\nFile Colse : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
			
		break;
		
		default:
			buffer=new char[BUFFER_SIZE];
	
			if((unsigned int)size%BUFFER_SIZE == 0) SubNum = (unsigned int)size/BUFFER_SIZE;
			else SubNum = (unsigned int)size/BUFFER_SIZE + 1;
			
			start = chrono::steady_clock::now();
			
			for(int i=0; i<SubNum; i++){
				if(i==SubNum-1) memset(buffer, 0, BUFFER_SIZE);
				pbuf->pubseekpos(i*BUFFER_SIZE);
				pbuf->sgetn (buffer,BUFFER_SIZE);
				/*printf("\ni = %d  :  %s", i, buffer);*/
				if(!ifs){
					printf("error");
					exit(1);
				}
			}
			
			ifs.ignore(std::numeric_limits<std::streamsize>::max());
			end = chrono::steady_clock::now();
			printf("\nFile Colse : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
		break;
	}
	
	/*Read finish*/
	printf("size : %zu\n", size);
	printf("BUFFER_SIZE : %u\n", BUFFER_SIZE);
	printf("SubNum : %d\n", SubNum);
	
    return 0;
}