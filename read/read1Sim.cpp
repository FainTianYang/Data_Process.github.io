#include <stdio.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <string.h>

using namespace std;

const int BUFFER_SIZE = 256; //2008.csv : 144
const int SUB_BUFFER_SIZE = 24;

int main( int argc, char** argv ){
	vector< vector<float> > vContent; //file content
	long long countW = 0;
	char carrSub[SUB_BUFFER_SIZE]; //input buffer
	char cptrInput[BUFFER_SIZE]; //Sub word
	char *cptrStop = NULL; //stop location of sub word
	char *cptrStart = NULL; //start locstion of sub word
	chrono::time_point<chrono::steady_clock> tpStart, tpEnd;
	tpStart = chrono::steady_clock::now();

	/*---------- Read file ----------*/
	FILE *fp = fopen("/home/Data/1996_2008Sub.csv", "r = NULL");
			
	if(!fp){
		printf("File not exist!!\n");
	}
	cptrStart = cptrInput;
	
	/*--- read another row(column) ---*/
	while(fgets(cptrInput, BUFFER_SIZE, fp)!=NULL) {
		vector<float> v; 
		cptrStart = cptrInput;
		cptrStop = strchr(cptrStart,',');
				
		while(cptrStop != NULL){
			memcpy(carrSub, cptrStart, strlen(cptrStart)-strlen(cptrStop));
			carrSub[strlen(cptrStart)-strlen(cptrStop)] = 0;
			cptrStart = cptrStop+1;
			cptrStop = strchr(cptrStart,',');
			v.push_back(atof(carrSub));
			countW++;
		}
		memcpy(carrSub, cptrStart, strlen(cptrStart)-1);
		carrSub[strlen(cptrStart)-1] = 0;
				
		v.push_back(atof(carrSub));
		countW++;
		vContent.push_back(v);
	}
	
	fclose(fp);
	tpEnd = chrono::steady_clock::now();
	/*---------- Show info ----------*/
	printf("Time : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds>(tpEnd - tpStart).count());
	printf("countW : %lld\n", countW);
	printf("vContent.size() : %zd\n", vContent.size());
	
	
	for(int j =0; j<10; j++){
		cout<<"\nIndex : "<< j <<" -- ";
		for(int i=0;i<vContent[j].size();i++){
			cout<<vContent[j][i]<<" , ";
		}
	}
	
	return 0;
}
