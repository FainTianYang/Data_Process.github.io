/*Hold Read & Hold Write*/
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
string OutPath = "../DataTest/Data/Win";

void readFile(long long int BufSize, string FilePath, long long int *FileSize, long int *Time, int *Count);
int main(int argc, char* argv[])
{
	
	FilePath.append(argv[1]);
	OutPath.append(argv[1]);
	long long int BufSize = 0;
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
	BufSize = lpFileSizeHigh.QuadPart;
	
	printf("BufSize : %lld\n",BufSize);
	buf = (char*)VirtualAlloc(NULL, BufSize, MEM_COMMIT, PAGE_READWRITE);
	//resize buffersize (512*N)
	if(BufSize%512 != 0){
		int a = BufSize/512;
		BufSize = (a+1)*512;
	}		
	printf("BufSize : %lld\n",BufSize);
	
	BOOL result;
	DWORD readSize;
	
	cstart = chrono::steady_clock::now();
	//Read
	result = ReadFile(hFile, buf, BufSize, &readSize, NULL);
	if(result){}
	else{
			printf("ReadFile Error!!");
			exit(1);
	}
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	FlushFileBuffers(hFile);
	CloseHandle(hFile);
	cend = chrono::steady_clock::now();
	
	Time = chrono::duration_cast<std::chrono::milliseconds> (cend - cstart).count();
	printf("\nTime: %ld ms\n", Time);
	
	/*Write*/
	hFile = ::CreateFile(OutPath.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING|FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	USHORT lpInBuffer = COMPRESSION_FORMAT_NONE;
	DWORD lpBytesReturned = 0;
	DeviceIoControl(hFile, FSCTL_SET_COMPRESSION, (LPVOID) &lpInBuffer,
				sizeof(USHORT), NULL, 0, (LPDWORD)&lpBytesReturned, NULL);

	cstart = chrono::steady_clock::now();
	DWORD writeSize;
	result = WriteFile(hFile, buf, BufSize, &writeSize, NULL);
	if(result){}
	else{
			printf("WriteFile Error!!");
			exit(1);
	}
	FlushFileBuffers(hFile);
	CloseHandle(hFile);
	cend = chrono::steady_clock::now();
	Time = chrono::duration_cast<std::chrono::milliseconds> (cend - cstart).count();
	printf("\nTime: %ld ms\n", Time);
	VirtualFree(buf, BufSize, MEM_DECOMMIT);
	
	return 0;
}
