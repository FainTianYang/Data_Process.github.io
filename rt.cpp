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

#define COLUMN_WIDTH 8
#define COLUMN_NUMBER 14

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
	
	char* buffer = new char[lliFileSize+1];
	
	start = chrono::steady_clock::now();
	pbuf->sgetn(buffer, lliFileSize);
	if(!ifs){
		printf("error");
		exit(1);
	}
	buffer[lliFileSize] = 0;
	//cout<<buffer<<endl;
	end = chrono::steady_clock::now();
	printf("\nRead Data : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
	
	/*----- Split Input Data(vcharBuffer)-----*/
	start = chrono::steady_clock::now();
	int pos=0;
	char *sub = new char[COLUMN_WIDTH+1];
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
		vvfContent.push_back(v);
		pch = strtok(NULL,"\n");
	}
	end = chrono::steady_clock::now();
	printf("Split Data : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
	cout<<"vvfContent : "<<vvfContent.size()<<endl;
	for(int i=0;i<10;i++){
		for(int j=0;j<vvfContent[i].size(); j++){
			cout<<vvfContent[i][j]<<" , ";
		}
		cout<<endl;
	}
	delete[] buffer;
}

int main(){
	unique_ptr<Data> dataptr2008(new Data("../DataTest/Data/2008SubFix.prn"));
	
	
	return 0;
}