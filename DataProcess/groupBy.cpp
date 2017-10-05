#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <mutex>
#include <memory>
#include <vector>

#ifndef CONIO_H
#define CONIO_H
int getch( void );
int getche( void );
#endif // CONIO_H

using namespace std;

#define COLUMN_WIDTH 7
#define COLUMN_NUMBER 14

class Data{
public:
	string strFilePath;
	vector<int> viContent;
	
	Data(string str){
		strFilePath = str;
		Read();
	}
	~Data(){
		printf("~Data -- %s \n", this->strFilePath.c_str());
	}
private:
	size_t lliFileSize = 0;
	static const int iThreadNum = 4;
	long long int lldarrStarPos[iThreadNum];
	void Read();
};

void Data::Read(){
	int i=0;
	vector<char*> vcharBuffer(this->iThreadNum);
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
	
	int iLineNum = (lliFileSize/iThreadNum)/(COLUMN_WIDTH*COLUMN_NUMBER +1) + 1; //COLUMN_WIDTH*COLUMN_NUMBER "+1" : 是\n
	cout<<"iLineNum : "<<iLineNum<<endl;
	long long int lliSubBufferSize = iLineNum*(COLUMN_WIDTH*COLUMN_NUMBER +1);
	cout<<"lliSubBufferSize : "<<lliSubBufferSize<<endl;
	
	//read to each buffer
	start = chrono::steady_clock::now();
	
	for(i=0; i<this->iThreadNum; i++){
		vcharBuffer[i] = new char[lliSubBufferSize+1];
		
		lldarrStarPos[i] = i*lliSubBufferSize;
		pbuf->pubseekpos(lldarrStarPos[i]);
		
		if(i == this->iThreadNum-1) {
			memset(vcharBuffer[i], 0, lliSubBufferSize);
			lliSubBufferSize = lliFileSize - this->iThreadNum-1*lliSubBufferSize;
		}
		
		pbuf->sgetn (vcharBuffer[i],lliSubBufferSize);
		if(!ifs){
			printf("error");
			exit(1);
		}
		vcharBuffer[i][lliSubBufferSize] = 0;
	}
	ifs.close();
	end = chrono::steady_clock::now();
	printf("\nFile Read : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds> (end - start).count());
	
	
	for(i =0;i<this->iThreadNum; i++) delete[] vcharBuffer[i];
}
int main(){
	unique_ptr<Data> dataptr2008(new Data("../DataTest/Data/2008SubFix.txt"));
	
	printf("test");
	
	return 0;
}