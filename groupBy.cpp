#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <future>
#include <memory>
#include <vector>
#include <algorithm>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <windows.h>  

#ifndef CONIO_H
#define CONIO_H
int getch( void );
int getche( void );
#endif // CONIO_H

using namespace std;

#define COLUMN_WIDTH 7
#define COLUMN_NUMBER 14
const int THREAD_NUM  = 1;
const int SEP_SIZE = 1;

class Data{
public:
	string strFilePath;
	vector<vector<float>> vvfContent;
	
	Data(string str){
		strFilePath = str;
		Read();
	}
	~Data(){
		printf("~Data -- %s \n", this->strFilePath.c_str());
	}
private:
	size_t lliFileSize = 0;
	long long int lliSubBufferSize;
	long long int lldarrStarPos[THREAD_NUM];
	int readCount = 0;
	ifstream ifs;
	filebuf* pbuf;
	mutex mutex_viContent;
	mutex mutex_pbuf;
	
	void Read();
	int ThreadRead(int);
};

void Data::Read(){
	int i=0;
	chrono::time_point<chrono::steady_clock> start, end;
	
	/*----- Read file -----*/
	ifs.open(strFilePath.c_str(), ifstream::in);
	if(!ifs){
		printf("File Open Error !\n");
		exit(1);
	}
	pbuf = ifs.rdbuf();
	
	lliFileSize = pbuf->pubseekoff (0,ifs.end,ifs.in);
	pbuf->pubseekpos (0,ifs.in);
	printf("size_t : %zu\n", lliFileSize);
	
	int iLineNum = (lliFileSize/THREAD_NUM)/(COLUMN_WIDTH*COLUMN_NUMBER + SEP_SIZE) + 1; //COLUMN_WIDTH*COLUMN_NUMBER "+1" : 是\n
	cout<<"iLineNum : "<<iLineNum<<endl;
	lliSubBufferSize = iLineNum*(COLUMN_WIDTH*COLUMN_NUMBER + SEP_SIZE);
	cout<<"lliSubBufferSize : "<<lliSubBufferSize<<endl;
	
	for(i=0; i<THREAD_NUM; i++){
		lldarrStarPos[i] = i*lliSubBufferSize;
	}
	
	/*----- Split Input Data(vcharBuffer)-----*/
	start = chrono::steady_clock::now();
	vector<future<int>> threads;
	for(i=0; i<THREAD_NUM; i++){
		threads.push_back(async(launch::async, &Data::ThreadRead, this, i ));
	}
	int aa = 0;
	for(i=0; i<THREAD_NUM; i++){
		aa+=threads[i].get();
	}
	cout<<"aa "<<aa<<endl;
	end = chrono::steady_clock::now();
	printf("\nThread : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
	cout<<"vvfContent : "<<vvfContent.size()<<endl;
	for(int i=0;i<10;i++){
		for(int j=0;j<vvfContent[i].size(); j++){
			cout<<vvfContent[i][j]<<" , ";
		}
		cout<<endl;
	}
}
int Data::ThreadRead(int ind){
	char* buffer = new char[lliSubBufferSize+1];
	long long int lliBufferSizeThread = lliSubBufferSize;
	chrono::time_point<chrono::steady_clock> start, end;
	{
		unique_lock<mutex> lock_pbuf(mutex_pbuf);
		start = chrono::steady_clock::now();
		
		pbuf->pubseekpos(lldarrStarPos[ind]);
		
		if(ind == THREAD_NUM-1 && THREAD_NUM!=1) {
			lliBufferSizeThread = lliFileSize - (THREAD_NUM-1)*lliSubBufferSize;
		}
		
		pbuf->sgetn (buffer,lliBufferSizeThread);
		if(!ifs){
			printf("error");
			exit(1);
		}
		buffer[lliBufferSizeThread] = 0;
		readCount++;
		end = chrono::steady_clock::now();
		printf("\nRead Data : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
		int iCupId = GetCurrentProcessorNumber(); //current cpuID which this thread running 
		printf("CPU Ind : %d\n", iCupId);
		if(readCount==THREAD_NUM) ifs.close();
		lock_pbuf.unlock();
	}
	
	start = chrono::steady_clock::now();
	int pos=0;
	char *sub = new char[COLUMN_WIDTH+1];
	vector<vector<float>> vvf;
	vector<float> v(COLUMN_NUMBER);
	char *pch = strtok(buffer,"\n");
	while(pch){
		//cout<<pch<<endl;
		pos=0;
		for(int i=0;i<COLUMN_NUMBER;i++){
			memcpy(sub, pch+pos, COLUMN_WIDTH);
			pos+=COLUMN_WIDTH;
			sub[COLUMN_WIDTH] = 0;
			v[i] = atof(sub);
		}
		vvf.push_back(v);
		pch = strtok(NULL,"\n");
	}
	delete[] buffer;
	int a = vvf.size();
	cout<<"a"<<a<<endl;
	end = chrono::steady_clock::now();
	printf("Split Data : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
	
	{
		unique_lock<mutex> lock_m(mutex_viContent);
		vvfContent.insert(vvfContent.end(), vvf.begin(), vvf.end());
		lock_m.unlock();
	}
	
	return a;
	
}
int main(){
	unique_ptr<Data> dataptr2008(new Data("../DataTest/Data/1996_1999Fix.txt"));
	
	
	return 0;
}