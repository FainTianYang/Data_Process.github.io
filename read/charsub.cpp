/*測試如何將char *cBuff (一行的char拆開)*/
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>

using namespace std;

int main( int argc, char** argv ){
	char *cBuff = "1,5,15,,-4,-44.333,NAN,78.6";
	char *start;
	char *stop;
	char cSub[24];
	float fvalue = 0.0;
	
	start = cBuff;
	stop = strchr(start,',');
	while(stop != NULL){
		memcpy(cSub, start, strlen(start)-strlen(stop));
		cSub[strlen(start)-strlen(stop)] = 0;
		fvalue = atof(cSub);
		if(fvalue == fvalue)
			cout<<"start : "<<start<<" -- stop : "<<stop<<"  --  "<<strlen(start)-strlen(stop)<<"  --  "<<fvalue<<endl;

		start = stop+1;
		stop = strchr(start,',');
	}
	memcpy(cSub, start, strlen(start));
	cSub[strlen(start)] = 0;
	fvalue = atof(cSub);
	cout<<"start : "<<start<<"  --  "<<strlen(start)<<"  --  "<<fvalue<<endl;
	
	std::ifstream ifs ;
	ifs.open("./DataTest/Data/2008tSub.csv", std::ifstream::in);

	// get pointer to associated buffer object
	filebuf* pbuf = ifs.rdbuf();

	// get file size using buffer's members
	size_t size = pbuf->pubseekoff (0,ifs.end,ifs.in);
	pbuf->pubseekpos (0,ifs.in);

	cout<<"size : "<<size<<endl;
	
	// allocate memory to contain file data
	char* buffer=new char[size];

	// get file data
	pbuf->sgetn (buffer,size);

	ifs.close();

	// write content to stdout
	//cout.write (buffer,size);
	
	char* cptrLine = NULL;
	char* cptrOuter = NULL;
	char* cptrInner = NULL;
	char *cptrBuffer = NULL;
	
	vector< vector<string> > vC;
	
	cptrBuffer = buffer;
	while(cptrLine = strtok_r(cptrBuffer, "\n", &cptrOuter)){
		cptrBuffer = cptrLine;
		vector<string> v;
		
		while(cptrLine = strtok_r(cptrBuffer, ",", &cptrInner)){
			cptrBuffer = NULL;
			string s(cptrLine);
			v.push_back(s);
		}
		cptrBuffer = NULL;
		vC.push_back(v);
	}
	for(int i=0; i<20; i++){
		cout<<"Index : "<<endl;
		for(int j=0; j<vC[i].size(); j++){
			cout<<vC[i][j]<<endl;;
		}
		cout<<endl;
	}
	
	delete[] buffer;
}