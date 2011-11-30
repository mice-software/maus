#include <iostream>
#include "struct.h"
#include "DataDict.h"
#include <iostream>
#include "rstream.h"

using namespace std;

int main(){

  orstream file("Alex.root");

  B *ob1 = new B();
  B *ob2 = new B();
  B ob3;



  file<<branchName("AlexBranch1") <<ob1;
  file<<branchName("AlexBranch2") <<ob2;
  file<<branchName("AlexBranch3") <<ob3;
  
  ob1->a=1;
  ob1->b=2;

  ob2->a=5;
  ob2->b=6;

  ob3.a=12;
  ob3.b=14;

  file<<fillEvent;

  ob1->a=3;
  ob1->b=4;

  ob2->a=7;
  ob2->b=8;

  ob3.a=13;
  ob3.b=15;


  file<<fillEvent;
  file.close();

  // try reading in
  ob1->a=0;
  ob1->b=0;

  ob2->a=0;
  ob2->b=0;

  ob3.a=0;
  ob3.b=0;

  irstream file2("Alex.root");
  file2>>branchName("AlexBranch1") >>ob1;
  file2>>branchName("AlexBranch2") >>ob2;
  file2>>branchName("AlexBranch3") >>ob3;

  file2>>readEvent;

  cout<<"Event:"<<endl;
  cout<<"branch1: a = "<<ob1->a<<" b = "<<ob1->b<<endl;
  cout<<"branch2: a = "<<ob2->a<<" b = "<<ob2->b<<endl;
  cout<<"branch3: a = "<<ob3.a<<" b = "<<ob3.b<<endl;

  file2>>readEvent;
  cout<<"Event:"<<endl;
  cout<<"branch1: a = "<<ob1->a<<" b = "<<ob1->b<<endl;
  cout<<"branch2: a = "<<ob2->a<<" b = "<<ob2->b<<endl;
  cout<<"branch3: a = "<<ob3.a<<" b = "<<ob3.b<<endl;

  file2>>readEvent;
  file2.close();

  delete ob1;
  delete ob2;
  return 0;
}
