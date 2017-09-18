#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>

using namespace std;

float a[1] = {0.0};
struct addfloat{
	public:
		addfloat(float i){fSum = i;}
		void add(float b){ 
			fSum +=b; 
			//printf("Add %f\n", b);
		}
		void show(){ printf("fSum : %f\n", fSum); }
	private:
		float fSum;
};

atomic<addfloat*> _atoaddfloat;

void funcThread1(){
	printf("Thread1~~ \n");
	addfloat *aSum = _atoaddfloat.load();
	addfloat *newad;
	for(int i=0;i<10000;i++){
		do{
			newad = aSum;
			newad->add(1.0);
		}while(!_atoaddfloat.compare_exchange_strong(aSum, newad));
	}
}
void funcThread22(){
	printf("Thread22~~\n");
	addfloat *aSum = _atoaddfloat.load();
	addfloat *newad;
	for(int i=0;i<10000;i++){
		do{
			newad = aSum;
			newad->add(5.0);
		}while(!_atoaddfloat.compare_exchange_strong(aSum, newad));
	}
}

int main(){
	/*
	cout<<*_atofSum.load()<<endl;
	float c[1] = {*_atofSum.load()};
	cout<< c[0] <<endl;
	c[0] = c[0] + 44.5;
	_atofSum.store(c);
	cout<<*_atofSum.load()<<endl;*/
	addfloat *MySum = new addfloat(21.5);
	_atoaddfloat = MySum;
	thread *threadptrMy;
	threadptrMy = new thread[2];
	
	threadptrMy[0] = thread(funcThread1); 
	threadptrMy[1] = thread(funcThread22); 
	threadptrMy[0].join(); 
	threadptrMy[1].join(); 
	/*addfloat *aSum = _atoaddfloat.load();
	aSum->add(10.1);
	aSum->show();*/
	MySum->show();
	
	return 0;
}
