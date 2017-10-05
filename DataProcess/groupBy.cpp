#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <memory>
#include <vector>
#include <algorithm>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#ifndef CONIO_H
#define CONIO_H
int getch( void );
int getche( void );
#endif // CONIO_H

using namespace std;

#define COLUMN_WIDTH 7
#define COLUMN_NUMBER 14
const int THREAD_NUM  = 1;

class Data{
public:
	string strFilePath;
	vector<vector<float>> vvfContent;
	mutex mutex_viContent;
	Data(string str){
		strFilePath = str;
		Read();
	}
	~Data(){
		printf("~Data -- %s \n", this->strFilePath.c_str());
	}
private:
	size_t lliFileSize = 0;
	long long int lldarrStarPos[THREAD_NUM];
	vector<char*> vcharBuffer;
	void Read();
	void ThreadRead(int);
};

void Data::Read(){
	int i=0;
	chrono::time_point<chrono::steady_clock> start, end;
	
	/*----- Read file -----*/
	ifstream ifs;
	ifs.open(strFilePath.c_str(), ifstream::in);
	if(!ifs){
		printf("File Open Error !\n");
		exit(1);
	}
	filebuf* pbuf = ifs.rdbuf();
	
	lliFileSize = pbuf->pubseekoff (0,ifs.end,ifs.in);
	pbuf->pubseekpos (0,ifs.in);
	printf("size_t : %zu\n", lliFileSize);
	
	int iLineNum = (lliFileSize/THREAD_NUM)/(COLUMN_WIDTH*COLUMN_NUMBER +1) + 1; //COLUMN_WIDTH*COLUMN_NUMBER "+1" : 是\n
	cout<<"iLineNum : "<<iLineNum<<endl;
	long long int lliSubBufferSize = iLineNum*(COLUMN_WIDTH*COLUMN_NUMBER +1);
	cout<<"lliSubBufferSize : "<<lliSubBufferSize<<endl;
	
	//read to each buffer
	start = chrono::steady_clock::now();
	vcharBuffer.resize(THREAD_NUM);
	for(i=0; i<THREAD_NUM; i++){
		vcharBuffer[i] = new char[lliSubBufferSize+1];
		
		lldarrStarPos[i] = i*lliSubBufferSize;
		pbuf->pubseekpos(lldarrStarPos[i]);
		
		if(i == THREAD_NUM-1 && THREAD_NUM!=1) {
			memset(vcharBuffer[i], 0, lliSubBufferSize);
			lliSubBufferSize = lliFileSize - THREAD_NUM-1*lliSubBufferSize;
		}
		
		pbuf->sgetn (vcharBuffer[i],lliSubBufferSize);
		if(!ifs){
			printf("error");
			exit(1);
		}
		vcharBuffer[i][lliSubBufferSize] = 0;
	}
	end = chrono::steady_clock::now();
	printf("\nFile Read : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
	
	/*----- Split Input Data(vcharBuffer)-----*/
	start = chrono::steady_clock::now();
	vector<thread> threads;
	for(i=0; i<THREAD_NUM; i++){
		threads.push_back(thread(&Data::ThreadRead, this, i));
	}
	for_each(threads.begin(), threads.end(), mem_fn(&thread::join));
	end = chrono::steady_clock::now();
	printf("\nSplit Data : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
	cout<<"vvfContent : "<<vvfContent.size()<<endl;
	//for(i=0; i<this->THREAD_NUM; i++) delete[] vcharBuffer[i];
}
void Data::ThreadRead(int ind){
	cout<<ind<<"---"<<strlen(vcharBuffer[ind])<<endl;
	vector<vector<float>> vfBuffer;
	
	char* cptrLine = NULL;
	char* cptrOuter = NULL;
	char* cptrInner = NULL;
	char *cptrBuffer = NULL;
	int icount = 0;
	cptrBuffer = vcharBuffer[ind];
			
	while(cptrLine = strtok_r(cptrBuffer, "\n", &cptrOuter)){
		cptrBuffer = cptrLine;
		vector<float> vf;
			
		while(cptrLine = strtok_r(cptrBuffer, ",", &cptrInner)){
			cptrBuffer = NULL;
			string s(cptrLine);
			vf.push_back(atof(s.c_str()));
		}
		cptrBuffer = NULL;
		vfBuffer.push_back(vf);
	}
	delete[] vcharBuffer[ind];
	cout<<"vfBuffer.size() : "<<vfBuffer.size()<<endl;
	unique_lock<mutex> lock_m(mutex_viContent);
	vvfContent.insert(vvfContent.end(), vfBuffer.begin(), vfBuffer.end());
}
int main(){
	unique_ptr<Data> dataptr2008(new Data("../DataTest/Data/2008SubFix.txt"));
	
	
	return 0;
}