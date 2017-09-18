#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>

using namespace std;

class Reader{
	private:
		string strPath;
		vector<string> *vptrName;
		vector< vector<float> > *vptrContent;
		
		void reading(){
			ifstream ifs ;
			ifs.open(strPath.c_str(), ifstream::in);

			// get pointer to associated buffer object
			filebuf* pbuf = ifs.rdbuf();

			// get file size using buffer's members
			size_t size = pbuf->pubseekoff (0,ifs.end,ifs.in);
			pbuf->pubseekpos (0,ifs.in);
			
			// allocate memory to contain file data
			char* buffer=new char[size];

			// get file data
			pbuf->sgetn (buffer,size);

			ifs.close();
			
			char* cptrLine = NULL;
			char* cptrOuter = NULL;
			char* cptrInner = NULL;
			char *cptrBuffer = NULL;
			int icount = 0;
			cptrBuffer = buffer;
			
			cptrLine = strtok_r(cptrBuffer, "\n", &cptrOuter);
			cptrBuffer = cptrLine;
			
			while(cptrLine = strtok_r(cptrBuffer, ",", &cptrInner)){
				cptrBuffer = NULL;
				string s(cptrLine);
				vptrName->push_back(s);
			}
			cptrBuffer = NULL;
			
			while(cptrLine = strtok_r(cptrBuffer, "\n", &cptrOuter)){
				cptrBuffer = cptrLine;
				vector<float> v;
			
				while(cptrLine = strtok_r(cptrBuffer, ",", &cptrInner)){
					cptrBuffer = NULL;
					string s(cptrLine);
					v.push_back(atof(s.c_str()));
				}
				cptrBuffer = NULL;
				vptrContent->push_back(v);
			}
			delete[] buffer;
		}
	
	public:
		Reader(string str, vector<string> *vN, vector< vector<float> > *vC){
			strPath = str;
			vptrName = vN;
			vptrContent = vC;
			reading();
		}
};

int main( int argc, char** argv ){
	vector<string> vColumn_name; //column name
	vector< vector<float> > vContent; //file content
	
	Reader *reader1 = new Reader("./DataTest/Data/2008Sub.csv", &vColumn_name, &vContent);
	delete reader1;
	cout<<"Finish 1"<<endl;
	
	Reader *reader2 = new Reader("./DataTest/Data/2007Sub.csv", &vColumn_name, &vContent);
	delete reader2;
	cout<<"Finish 2"<<endl;
	Reader *reader3 = new Reader("./DataTest/Data/2006Sub.csv", &vColumn_name, &vContent);
	delete reader3;
	cout<<"Finish 3"<<endl;
	/*---------- Show info ----------*/
	/*---------- Show info ----------*/
	cout<<vContent.size()<<"\n";
	
	cout<<"\n";
	for(int i=0;i<vColumn_name.size();i++){
		cout<<vColumn_name[i]<<" , ";
	}
	
	for(int j = 402; j<422;j++){
		cout<<"\nIndex : "<< j <<" -- ";
		for(int i=0;i<vContent[j].size();i++){
			cout<<vContent[j][i]<<" , ";
		}
	}
	
	cout<<"vContent Size 1 : "<< vContent.size() * vContent[0].size() * sizeof(float)<<endl;
	cout<<"vContent Size 2 : "<< vContent.size() * sizeof vContent[0] <<endl;
	cout<<"vContent Size 3 : "<< sizeof vContent <<endl;
	
	return 0;
}