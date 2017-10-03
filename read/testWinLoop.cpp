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

void readFile(long long int BufSize, string FilePath, long long int *FileSize, long int *Time, int *Count);
int main(int argc, char* argv[])
{
	int j=0;
	int m=0;
	int runNum = 3;
	string FilePath = "../DataTest/Data/1999Sub.csv";
	long long int fileSize = 0;
	long int time = 0;
	long int timeAll = 0;
	int count = 0;
	long long int b = 1024;
	
	printf("File Path : %s", FilePath.c_str());
	/*
	for(j=0; j<=10; j++){
		long long int b = pow(2, 9+j);
		timeAll = 0;
		printf("\nBufSize : %f KB\n", b/1024.0);
		printf("Time : ");
		
		for(m=0; m<runNum; m++){
			readFile(b, FilePath, &fileSize, &time, &count);
			timeAll += time;
			printf("%ld , ", time);
		}
		printf("\nSum Time: %ld ms , Average Time : %lf ms\n", timeAll, (double)timeAll/runNum);
		printf("Speed : %lf MB/s\n", ((double)fileSize/1024.0/1024.0)/((double)timeAll/runNum/1000.0));
	}
	printf("FileSize : %lld KB\n", fileSize/1024);
	printf("count : %d\n", count);
	*/
	
	for(j=0; j<=10; j++){
		long long int b = pow(2, 17+j);
		timeAll = 0;
		printf("\nBufSize : %f KB\n", b/1024.0);
		printf("Time : ");
		
		for(m=0; m<runNum; m++){
			readFile(b, FilePath, &fileSize, &time, &count);
			timeAll += time;
			printf("%ld , ", time);
		}
		printf("\nSum Time: %ld ms , Average Time : %lf ms\n", timeAll, (double)timeAll/runNum);
		printf("Speed : %lf MB/s\n", ((double)fileSize/1024.0/1024.0)/((double)timeAll/runNum/1000.0));
	}
	printf("FileSize : %lld KB\n", fileSize/1024);
	printf("count : %d\n", count);
	
	return 0;
}
void readFile(long long int BufSize, string FilePath, long long int *FileSize, long int *Time, int *Count){
	*FileSize = 0;
	HANDLE hFile;
	int Loop = 0;
	char* buf = NULL;
	chrono::time_point<chrono::steady_clock> cstart, cend;

	buf = (char*)VirtualAlloc(NULL, BufSize, MEM_COMMIT, PAGE_READWRITE);
	
	hFile = CreateFile(FilePath.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING|FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		
	if (hFile == INVALID_HANDLE_VALUE){
		printf("CreateFile error!");
		exit(1);
	} 			
		
	LARGE_INTEGER lpFileSizeHigh;
	GetFileSizeEx(hFile, &lpFileSizeHigh);
	*FileSize = lpFileSizeHigh.QuadPart;
		
	Loop = (unsigned int)*FileSize/(BufSize);
	if(*FileSize%BufSize != 0)Loop++;
		
	int i = 0;
	*Count = 0;
		
	BOOL result;
	DWORD readSize;
		
	cstart = chrono::steady_clock::now();
	for(i = 0; i < Loop; i++){
		result = ReadFile(hFile, buf, BufSize, &readSize, NULL);
		if(result) *Count = *Count + 1;
		else{
			printf("ReadFile Error!! \ni = %d\n", i);
			exit(1);
		}
	}
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	CloseHandle(hFile);
	cend = chrono::steady_clock::now();
	FlushFileBuffers(hFile);
	VirtualFree(buf, BufSize, MEM_DECOMMIT);
	
	*Time = chrono::duration_cast<std::chrono::milliseconds> (cend - cstart).count();
}