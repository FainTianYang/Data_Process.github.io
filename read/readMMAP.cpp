#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h> //system defind file stat
#include <errno.h>

int main(int argc, char* argv[]){
	int fd; //open() return
	int offset; 
	char* data;
	struct stat sbuf;
	
	/*
	if(argc != 2){
		fprintf(stderr, "usage : mmapdemo offset\n");
		exit(1);
	}*/
	
	if((fd = open("/home/Data/TestNum", O_RDONLY)) == -1){
		perror("open");
		exit(1);
	}
	
	if(stat("/home/Data/TestNum", &sbuf) == -1){
		perror("stat");
		exit(1);
	}
	
	/*offset = atoi(argv[1]);
	
	if(offset < 0 || offset > sbuf.st_size - 1){
		fprintf(stderr, "testMMAP : offset must be in the range 0 ~ %ld \n", sbuf.st_size - 1);
		exit(1);
	}*/
	
	//char *data = (char*)malloc(offset);
	data = (char*)mmap((caddr_t)0, sbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
	
	
	if(data == (caddr_t)(-1)){
		perror("mmap");
		exit(1);
	}
	
	for(int i=0; i<sbuf.st_size - 1; i++){
		printf("offeset %d is '%c' \n", i, data[i]);
	}
	//printf("byte at offeset %d is '%c' \n", offset, data[offset]);
	printf("size : %ld\n", sbuf.st_size - 1);
	printf("block size : %ld\nblock number : %ld\n", sbuf.st_blksize, sbuf.st_blocks);
	//printf("data is : \n %s \n", data);
	
	return 0;

}
