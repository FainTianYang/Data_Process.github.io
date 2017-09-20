#include <stdio.h>
#include <iostream>
#include <vector>
#include <string.h>

using namespace std;

const int BUFFER_SIZE = 256; //2008.csv : 144
const int SUB_BUFFER_SIZE = 24;

class Reader{
	private:
		char carrSub[SUB_BUFFER_SIZE]; //input buffer
		char cptrInput[BUFFER_SIZE]; //Sub word
		char *cptrStop; //stop location of sub word
		char *cptrStart; //start locstion of sub word
		vector< vector<float> > *vptrContent;
		void reading(){
			/*---------- Read file ----------*/
			FILE *fp = fopen(strPath.c_str(), "r");
			
			if(!fp){
				printf("File mot exist!!\n");
			}
			
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
				}
				memcpy(carrSub, cptrStart, strlen(cptrStart)-1);
				carrSub[strlen(cptrStart)-1] = 0;
				
				v.push_back(atof(carrSub));
				vptrContent->push_back(v);
			}
			fclose(fp);
		}

	public:
		string strPath;
		Reader(string str, vector< vector<float> > *vC){
			strPath = str;
			vptrContent = vC;
			reading();
		}
};

int main( int argc, char** argv ){
	vector< vector<float> > vContent; //file content
	
	Reader *reader1 = new Reader("/home/Data/1999Sub.csv", &vContent);
	delete reader1;

	/*---------- Show info ----------*/
	cout<<vContent.size()<<"\n";
	
	cout<<"\n";
	for(int j =0; j<10; j++){
		cout<<"\nIndex : "<< j <<" -- ";
		for(int i=0;i<vContent[j].size();i++){
			cout<<vContent[j][i]<<" , ";
		}
	}

	cout<<vContent.size()<<"\n";
	cout<<"vContent Size 1 : "<< vContent.size() * vContent[0].size() * sizeof(float)<<endl;
	cout<<"vContent Size 2 : "<< vContent.size() * sizeof vContent[0] <<endl;
	cout<<"vContent Size 3 : "<< sizeof vContent <<endl;
	
	return 0;
}
