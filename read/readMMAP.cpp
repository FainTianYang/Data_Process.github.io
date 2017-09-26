#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <mutex>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h> //system defind file stat
#include <errno.h>


using namespace std;

int main(int argc, char* argv[]){
	int fd; //open() return
	int offset; 
	char* data;
	struct stat sbuf;
	
	
	if((fd = open("/home/Data/2000Sub.csv", O_RDONLY)) == -1){
		perror("open");
		exit(1);
	}
	
	if(stat("/home/Data/1999Sub.csv", &sbuf) == -1){
		perror("stat");
		exit(1);
	}
	chrono::time_point<chrono::steady_clock> start, end;
	start = chrono::steady_clock::now();
	
	data = (char*)mmap((caddr_t)0, sbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);

	end = chrono::steady_clock::now();
	chrono::duration<double> elapsed_seconds = end-start;
	printf("\nFile Colse : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

	if(data == (caddr_t)(-1)){
		perror("mmap");
		exit(1);
	}
	cout<<"sbuf.st_size "<<sbuf.st_size<<endl;
	start = chrono::steady_clock::now();
	
	char* bufferCopy=new char[sbuf.st_size];
	memcpy(bufferCopy, data, sbuf.st_size);
	
	end = chrono::steady_clock::now();
	elapsed_seconds = end-start;
	printf("\nFile Copy : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
	

	return 0;

}
