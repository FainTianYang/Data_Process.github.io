#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <string>
#include <time.h>
#include <stdio.h>
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

/*---------- Class & Template ----------*/
	
class data{
	public:
		string strPath;
		vector< tuple<int, string> > vColumn_name;
		int iColumnNum;
		int iRow;
		vector< vector<string> > vContent;
	
		data(string str);
		data(){};
	
		void readFile();
};
data::data(string str){
		strPath = str;
		iColumnNum = 0;
		iRow = 0;
}
void data::readFile(){
	ifstream file (strPath.c_str(), ifstream::in);
	string line;
	bool fst = true;
		
	while( getline(file, line) ){
		istringstream s(line);
		string value;
		vector<string> vLine;
		
		if(fst){
			/*Get Column*/
			while (getline(s, value, ',')){
				tuple<int, string> t = make_tuple(iColumnNum, value);
				vColumn_name.push_back(t);
				iColumnNum++;
			}
			fst = false;
		}
		else{
			while (getline(s, value, ',')){
				vLine.push_back(value);
			}
			vContent.push_back(vLine);
			vContent.push_back(vLine);
			iRow+=2;
		}
			
	}
	printf("Read File Finish!\n");
	file.close();
}

class dataGroup{
private:
	struct ClusterData{
		string strName;
		vector<int> vInd;
		atomic<int> atoSum;
		atomic<long long> atoSDSub;
		float fAverage;
		float StandardDeviation;
		ClusterData(){
			strName = "";
			StandardDeviation = 0.0;
			atoSum = 0;
			atoSDSub = 0;
		}
		
	};
	
	string strColumnName;
	int iColumnInd;
	vector<ClusterData*> vCluData;
	mutex mutexCD;
	int iThreadNUm;
	int iSubThread;
	atomic<int> *iCpuUsed;
	
	void grouping(weak_ptr<data>);
	void clusterSD(weak_ptr<data>);
	void groupingThread(int, weak_ptr<data> );
	void clusterSDThread(int, weak_ptr<data> );
	
public:
	
	dataGroup(int, weak_ptr<data>, int);
	dataGroup(string, weak_ptr<data>);
	void showInfo();
}; 
dataGroup::dataGroup(int ind, weak_ptr<data> wptrD, int iThNum){
	chrono::time_point<chrono::steady_clock> start, end;
	
	start = chrono::steady_clock::now();
	
	shared_ptr<data> sptrD = wptrD.lock();
	
	iThreadNUm = iThNum;
	iSubThread = sptrD->vContent.size() / iThreadNUm;
	this->iColumnInd = ind;
	for(auto tp : sptrD->vColumn_name){
		if(ind == get<0>(tp)){
			this->strColumnName = get<1>(tp);
			break;
		}
	}
	iCpuUsed = new atomic<int>[8];
	for(int k=0; k<8; k++) { iCpuUsed[k]=0; }
	
	grouping(wptrD);
	clusterSD(wptrD);
	
	sptrD.reset();
	wptrD.reset();
	
	end = chrono::steady_clock::now();
	
	/*----- Show Data Info -----*/
	chrono::duration<double> elapsed_seconds = end-start;
	printf("\niThreadNUm num : %d\n", iThreadNUm);
	printf("CPU used : \n");
	for(int k=0; k<8; k++) { printf("[%d] : %d\n", k, iCpuUsed[k].load()); }
	printf("Time : %ld ms\n", chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
	
	showInfo();
}
void dataGroup::showInfo(){
	printf("strColumnName : %s\n", strColumnName.c_str());
	printf("iColumnInd : %d\n", iColumnInd);
	printf("ClusterData : \n");
	for(int n=0; n<vCluData.size(); n++){ 
		printf("strName : %s -- ElementNum : %zd -- Sum : %d\n", vCluData[n]->strName.c_str(), vCluData[n]->vInd.size(), vCluData[n]->atoSum.load()); 
		printf("SDNum : %lld\n", vCluData[n]->atoSDSub.load()); 
		printf("StandardDeviation : %f\n", vCluData[n]->StandardDeviation); 
	}
}
void dataGroup::grouping(weak_ptr<data> wptrD){
	thread *arrThread;
	
	/*----- Create Thread -----*/
	arrThread = new thread[iThreadNUm];
	
	for(int i=0; i<iThreadNUm; i++){ arrThread[i] = thread(&dataGroup::groupingThread, this, i, wptrD); }
	for(int i=0; i<iThreadNUm; i++){ arrThread[i].join(); }
	
}
void dataGroup::clusterSD(weak_ptr<data> wptrD){
	thread *arrThread;
	
	/*----- Create Thread -----*/
	arrThread = new thread[iThreadNUm];
	
	for(int i=0; i<iThreadNUm; i++){ arrThread[i] = thread(&dataGroup::clusterSDThread, this, i, wptrD); }
	for(int i=0; i<iThreadNUm; i++){ arrThread[i].join(); }
	
	for(int j=0; j<vCluData.size(); j++){
		vCluData[j]-> StandardDeviation = sqrt(vCluData[j]->atoSDSub.load()/(float)vCluData[j]->vInd.size());
	}
	
};
void dataGroup::groupingThread(int iThreadCount, weak_ptr<data> wptrD){
	int iStart = 0; //start index
	int iEnd = 0; //end index(include value at last)
	vector<string> vStrName; //cluster name
	vector<int> vISum; //cluster sum
	vector< vector<int> > vvInd; //index of element (grouping)
	int iCupId = GetCurrentProcessorNumber(); //current cpuID which this thread running 
	
	shared_ptr<data> sptrDTh = wptrD.lock(); //data pointer
	
	iStart = iThreadCount * iSubThread;
	if(iThreadCount < iThreadNUm-1) iEnd = ((iThreadCount+1) * iSubThread) - 1;
	else iEnd = sptrDTh->iRow-1;
	
	string strGroName = ""; //element group name
	int iColNamInd = -1; //element group index in vStrName;
	vector<int> vvI; //empty vector
	
	for(int n=iStart; n<=iEnd; n++){
		strGroName = sptrDTh->vContent[n][this->iColumnInd];
		iColNamInd = -1;
		if(vStrName.empty()){
			vStrName.push_back(strGroName);
			iColNamInd = 0;
			vvInd.push_back(vvI);
			vISum.push_back(0);
		}
		else{
			for(int i=0; i<vStrName.size(); i++){
				if(strGroName == vStrName[i]){
					iColNamInd = i;
					break;
				}
			}
			if(iColNamInd == -1){
				vStrName.push_back(strGroName);
				iColNamInd = vStrName.size()-1;
				vector<int> vvI;
				vvInd.push_back(vvI);
				vISum.push_back(0);
			} 
		}
		vvInd[iColNamInd].push_back(n);
		vISum[iColNamInd]+= atoi(sptrDTh->vContent[n][12].c_str())+atoi(sptrDTh->vContent[n][11].c_str());
	}
	
	
	int i_vCluData_Ind = -1;
	for(int j=0; j<vStrName.size(); j++){
		unique_lock<mutex> lk(mutexCD);
		i_vCluData_Ind = -1;
		for(int i=0; i<vCluData.size(); i++){
			if(vStrName[j] == vCluData[i]->strName){
				i_vCluData_Ind = i;
				break;
			}
		}
		if(i_vCluData_Ind == -1){
			ClusterData *CD = new ClusterData();
			vCluData.push_back(new ClusterData());
			vCluData[vCluData.size()-1]->strName.append(vStrName[j]);
			i_vCluData_Ind = vCluData.size()-1;
			delete CD;
		}
		vCluData[i_vCluData_Ind]->vInd.insert(vCluData[i_vCluData_Ind]->vInd.end(), vvInd[j].begin(), vvInd[j].end());
		vCluData[i_vCluData_Ind]->atoSum+= vISum[j];
		lk.unlock();
	}
	
	
	iCpuUsed[iCupId]+=1;
	sptrDTh.reset();
}
void dataGroup::clusterSDThread(int iThreadCount, weak_ptr<data> wptrD){
	int iStart = 0; //start index
	int iEnd = 0; //end index(include value at last)
	int iCupId = GetCurrentProcessorNumber(); //current cpuID which this thread running 
	int iSubStep = 0;
	long long iSub = 0;
	
	shared_ptr<data> sptrDTh = wptrD.lock(); //data pointer
	
	for(int i=0; i<vCluData.size(); i++){
		iSubStep = vCluData[i]->vInd.size() / iThreadNUm;
		
		iStart = iThreadCount * iSubStep;
		if(iThreadCount < iThreadNUm-1) iEnd = ((iThreadCount+1) * iSubStep) - 1;
		else iEnd = vCluData[i]->vInd.size() - 1;
		
		for(int n=iStart; n<=iEnd; n++){
			iSub = atoi(sptrDTh->vContent[vCluData[i]->vInd[n]][13].c_str()) - vCluData[i]->atoSum.load()/vCluData[i]->vInd.size();
			vCluData[i]->atoSDSub += iSub*iSub;
		}
	}
}

/*---------- Main ----------*/
int main( int argc, char** argv ){
	 std::cout << "DEBUG: accessfile() called by process " << getpid() << " (parent: " << ::getppid() << ")" << std::endl;
	/*----- Read file -----*/
	shared_ptr<data> sptrData = make_shared<data>(data("Data/2008N.csv"));
	weak_ptr<data> wptrData(sptrData);
	sptrData->readFile();
	
	/*----- Show Data Info -----*/
	unique_ptr<dataGroup> dgPtr1(new dataGroup(0, wptrData, 1));
	unique_ptr<dataGroup> dgPtr2(new dataGroup(0, wptrData, 2));
	unique_ptr<dataGroup> dgPtr4(new dataGroup(0, wptrData, 4));
	unique_ptr<dataGroup> dgPtr8(new dataGroup(0, wptrData, 8));
	
	sptrData.reset();
	wptrData.reset();
	
	return 0;
}
