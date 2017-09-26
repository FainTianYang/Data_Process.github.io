#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h> //system defind file stat
#include <errno.h>
#include <chrono>
#include <vector>

using namespace std;

int main(){
	chrono::time_point<chrono::steady_clock> tpStart, tpEnd;
	tpStart = chrono::steady_clock::now();

	vector< vector<float> > vContent; //file content
	int fd; //open() return
	char* data;
	struct stat sbuf;
	
	
	if((fd = open("/home/Data/1996_2008Sub.csv", O_RDONLY)) == -1){
		perror("open");
		exit(1);
	}
	
	if(stat("/home/Data/1996_2008Sub.csv", &sbuf) == -1){
		perror("stat");
		exit(1);
	}
	
	data = (char*)mmap((caddr_t)0, sbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
	
	if(data == (caddr_t)(-1)){
		perror("mmap");
		exit(1);
	}
	int countW = 0;
	int start = 0;
	int end = 0;
	char dataSub[10] = "";
	vector<float> vEmpty;
	int iRow = 0;
	
	vContent.push_back(vEmpty);
	
	for(long long offset=0; offset<sbuf.st_size-1; offset++){
	
		if(data[offset] == ','){
			vContent[iRow].push_back(atof(dataSub));
			dataSub[0] = '\0';
			countW++;
		}
		else if(data[offset] == '\n' || offset == sbuf.st_size){
			vContent[iRow].push_back(atof(dataSub));
			vContent.push_back(vEmpty);
			iRow++;
			dataSub[0] = '\0';
			countW++;
		}
		else{
			char c[2] = {data[offset], '\0'};
			strcat (dataSub, c);
		}
	}
	vContent[iRow].push_back(atof(dataSub));
	vContent.push_back(vEmpty);
	iRow++;
	dataSub[0] = '\0';
	countW++;
	
	tpEnd = chrono::steady_clock::now();
	printf("size : %ld\n", sbuf.st_size - 1);
	printf("block size : %ld\nblock number : %ld\n", sbuf.st_blksize, sbuf.st_blocks);
	printf("vector size : %ld\n", vContent.size());
	/*---------- Show info ----------*/
	printf("Time : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds>(tpEnd - tpStart).count());
	printf("countW : %d", countW);
	for(int i=0; i<10; i++){
		printf("Index %d :", i);
		for(int j=0; j<vContent[i].size(); j++){
			printf(" %f ,", vContent[i][j]);
		}
		printf("\n");
	}
	
	return 0;

}
