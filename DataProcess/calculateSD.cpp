#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>  
#include <unistd.h>
#include <windows.h>  

#ifndef CONIO_H
#define CONIO_H

int getch( void );
int getche( void );

#endif // CONIO_H

using namespace std;

class Reader{
	private:
		static const int BUFFER_SIZE = 256; //2008.csv : 144
		static const int SUB_BUFFER_SIZE = 24;
		
		char carrSub[SUB_BUFFER_SIZE]; //input buffer
		char carrInput[BUFFER_SIZE]; //Sub word
		char *cptrStop; //stop location of sub word
		char *cptrStart; //start locstion of sub word
		vector< tuple<int, string> > *vtuptrName;
		vector< vector<int> > *vviptrContent;
		string strPath;
		bool boolReadColumn; //whether save column(only first file need save)
		
		void reading(){
			/*---------- Read file ----------*/
			FILE *fp = fopen(strPath.c_str(), "r");
			int iColumnInd = 0;
			/*--- check file existuing ---*/
			if (!fp) {
				printf("File not exist !!");
				exit(EXIT_FAILURE);
			}

			/*--- read header ---*/
			fgets(carrInput, BUFFER_SIZE, fp);
			if(boolReadColumn){
				cptrStart = carrInput;
				cptrStop = strchr(cptrStart,',');
				
				while(cptrStop != NULL){
					memcpy(carrSub, cptrStart, strlen(cptrStart)-strlen(cptrStop));
					carrSub[strlen(cptrStart)-strlen(cptrStop)] = 0; //last char should fill 0
					cptrStart = cptrStop+1; //1 : skip a ','
					cptrStop = strchr(cptrStart,',');
					tuple<int, string> t = make_tuple(iColumnInd, carrSub);
					vtuptrName->push_back(t);
					iColumnInd++;
				}
				//the last word is start to end of carrSub 
				memcpy(carrSub, cptrStart, strlen(cptrStart));
				carrSub[strlen(cptrStart)] = 0;
				tuple<int, string> t = make_tuple(iColumnInd, carrSub);
				vtuptrName->push_back(t);
				iColumnInd++;
			}
			else {}
			
			/*--- read another row(column) ---*/
			while(fgets(carrInput, BUFFER_SIZE, fp)!=NULL) {
				vector<int> v; 
				cptrStart = carrInput;
				cptrStop = strchr(cptrStart,',');
				
				while(cptrStop != NULL){
					memcpy(carrSub, cptrStart, strlen(cptrStart)-strlen(cptrStop));
					carrSub[strlen(cptrStart)-strlen(cptrStop)] = 0;
					cptrStart = cptrStop+1;
					cptrStop = strchr(cptrStart,',');
					v.push_back(atoi(carrSub));
				}
				memcpy(carrSub, cptrStart, strlen(cptrStart)-1);
				carrSub[strlen(cptrStart)-1] = 0;
				
				v.push_back(atoi(carrSub));
				vviptrContent->push_back(v);
			}
			fclose(fp);
		}

	public:
		Reader(string str, vector< tuple<int, string> > *vN, vector< vector<int> > *vC, bool boolColumn){
			strPath = str;
			vtuptrName = vN;
			vviptrContent = vC;
			boolReadColumn = boolColumn;
			reading();
		}
		~Reader(){ printf("Destory Reader~\n"); }
};

class Data{
	public:
		string *strptrPath; //file path (pointer to array)
		int iFileNum; //file number
		vector< tuple<int, string> > vtuColumnName; //column name and index(first row)
		vector< vector<int> > vviContent;
		int iColumnNum; //column number
		int iRowNum; //row number
		
		Data(string *str, int i){
			strptrPath = str;
			iFileNum = i;
			iColumnNum = 0;
			iRowNum = 0;
		}
		~Data(){ printf("Destory data~ \n"); }
	
		void readFile(){
			for(int n=0; n<iFileNum; n++){
				bool boolReadColumn = true;
				
				if(n == 0) boolReadColumn = true;
				else boolReadColumn = false;
				
				Reader *reader1 = new Reader(strptrPath[n].c_str(), &vtuColumnName, &vviContent, boolReadColumn);
				delete reader1;
				iColumnNum = vtuColumnName.size();
				iRowNum = vviContent.size();
				
				cout<<"Finish : "<< strptrPath[n] <<endl;
				cout<<"iColumnNum : "<<iColumnNum<<endl;
				cout<<"iRowNum : "<<iRowNum<<endl;
			}
		}
};

class DataGroup{
private:
	struct ClusterData{
		int iName; //column name
		vector<int> viInd; //index in "data->vContent"
		atomic<int> atoiSum; //sumnth of group
		atomic<long long> atolliSubSum; //Sub*Sub sum of group
		float fStandardDeviation; //Standard Deviation of group
		
		ClusterData(){
			iName = -1;
			atoiSum = 0;
			atolliSubSum = 0;
			fStandardDeviation = 0.0;
		}
	};
	
	string strColumnName;
	int iColumnInd;
	vector<ClusterData*> vcdptrCluData;
	mutex mutexCD; //mutex of vCluData
	int iThreadNUm; //thread number
	int iSubThread; //processed data number of each thread
	atomic<int> *atoiptrCpuUsed; //thread use which cpu
	
	void grouping(weak_ptr<Data>); 
	void clusterSD(weak_ptr<Data>);
	void groupingThread(int, weak_ptr<Data> );
	void clusterSDThread(int, weak_ptr<Data> );
	void addCluData(int iNameInput){
		ClusterData *CD = new ClusterData();
		vcdptrCluData.push_back(new ClusterData());
		vcdptrCluData[vcdptrCluData.size()-1]->iName = iNameInput;
		delete CD;
	};
	
public:
	DataGroup(int, weak_ptr<Data>, int);
	~DataGroup(){ printf("Destory DataGroup ~\n"); }
	void showInfo();
}; 
DataGroup::DataGroup(int iColumnIndInput, weak_ptr<Data> wptrD, int iThNum){
	chrono::time_point<chrono::steady_clock> tpStart, tpEnd;
	tpStart = chrono::steady_clock::now();
	
	shared_ptr<Data> sptrD = wptrD.lock();
	iThreadNUm = iThNum;
	iSubThread = sptrD->vviContent.size() / iThreadNUm;
	iColumnInd = iColumnIndInput;
	for(auto tp : sptrD->vtuColumnName){
		if(iColumnInd == get<0>(tp)){
			strColumnName = get<1>(tp);
			break;
		}
		else{
			printf("This column not exist!!\n");
			exit(EXIT_FAILURE);
		}
	}
	atoiptrCpuUsed = new atomic<int>[8];
	for(int k=0; k<8; k++) { atoiptrCpuUsed[k]=0; } //init array pointer 
	
	grouping(wptrD);
	clusterSD(wptrD);
	
	sptrD.reset(); // reset pointer
	wptrD.reset();
	
	tpEnd = chrono::steady_clock::now();
	
	/*----- Show Data Info -----*/
	printf("\niThreadNUm num : %d\n", iThreadNUm);
	printf("CPU used : \n");
	for(int k=0; k<8; k++) { printf("[%d] : %d\n", k, atoiptrCpuUsed[k].load()); }
	printf("Time : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds>(tpEnd - tpStart).count());
	
	showInfo();
	delete[] atoiptrCpuUsed;
}
void DataGroup::grouping(weak_ptr<Data> wptrD){
	/*----- Create Thread -----*/
	thread *threadptrMy;
	threadptrMy = new thread[iThreadNUm];
	
	int iThInd = 0;
	for(iThInd=0; iThInd<iThreadNUm; iThInd++){
		threadptrMy[iThInd] = thread(&DataGroup::groupingThread, this, iThInd, wptrD); 
	}
	for(iThInd=0; iThInd<iThreadNUm; iThInd++){ 
		threadptrMy[iThInd].join(); 
	}
}
void DataGroup::groupingThread(int iThreadInd, weak_ptr<Data> wptrD){
	int iStart = 0; //start index
	int iEnd = 0; //end index(include value at last)
	vector<int> viName; //cluster name
	vector<int> viSum; //cluster sum
	vector< vector<int> > vviInd; //index of element (grouping)
	int iCupId = GetCurrentProcessorNumber(); //current cpuID which this thread running 
	
	shared_ptr<Data> sptrD = wptrD.lock(); //data pointer
	
	iStart = iThreadInd * iSubThread;
	if(iThreadInd < iThreadNUm-1) iEnd = (iThreadInd+1) * iSubThread - 1;
	else iEnd = sptrD->iRowNum-1;
	
	int iGpName = -1; //element group name
	int iNamInd = -1; //element group index in vStrName;
	
	for(int n=iStart; n<=iEnd; n++){
		iGpName = sptrD->vviContent[n][iColumnInd];
		iNamInd = -1;
		if(viName.empty()){
			vector<int> vviEmpty; //empty vector
			viName.push_back(iGpName);
			iNamInd = 0;
			vviInd.push_back(vviEmpty);
			viSum.push_back(0);
		}
		else{
			for(int m=0; m<viName.size(); m++){
				if(iGpName == viName[m]){
					iNamInd = m;
					break;
				}
			}
			if(iNamInd == -1){ //mean no this group data
				vector<int> vviEmpty; //empty vector
				viName.push_back(iGpName);
				iNamInd = viName.size()-1;
				vviInd.push_back(vviEmpty);
				viSum.push_back(0);
			} 
		}
		vviInd[iNamInd].push_back(n);
		viSum[iNamInd] += sptrD->vviContent[n][13];
	}
	
	int i_vCluDataInd = -1;
	unique_lock<mutex> lk(mutexCD);
	for(int j=0; j<viName.size(); j++){
		i_vCluDataInd = -1;
		for(int i=0; i<vcdptrCluData.size(); i++){
			if(viName[j] == vcdptrCluData[i]->iName){
				i_vCluDataInd = i;
				break;
			}
		}
		if(i_vCluDataInd == -1){
			addCluData(viName[j]);
			i_vCluDataInd = vcdptrCluData.size()-1;
		}
		
		vcdptrCluData[i_vCluDataInd]->viInd.insert(vcdptrCluData[i_vCluDataInd]->viInd.end(), vviInd[j].begin(), vviInd[j].end());
		vcdptrCluData[i_vCluDataInd]->atoiSum += viSum[j];
	}
	lk.unlock();
	
	atoiptrCpuUsed[iCupId] += 1;
	sptrD.reset();
}
void DataGroup::clusterSD(weak_ptr<Data> wptrD){
	thread *threadptrMy;
	
	/*----- Create Thread -----*/
	threadptrMy = new thread[iThreadNUm];
	
	int iThInd = 0;
	for(iThInd=0; iThInd<iThreadNUm; iThInd++){ 
		threadptrMy[iThInd] = thread(&DataGroup::clusterSDThread, this, iThInd, wptrD); 
	}
	for(iThInd=0; iThInd<iThreadNUm; iThInd++){ 
		threadptrMy[iThInd].join(); 
	}
	
	for(int j=0; j<vcdptrCluData.size(); j++){
		vcdptrCluData[j]->fStandardDeviation = sqrt(vcdptrCluData[j]->atolliSubSum.load()/(float)vcdptrCluData[j]->viInd.size());
	}
}
void DataGroup::clusterSDThread(int iThreadInd, weak_ptr<Data> wptrD){
	int iStart = 0; //start index
	int iEnd = 0; //end index(include value at last)
	int iCupId = GetCurrentProcessorNumber(); //current cpuID which this thread running 
	int iSubStep = 0;
	long long iSub = 0;
	
	shared_ptr<Data> sptrD = wptrD.lock(); //data pointer
	
	for(int i=0; i<vcdptrCluData.size(); i++){
		iSubStep = vcdptrCluData[i]->viInd.size() / iThreadNUm;
		
		iStart = iThreadInd * iSubStep;
		if(iThreadInd < iThreadNUm-1) iEnd = ((iThreadInd+1) * iSubStep) - 1;
		else iEnd = vcdptrCluData[i]->viInd.size() - 1;
		
		for(int n=iStart; n<=iEnd; n++){
			iSub = sptrD->vviContent[vcdptrCluData[i]->viInd[n]][13] - vcdptrCluData[i]->atoiSum.load()/vcdptrCluData[i]->viInd.size();
			vcdptrCluData[i]->atolliSubSum += iSub*iSub;
		}
	}
}
void DataGroup::showInfo(){
	printf("strColumnName : %s\n", strColumnName.c_str());
	printf("iColumnInd : %d\n", iColumnInd);
	printf("ClusterData : %zd \n", vcdptrCluData.size());
	for(int n=0; n<vcdptrCluData.size(); n++){ 
		printf("    iName : %d -- ElementNum : %zd\n", vcdptrCluData[n]->iName, vcdptrCluData[n]->viInd.size()); 
		printf("    	SDNum : %lld -- Sum : %d \n", vcdptrCluData[n]->atolliSubSum.load(), vcdptrCluData[n]->atoiSum.load()); 
		printf("    	StandardDeviation : %f\n", vcdptrCluData[n]->fStandardDeviation); 
	}
}
int main( int argc, char** argv ){
	cout << "DEBUG: main() called by process " << getpid() << " (parent: " << ::getppid() << ")" << std::endl;
	/*string strPath[3] = {"C:/Users/chun-hsin/Desktop/C++Project/pthreadTest/DataTest/Data/2008Sub.csv",
						"C:/Users/chun-hsin/Desktop/C++Project/pthreadTest/DataTest/Data/2007Sub.csv",
						"C:/Users/chun-hsin/Desktop/C++Project/pthreadTest/DataTest/Data/2006Sub.csv"};
	int iFileNum = 3;*/
	string strPath[1] = {"C:/Users/chun-hsin/Desktop/C++Project/pthreadTest/DataTest/Data/2008Sub.csv"};
	int iFileNum = 1;
	/*----- Read file -----*/
	chrono::time_point<chrono::steady_clock> tpStart, tpEnd;
	tpStart = chrono::steady_clock::now();
	
	shared_ptr<Data> sptrData = make_shared<Data>(Data(strPath, iFileNum));
	weak_ptr<Data> wptrData1(sptrData);
	sptrData->readFile();
	
	tpEnd = chrono::steady_clock::now();
	printf("Time : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds>(tpEnd - tpStart).count());
	/*----- Show Data Info -----
	unique_ptr<DataGroup> dgPtr1(new DataGroup(0, wptrData1, 1));
	dgPtr1.reset();
	unique_ptr<DataGroup> dgPtr2(new DataGroup(0, wptrData1, 2));
	dgPtr2.reset();
	unique_ptr<DataGroup> dgPtr4(new DataGroup(0, wptrData1, 4));
	dgPtr4.reset();
	unique_ptr<DataGroup> dgPtr8(new DataGroup(0, wptrData1, 8));
	dgPtr8.reset();*/
	
	wptrData1.reset();
	sptrData.reset();
	
	return 0;
}
