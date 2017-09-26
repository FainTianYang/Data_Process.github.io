#include <stdio.h>
#include <iostream>
#include <vector>
#include <tuple>
#include <memory>
#include <thread>
#include <string.h>

using namespace std;

const int BUFFER_SIZE = 256; //2008.csv : 144
const int SUB_BUFFER_SIZE = 24;
const int THREAD_NUM = 4;

string strOperate[1] = {"SUM"};
string strColumnName[14] = {"Month","DayOfMonth","DayOfWeek","DepTime","CRSDepTime",
								"ArrTime","CRSArrTime","FlightNum","ActualElapsedTime","CRSElapsedTime",
								"AirTime","ArrDelay","DepDelay","Distance"};
								
vector< vector<float> > vvData;
vector< tuple<float, float, float>> vtResult; //<column name outer, column name inner, sum value>

void funThread(int iThreadInd){
	int iStart = 0; //start index
	int iEnd = 0; //end index(include value at last)
	int iSubDataNum = vvData.size() / THREAD_NUM;
	vector< tuple< float, vector<tuple<float, float>>> > vtResultThread;
	
	iStart = iThreadInd * iSubDataNum ;
	if(iThreadInd < THREAD_NUM-1) iEnd = (iThreadInd+1) * iSubDataNum - 1;
	else iEnd = vvData.size()-1;
	
	float fD1 = 0.0;
	float fD2 = 0.0;
	float fD3 = 0.0;
	int iInd1 = -1;
	int iInd2 = -1;
	
	for(int n=iStart; n<=iEnd; n++){
		iInd1 = -1;
		iInd2 = -1;
		fC1 = vvData[n][0];
		fC2 = vvData[n][1];
		fC3 = vvData[n][2];
		for(int i=0; i<vtResultThread.size(); i++){
			if(fC1 == vtResultThread[i].get<0>){
				iInd1 = i;
				break;
			}
			else{}
		}
		if(iInd1 == -1){
			vector<tuple<float, float>> v = {make_tuple(fD2, fD3)};
			vtResultThread.push_bck(make_tuple(fD1, v));
		}
		else{
			for(int j=0; j<vtResultThread[iInd1]get<1>.size(); j++){
				if(fC2 == vtResultThread[iInd1].get<1>[j]){
					iInd2 = j;
					break;
				}
				else{}
			}
			if(iInd2 == -1){
				vtResultThread[iInd1]get<1>.push_back
			}
		}
	}
}

int main(){
	int iOperateColumn = 5;
	int iGroupByColumn[2] = {0,2};
	int iOperate = 0;
	vector<int> vNeedColumn = {0,2,5};
	
	printf("SELECT %s %s, %s %s\nGROUP BY %s %s\n", strColumnName[iGroupByColumn[0]].c_str(), strColumnName[iGroupByColumn[1]].c_str(), strOperate[iOperate].c_str(), strColumnName 		[iOperateColumn].c_str(), strColumnName[iGroupByColumn[0]].c_str(), strColumnName[iGroupByColumn[1]].c_str());
	
	/*----- Read file -----*/
	//save data i need
	char carrSub[SUB_BUFFER_SIZE]; //input buffer
	char cptrInput[BUFFER_SIZE]; //Sub word
	char *cptrStop = NULL; //stop location of sub word
	char *cptrStart = NULL; //start locstion of sub word
	int iColumnInd = 0;
	int iNowInd = 0;
	string strFilePath = "/home/Data/1999Sub.csv";
	FILE *fp = fopen(strFilePath.c_str(), "r");
	
	if(!fp){
		printf("File not exist!!\n");
		exit(1);
	}
	
	cptrStart = cptrInput;
	
	/*--- read another row(column) ---*/
	while(fgets(cptrInput, BUFFER_SIZE, fp)!=NULL) {
		vector<float> v; 
		cptrStart = cptrInput;
		cptrStop = strchr(cptrStart,',');
		iColumnInd = 0;
		iNowInd = 0;
		while(cptrStop != NULL){
			if(iColumnInd>=vNeedColumn.size()) break;
			else{
				if(iNowInd == vNeedColumn[iColumnInd]){
					memcpy(carrSub, cptrStart, strlen(cptrStart)-strlen(cptrStop));
					carrSub[strlen(cptrStart)-strlen(cptrStop)] = 0;
					v.push_back(atof(carrSub));
					iColumnInd++;
				}
			}
			cptrStart = cptrStop+1;
			cptrStop = strchr(cptrStart,',');
			iNowInd++;
		}
		if(iColumnInd<vNeedColumn.size()){
			memcpy(carrSub, cptrStart, strlen(cptrStart)-1);
			carrSub[strlen(cptrStart)-1] = 0;		
			v.push_back(atof(carrSub));
		}
		vvData.push_back(v);
	}
	
	fclose(fp);
	
	printf("vContent.size() : %zd\n", vvData.size());
	
	
	for(int j =0; j<10; j++){
		cout<<"\nIndex : "<< j <<" -- ";
		for(int i=0;i<vvData[j].size();i++){
			cout<<vvData[j][i]<<" , ";
		}
	}
	
	/*----- Group By -----*/
	thread *threadptrMy;
	threadptrMy = new thread[THREAD_NUM];
	
	int iThInd = 0;
	for(iThInd=0; iThInd<THREAD_NUM ; iThInd++){
		threadptrMy[iThInd] = thread(funThread, iThInd); 
	}
	for(iThInd=0; iThInd<THREAD_NUM ; iThInd++){ 
		threadptrMy[iThInd].join(); 
	}
	
	
	return 0;
}
