#include <windows.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <mutex>
#include <math.h>

#ifndef CONIO_H
#define CONIO_H
int getch( void );
int getche( void );
#endif // CONIO_H

using namespace std;
string FilePath = "../DataTest/Data/";

void readFile(long long int BufSize, string FilePath, long long int *FileSize, long int *Time, int *Count);
int main(int argc, char* argv[])
{
	FilePath.append(argv[1]);
	long long int BUFFER_SIZE = atoi(argv[2]);
	
	long long int FileSize = 0;
	long int Time = 0;
	HANDLE hFile;
	char* buf = NULL;
	chrono::time_point<chrono::steady_clock> cstart, cend;
	
	/*Read File*/
	hFile = CreateFile(FilePath.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING|FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		
	if (hFile == INVALID_HANDLE_VALUE){
		printf("CreateFile error!");
		exit(1);
	} 			
	
	LARGE_INTEGER lpFileSizeHigh;
	GetFileSizeEx(hFile, &lpFileSizeHigh);
	FileSize = lpFileSizeHigh.QuadPart;
	
	BOOL result;
	DWORD readSize;
			
	switch(BUFFER_SIZE){
		case 0:
			BUFFER_SIZE = FileSize;
			buf = (char*)VirtualAlloc(NULL, BUFFER_SIZE, MEM_COMMIT, PAGE_READWRITE);
			//resize buffersize (512*N)
			if(BUFFER_SIZE%512 != 0){
				int a = BUFFER_SIZE/512;
				BUFFER_SIZE = (a+1)*512;
			}		
			
			cstart = chrono::steady_clock::now();
			result = ReadFile(hFile, buf, BUFFER_SIZE, &readSize, NULL);
			if(result){}
			else{
					printf("ReadFile Error!!");
					exit(1);
			}
			SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
			CloseHandle(hFile);
			FlushFileBuffers(hFile);
			cend = chrono::steady_clock::now();
			Time = chrono::duration_cast<std::chrono::milliseconds> (cend - cstart).count();
		break;
		
		default:
			int Loop; 
			int i = 0;
			buf = (char*)VirtualAlloc(NULL, BUFFER_SIZE, MEM_COMMIT, PAGE_READWRITE);
			
			Loop = (unsigned int)FileSize/(BUFFER_SIZE);
			if(FileSize%BUFFER_SIZE != 0)Loop++;
	
			cstart = chrono::steady_clock::now();
			for(i = 0; i < Loop; i++){
				result = ReadFile(hFile, buf, BUFFER_SIZE, &readSize, NULL);
				if(result){}
				else{
					printf("ReadFile Error!! \ni = %d\n", i);
					exit(1);
				}
			}
			SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
			CloseHandle(hFile);
			FlushFileBuffers(hFile);
			cend = chrono::steady_clock::now();
			Time = chrono::duration_cast<std::chrono::milliseconds> (cend - cstart).count();
		break;
	}
	
	printf("FileSize : %lld\n",FileSize);
	printf("BufSize : %lld\n",BUFFER_SIZE);
	printf("\nTime: %ld ms\n", Time);
	
	
	
	return 0;
}
