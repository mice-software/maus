#ifndef TEST_ONEARGMANIP_H
#define TEST_ONEARGMANIP_H
#include <cstring>
#include <TFile.h>
#include <TTree.h>

#include "gtest/gtest.h"

#include "src/common_cpp/JsonCppStreamer/RStream.hh"
#include "src/common_cpp/JsonCppStreamer/OneArgManip.hh"

class tmprstream:public rstream{
 public:
  tmprstream(const char* a,
	     const char* b,
	     MsgStream::LEVEL c):rstream(a,b,c){}
    void close(){m_file->Close();}
    bool operator!=(tmprstream& rs){
      return &rs!=this;//(m_file!=rs.m_file) || (strcmp(m_branchName,rs.m_branchName)) ||(m_tree!=rs.m_tree);
    }
    void operator=(const tmprstream& rs){
      m_file = rs.m_file;
      strcpy(m_branchName,rs.m_branchName);
      m_tree = rs.m_tree;
    }
};


TEST( IRStreamTest, TestConstructor ) {
    typedef rstream& (*manip_pointer)(rstream&,const char*);
    
    manip_pointer p = &rstream::setBranch;
    oneArgManip<const char*>* a = new oneArgManip<const char*>(p,
							       "TestConstructor");
  

    ASSERT_EQ(a->m_manip,p)<<"Fail: manip function pointers not initialised properly" << std::endl;

    ASSERT_EQ(a->m_value,"TestConstructor")<< "Fail: manip value not initialised properly"<<std::endl;

    delete a;
    a=0;
    std::cout<<"Passed"<<std::endl;
    return true;
}

//  bool test_operator(){
//     typedef rstream& (*manip_pointer)(rstream&,const char*);

//     manip_pointer p = &setBranch;
//     oneArgManip<const char*> a(p,"TestOperator");

//     tmprstream r1("TestFile","RECREATE",MsgStream::INFO);
//     tmprstream* r2 = reinterpret_cast<tmprstream*>(&a(r1));
 
//     if(*r2 != r1){
//       std::cout<<"Fail: rstream object not properly returned."<<std::endl;
//       return false;
//     }
//     if(strcmp(r1.m_branchName,"TestOperator")){
//       std::cout<<"Fail: manipulator value not passed properly"<<std::endl;
//       return false;
//     }


//     std::cout<<"Passed"<<std::endl;
//     return true;
//   }

//   bool test_branchName(){
//     oneArgManip<const char*>* a = branchName("TestBranchName");

//     if(!a){
//       std::cout<<"Fail: failed to construct a oneArgManip object."<<std::endl;
//       return false;
//     }
//     std::cout<<"Passed"<<std::endl;
//     return true;
//   }

#endif
