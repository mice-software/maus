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
	     const char* b):rstream(a,b){}
    void close(){m_file->Close();}
    bool operator!=(tmprstream& rs){
      return &rs!=this;
    }
    void operator=(const tmprstream& rs){
      m_file = rs.m_file;
      strcpy(m_branchName,rs.m_branchName);
      m_tree = rs.m_tree;
    }
private:
  FRIEND_TEST(OneArgManipTest, TestOperator);

};


bool functionPointerPred(rstream& (*pointer1)(rstream&,const char*),
			 rstream& (*pointer2)(rstream&,const char*)){
  return pointer1==pointer2;
}

TEST( OneArgManipTest, TestConstructor ) {
    typedef rstream& (*manip_pointer)(rstream&,const char*);
    
    manip_pointer p = &rstream::setBranch;
    oneArgManip<const char*>* a = new oneArgManip<const char*>(p,
							       "TestConstructor");
  

    ASSERT_PRED2(functionPointerPred,a->m_manip,p)<<"Fail: manip function pointers not initialised properly" << std::endl;

    ASSERT_EQ(a->m_value,"TestConstructor")<< "Fail: manip value not initialised properly"<<std::endl;

    delete a;
    a=0;
}

bool charArrayPred(const char* a, const char* b){
  return !strcmp(a,b);
}

TEST( OneArgManipTest, TestOperator ) {
    typedef rstream& (*manip_pointer)(rstream&,const char*);

    manip_pointer p = &rstream::setBranch;
    oneArgManip<const char*> a(p,"TestOperator");

    tmprstream r1("TestFile","RECREATE");
    tmprstream* r2 = reinterpret_cast<tmprstream*>(&a(r1));
 
   
    ASSERT_FALSE(*r2 != r1) << "Fail: rstream object not properly returned."<<std::endl;

    ASSERT_PRED2(charArrayPred,r1.m_branchName,"TestOperator")<< "Fail: manipulator value not passed properly"<<std::endl;
}

TEST( OneArgManipTest, TestBranchName ) {

  oneArgManip<const char*>* a = branchName("TestBranchName");
  
  ASSERT_NE(a, static_cast<void*>(NULL))<<"Fail: failed to construct a oneArgManip object."<<std::endl;

}

#endif
