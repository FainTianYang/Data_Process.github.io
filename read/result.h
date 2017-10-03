/*---------------------------------------------------------------------------------*/
/*測試開檔用二進制或是一般字串流*/
//PS.任何檔案都是以二進制的方式儲存,以下差別在你想用哪種方式操作檔案
//ios::binary的話就是不先做任何轉換直接以二進制的方式讀入
/*---結果---*/
Data : 1996_2008Sub.csv 5112230KB(4992.4121MB = 4.8754GB)
windows
<1> ifs.open("../DataTest/Data/1996_2008Sub.csv", ifstream::in| ios::binary);
	$ time ./rB
	939955706
	real    0m57.036s
	user    0m42.447s
	sys     0m5.382s
Speed =  87.531(MB/s)
<2> ifs.open("../DataTest/Data/1996_2008Sub.csv", ifstream::in);
	$ time ./rB
	939955706
	real    0m57.732s
	user    0m41.714s
	sys     0m5.490s
Speed =  86.475682(MB/s)
/*---結論---*/
時間一樣
/*---Code---*/
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

int main( int argc, char** argv ){
	
	ifstream ifs;
	char ch; 
	//ifs.open("../DataTest/Data/1996_2008Sub.csv", ifstream::in| ios::binary);
	//ifs.open("../DataTest/Data/1996_2008Sub.csv", ifstream::in);
	int count = 0;
	if(ifs){ 
		while(!ifs.eof()) { 
			ifs.get(ch); 
			count++;
		}
	}
	else{
		printf("File not find!!\n");
	}
	ifs.close();
	cout<<count<<endl;
	return 0;
}

/*---------------------------------------------------------------------------------*/
/*測試整塊讀入(filebuf)*/
//為了確保他會讀進記憶體,所以在讀入後做複製
/*---結果---*/
windows
Data : 2000Sub.csv 360432KB(351.984375MB)
output :
	$ time ./rB
	File Colse : 4159 ms
	File Copy : 110 ms
	real    0m4.315s
	user    0m0.031s
	sys     0m0.373s
Speed =  81.57227(MB/s)

Data : 1996_2008Sub.csv 5112230KB(4992.4121MB = 4.8754GB)
output :
	$ time ./rB
	File Colse : 14348 ms
	File Copy : 166631 ms
	real    4m4.764s
	user    0m1.044s
	sys     0m6.754s
Speed =  347.951(MB/s)

Linux
Data : 2000Sub.csv 360432KB(351.984375MB)
output :
	$ time ./rB
	File Colse : 13989 ms
	File Copy : 8647 ms
	real    0m22.645s
	user    0m0.151s
	sys     0m22.072s
Speed =  25.16153(MB/s)

/*---No Copy--*/
windows
Data : 2000Sub.csv 360432KB(351.984375MB)
output :
	$ time ./rB
	File Colse : 4177 ms
	real    0m4.226s
	user    0m0.000s
	sys     0m0.265s
Speed =  86.2897(MB/s)

Data : 1996_2008Sub.csv 5112230KB(4992.4121MB = 4.8754GB)
output :
	$ time ./rB
	File Colse : 9609 ms
	real    0m9.647s
	user    0m0.000s
	sys     0m0.998s
Speed =  519.555843(MB/s)

/*---Code---*/
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

int main()
{
	chrono::time_point<chrono::steady_clock> start, end;
	start = chrono::steady_clock::now();
	/*Read*/
	ifstream ifs ;
	ifs.open("../DataTest/Data/2000Sub.csv", ifstream::in | ifstream::binary);

	filebuf* pbuf = ifs.rdbuf();

	size_t size = pbuf->pubseekoff (0,ifs.end,ifs.in);
	pbuf->pubseekpos (0,ifs.in);
			
	char* buffer=new char[size];

	pbuf->sgetn (buffer,size);
	ifs.close();
	/*Read finish*/
	end = chrono::steady_clock::now();
	chrono::duration<double> elapsed_seconds = end-start;
	printf("\nFile Colse : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
	
	start = chrono::steady_clock::now();
	/*Copy Data*/
	char* bufferCopy=new char[size];
	memcpy(bufferCopy, buffer, size);
	
	end = chrono::steady_clock::now();
	elapsed_seconds = end-start;
	printf("\nFile Copy : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
	
    return 0;
}

/*---------------------------------------------------------------------------------*/
/*使用mmap測試*/
//由於mmap是直接映射到Disk位址上,所以要使用到Data時才會讀
//因此複製時才有花有時間
/*---結果---*/
Linux
Data : 2000Sub.csv 360432KB(351.984375MB)
output :
	File Close: 0 ms
	sbuf.st_size 358084630
	File Copy : 5473 ms
	real    0m5.539s
	user    0m0.163s
	sys     0m0.591s
Speed =  64.3129(MB/s)
/*---Code---*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h> //system defind file stat
#include <errno.h>
#include <chrono>

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
