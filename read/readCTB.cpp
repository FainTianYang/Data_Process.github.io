#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <mutex>

using namespace std;

int main()
{
	chrono::time_point<chrono::steady_clock> start, end;
	start = chrono::steady_clock::now();
	
	ifstream ifs ;
	ifs.open("/home/Data/2000Sub.csv", ifstream::in | ifstream::binary);

	filebuf* pbuf = ifs.rdbuf();

	size_t size = pbuf->pubseekoff (0,ifs.end,ifs.in);
	pbuf->pubseekpos (0,ifs.in);
			
	char* buffer=new char[size];

	pbuf->sgetn (buffer,size);
	ifs.close();
	
	end = chrono::steady_clock::now();
	chrono::duration<double> elapsed_seconds = end-start;
	printf("\nFile Colse : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
	
	start = chrono::steady_clock::now();
	
	char* bufferCopy=new char[size];
	memcpy(bufferCopy, buffer, size);
	
	end = chrono::steady_clock::now();
	elapsed_seconds = end-start;
	printf("\nFile Copy : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
	
    return 0;
}
