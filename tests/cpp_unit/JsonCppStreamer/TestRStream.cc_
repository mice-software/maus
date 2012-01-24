#ifndef TEST_RSTREAM_H
#define TEST_RSTREAM_H
#include "TestBase.h"
#include <cstring>
#include <TFile.h>
#include <TTree.h>

/* #define EXINCL */
/* #define TESTING */
/* #define EXSETBRANCH */
 #include "rstream.h"

class temprstream:public rstream{
 public:
  temprstream(const char* a,
	     const char* b,
	     MsgStream::LEVEL c):rstream(a,b,c){}
    void close(){m_file->Close();}
    friend class test_rstream;
    
};

class test_rstream : public TestBase<test_rstream>{
 private:
  bool test_constructor(){
    temprstream a("TestFile","RECREATE",MsgStream::INFO);
    if(!a.m_file){
      std::cout<<"Fail: file object not correctly created."<<std::endl;
      return false;
    }
    if(!a.m_branchName){
      std::cout<<"Fail: branch name memory not correctly allocated"<<std::endl;
      return false;
    }
    if(strcmp(a.m_branchName,"")){
      std::cout<<"Fail. branch name not set to default value."<<std::endl;
      return false;
    }
    std::cout<<"Passed"<<std::endl;
    return true;
  }

  bool test_is_open(){
    temprstream a("TestFile","RECREATE",MsgStream::INFO);
    if(!a.is_open()){
      std::cout<<"Fail: reporting file closed when open"<<std::endl;
      return false;
    }
    TFile * f = a.m_file;
    a.m_file=0;
    if(a.is_open()){
      std::cout<<"Fail: reporting file open when null pointer."<<std::endl;
      return false;
    }
    a.m_file=f;
    a.close();
    if(a.is_open()){
      std::cout<<"Fail: reporting file open when closed."<<std::endl;
      return false;
    }
    std::cout<<"Passed"<<std::endl;
    return true;
  }
  bool test_setBranch(){
    temprstream a("TestFile","RECREATE",MsgStream::INFO);
    setBranch(a,"TestingBranch");

    if(strcmp(a.m_branchName,"TestingBranch")){
      std::cout<<"Fail: Branch name not set correctly."<<std::endl;
      return false;
    }

    std::cout<<"Passed"<<std::endl;
    return true;
  }
 public:
  test_rstream(): TestBase<test_rstream>(this){
    m_tests.push_back(&test_rstream::test_constructor);
    m_tests.push_back(&test_rstream::test_is_open);
    m_tests.push_back(&test_rstream::test_setBranch);
  }
};

#endif
