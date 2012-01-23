#include <fstream>
#include "RootStreamer/rstream.h"
#include "JsonCppConverter.h"

int main(){
  // Json -> cpp -> root test
  ////////////////////////////////////////////////////////////////////
  //std::ifstream infile("/home/hep/arichard/tmp/pythontest/new/data/twoeventsimulation.json");
  std::ifstream infile("/home/hep/arichard/tmp/pythontest/new/data/tmp.json");

  orstream outfile("test.root","MausData");
  Digits* d = new Digits();
  MC* mc = new MC();
  MausData * md = new MausData(d,mc);
  outfile<<branchName("digits")<< d;
  outfile<<branchName("mc")<< mc;
 

  JsonCppConverter c(md);
  std::string line;
  while(getline(infile,line)){
    c(line);
    outfile<<fillEvent;
  }

  outfile.close();
  infile.close();
  delete md;


  // root -> cpp -> Json test
  ////////////////////////////////////////////////////////////////////
  irstream rootinfile("test.root","MausData");
  d = new Digits();
  mc = new MC();
  md = new MausData(d,mc);
  rootinfile>> branchName("digits")>> d;
  rootinfile>>branchName("mc")>> mc;

  Json::Value jv(Json::objectValue);
  ofstream bout("tmpNew.json");
  JsonCppConverter cp(&jv);
  while(rootinfile>>readEvent){
    cp(*md);
    bout<<jv.toStyledString()<<std::endl;
    jv.clear();
  }
  
  bout.close();
  rootinfile.close();
  delete md;

  return 0;
}
