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
string FilePathOut = "../Re_Data/Win";

void readFile(long long int BufSize, string FilePath, long long int *FileSize, long int *Time, int *Count);
int main(int argc, char* argv[])
{
	FilePath.append(argv[1]);
	FilePathOut.append(argv[1]);
	long long int BUFFER_SIZE_OUT = atoi(argv[2]);
	long long int BUFFER_SIZE_IN = 0;
	
	long long int FILE_SIZE = 0;
	long int Time = 0;
	HANDLE hFile;
	char* buf = NULL;
	chrono::time_point<chrono::steady_clock> start, end;
	
	/*----- Read Hold File -----*/
	hFile = CreateFile(FilePath.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING|FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		
	if (hFile == INVALID_HANDLE_VALUE){
		printf("CreateFile error!");
		exit(1);
	} 			
	
	LARGE_INTEGER lpFileSizeHigh;
	GetFileSizeEx(hFile, &lpFileSizeHigh);
	FILE_SIZE = lpFileSizeHigh.QuadPart;
	
	printf("FILE_SIZE : %lld\n",FILE_SIZE);
	
	//resize buffersize to 512*N
	if(FILE_SIZE%512 != 0){
		int a = FILE_SIZE/512;
		BUFFER_SIZE_IN = (a+1)*512;
	}
	else BUFFER_SIZE_IN = FILE_SIZE;
	buf = (char*)VirtualAlloc(NULL, BUFFER_SIZE_IN, MEM_COMMIT, PAGE_READWRITE);	
	printf("BUFFER_SIZE_IN : %lld\n",BUFFER_SIZE_IN);
	
	BOOL result;
	DWORD readSize;
	start = chrono::steady_clock::now();
	result = ReadFile(hFile, buf, BUFFER_SIZE_IN, &readSize, NULL);
	if(result){}
	else{
			printf("ReadFile Error!!");
			exit(1);
	}
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	CloseHandle(hFile);
	FlushFileBuffers(hFile);
	end = chrono::steady_clock::now();
	
	Time = chrono::duration_cast<std::chrono::milliseconds> (end - start).count();
	printf("Read Time: %ld ms\n", Time);
	
	/*----- Write -----*/
	hFile = ::CreateFile(FilePathOut.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING|FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		printf("CreateFile error! (Write)");
		exit(1);
	} 
	
	USHORT lpInBuffer = COMPRESSION_FORMAT_NONE;
	DWORD lpBytesReturned = 0;
	DeviceIoControl(hFile, FSCTL_SET_COMPRESSION, (LPVOID) &lpInBuffer,
				sizeof(USHORT), NULL, 0, (LPDWORD)&lpBytesReturned, NULL);

	DWORD writeSize;
			
	switch(BUFFER_SIZE_OUT){
		case 0:
			BUFFER_SIZE_OUT = BUFFER_SIZE_IN;
			
			start = chrono::steady_clock::now();
			result = WriteFile(hFile, buf, BUFFER_SIZE_OUT, &writeSize, NULL);
			if(result){}
			else{
					printf("Write File Error!!");
					exit(1);
			}
			SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
			CloseHandle(hFile);
			FlushFileBuffers(hFile);
			end = chrono::steady_clock::now();
			Time = chrono::duration_cast<std::chrono::milliseconds> (end - start).count();
		break;
		
		default:
			int Loop; 
			int i = 0;
			
			Loop = (unsigned int)FILE_SIZE/BUFFER_SIZE_OUT;
			if(FILE_SIZE%BUFFER_SIZE_OUT != 0)Loop++;
	
			start = chrono::steady_clock::now();
			for(i = 0; i < Loop; i++){
				result = WriteFile(hFile, buf, BUFFER_SIZE_OUT, &writeSize, NULL);
				if(result){}
				else{
					printf("Write File Error!! \ni = %d\n", i);
					exit(1);
				}
			}
			SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
			CloseHandle(hFile);
			FlushFileBuffers(hFile);
			end = chrono::steady_clock::now();
			Time = chrono::duration_cast<std::chrono::milliseconds> (end - start).count();
		break;
	}
	printf("BUFFER_SIZE_OUT : %lld\n",BUFFER_SIZE_OUT);
	printf("Write Time: %ld ms\n", Time);
	VirtualFree(buf, BUFFER_SIZE_IN, MEM_DECOMMIT);
	return 0;
}
