#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <mutex>
#include <windows.h> 
#include <stdlib.h>

#ifndef CONIO_H
#define CONIO_H
int getch( void );
int getche( void );
#endif // CONIO_H

using namespace std;

string strFileName = "../DataTest/Data/";

unsigned int BUFFER_SIZE = 12;
unsigned int FILE_SIZE = 12;
LARGE_INTEGER lpFileSizeHigh;
int alltime = 0;
int SubNum = 0;
int count = 0;
VOID CALLBACK MyFileIOCompletionRoutine(  
  DWORD dwErrorCode,                // 對於此次操作返回的狀態  
  DWORD dwNumberOfBytesTransfered,  // 告訴已經操作了多少位元組,也就是在IRP裡的Information  
  LPOVERLAPPED lpOverlapped         // 這個資料結構  
)  
{  
    count++;
}  
int main(int argc, char* argv[])
{
	strFileName.append(argv[1]);
	BUFFER_SIZE = atoi(argv[2]);
	/*Read*/
	char* buffer=new char[BUFFER_SIZE];
	
	OVERLAPPED ol = {0};
	
    HANDLE hFile = CreateFile(strFileName.c_str(),               // file to open
                       GENERIC_READ,          // open for reading
                       FILE_SHARE_READ,       // share for reading
                       NULL,                  // default security
                       OPEN_EXISTING,         // existing file only
                       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
                       NULL);      
	GetFileSizeEx(hFile, &lpFileSizeHigh);
	FILE_SIZE = lpFileSizeHigh.QuadPart;

	SubNum = (unsigned int)FILE_SIZE/BUFFER_SIZE;
	if(FILE_SIZE%BUFFER_SIZE!=0)SubNum++;
	
	chrono::time_point<chrono::steady_clock> start, end;
	start = chrono::steady_clock::now();
	LARGE_INTEGER pointer;
	DWORD readSize;
	for(int i=0; i<SubNum; i++){
		if(i==SubNum-1) memset(buffer, 0, BUFFER_SIZE);
		pointer.QuadPart = i*BUFFER_SIZE;
		ReadFile(hFile, buffer, FILE_SIZE, &readSize, NULL);
		/*if(SetFilePointerEx(hFile, pointer, NULL, FILE_BEGIN)){
			if(ReadFile(hFile, buffer, BUFFER_SIZE, &readSize, NULL)){
			}
			else{
				exit(1);
				printf("Error\n");
			}
		}
		else{
			exit(1);
			printf("Error\n");
			
		}*/
	}
	cout<<buffer<<endl;
	end = chrono::steady_clock::now();
	printf("Count : %d", count);
	FlushFileBuffers(hFile);
	CloseHandle(hFile);
	/*Read finish*/
	printf("size : %u\n", FILE_SIZE);
	printf("BUFFER_SIZE : %u\n", BUFFER_SIZE);
	printf("SubNum : %d\n", SubNum);
	printf("\nFile Colse : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
    
	delete[] buffer;
	return 0;
}