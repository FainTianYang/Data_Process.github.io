/*測試不同Buffer的讀檔速度(C++)*/
/*./<exe> <檔名> <Buffer Size>*/
/*<Buffer Size>填0 : 一次讀整個檔案*/
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <mutex>

#ifndef CONIO_H
#define CONIO_H
int getch( void );
int getche( void );
#endif // CONIO_H

using namespace std;

string strFileName = "../DataTest/Data/";
string strFileNameOut = "../Re_Data/C";

unsigned long long int BUFFER_SIZE = 16;
int alltime = 0;
int SubNum = 0;

void print_state (const std::ios& stream) {
  std::cout << " good()=" << stream.good();
  std::cout << " eof()=" << stream.eof();
  std::cout << " fail()=" << stream.fail();
  std::cout << " bad()=" << stream.bad();
}

int main(int argc, char* argv[])
{
	strFileName.append(argv[1]);
	strFileNameOut.append(argv[1]);
	
	/*----- Read -----*/
	char* buffer;
	ifstream ifs ;
	chrono::time_point<chrono::steady_clock> start, end;
	
	ifs.open(strFileName.c_str(), ios::in | ios::binary);

	filebuf* pbuf = ifs.rdbuf();

	size_t size = pbuf->pubseekoff (0,ifs.end,ifs.in);
	pbuf->pubseekpos (0,ifs.in);
	
	BUFFER_SIZE = size;
	buffer=new char[BUFFER_SIZE];
	printf("BUFFER_SIZE : %llu\n", BUFFER_SIZE);
	
	start = chrono::steady_clock::now();
	pbuf->sgetn (buffer,BUFFER_SIZE);
	if(!ifs){
		printf("error");
		exit(1);
	}		
	ifs.close();
	end = chrono::steady_clock::now();
	printf("\nFile Colse : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
	
	/*----- Write -----*/
	ofstream ofs;
	ofs.open(strFileNameOut.c_str(), ios::out | ios::binary | ios::trunc);
	
	start = chrono::steady_clock::now();
	ofs.write (buffer, BUFFER_SIZE);
	if(!ofs){
		printf("Write Error!!\n");
		print_state(ofs);
	}
	ofs.flush();
	end = chrono::steady_clock::now();
	printf("\nFile Colse : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
	
	delete[] buffer;
	ofs.close();
	
    return 0;
}