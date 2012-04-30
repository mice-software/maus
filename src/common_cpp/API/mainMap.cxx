#include <iostream>
#include "MapBase.hh"


class MyMapper: public MapBase<int, int>{
public:
  MyMapper():MapBase<int,int>("MyMapper"){}
  virtual ~MyMapper(){}
private:
  virtual void _birth(const std::string& s){
    std::cout<<"Frank"<<std::endl;
    std::cout<<_classname<<std::endl;
    //return true;
  }
  virtual void _death(){
    //return true;
  }
  
  virtual int* _process(int* t) const{
    //return new int(*t);
    return t;
  }


};


int main(){


  MyMapper m;

  m.birth("hello");

  int * i = new int(98);
  double * d = new double(76.5);
  
  int* ret1 = m.process(i);
  if(ret1){std::cout<<*ret1<<std::endl;}
  int* ret2 = m.process(d);
  if(ret2){std::cout<<*ret2<<std::endl;}

  delete i;
  delete d;
  //delete ret1; //i is ret 1 and passed straight through
  delete ret2;
  return 0;

}
