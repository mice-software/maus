//Compile with
//g++ -o testmain testmain.cxx -DTESTING -L. -lrstream -I. `root-config --libs --cflags` `alex-config --libs --cflags MsgStream`
#include "test_rstream.h"
#include "test_oneArgManip.h"
#include "test_orstream.h"
#include "test_irstream.h"

int main(){

  test_oneArgManip a;
  test_rstream b;
  test_orstream c;
  test_irstream d;
  std::cout<<"running test_oneArgManip..."<<std::endl;
  std::cout<<std::boolalpha<<"Tests passed = "<<a.run()<<"\n"<<std::endl;
  std::cout<<"running test_rstream..."<<std::endl;
  std::cout<<std::boolalpha<<"Tests passed = "<<b.run()<<"\n"<<std::endl;
  std::cout<<"running test_orstream..."<<std::endl;
  std::cout<<std::boolalpha<<"Tests passed = "<<c.run()<<"\n"<<std::endl;
  std::cout<<"running test_irstream..."<<std::endl;
  std::cout<<std::boolalpha<<"Tests passed = "<<d.run()<<"\n"<<std::endl;

  return 0;
}
