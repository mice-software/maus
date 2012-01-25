#ifndef TEST_RSTREAM_H
#define TEST_RSTREAM_H
#include <cstring>
#include <TFile.h>
#include <TTree.h>

#include "gtest/gtest.h"

#include "src/common_cpp/JsonCppStreamer/RStream.hh"

class temprstream : public rstream{
 public:
  temprstream(const char* a,
	     const char* b,
	     MsgStream::LEVEL c):rstream(a,b,c){}
    void close(){m_file->Close();}
 private:
  FRIEND_TEST(RStreamTest, TestConstructor);
  FRIEND_TEST(RStreamTest, TestIsOpen);
  FRIEND_TEST(RStreamTest, TestSetBranch);
};

TEST(RStreamTest, TestConstructor) {
    temprstream a("TestFile","RECREATE",MsgStream::INFO);
    ASSERT_NE(a.m_file, static_cast<void*>(NULL)) <<"Fail: file object not correctly created."<<std::endl;
    ASSERT_NE(a.m_branchName, static_cast<void*>(NULL)) <<"Fail: branch name memory not correctly allocated"<<std::endl;
    ASSERT_NE(a.m_branchName,"")<<"Fail. branch name not set to default value."<<std::endl;
}

TEST(RStreamTest, TestIsOpen) {
    temprstream a("TestFile","RECREATE",MsgStream::INFO);
    ASSERT_TRUE(a.is_open())<<"Fail: reporting file closed when open"<<std::endl;
    TFile * f = a.m_file;
    a.m_file=0;
    ASSERT_FALSE(a.is_open())<<"Fail: reporting file open when null pointer."<<std::endl;
    a.m_file=f;
    a.close();
    ASSERT_FALSE(a.is_open())<<"Fail: reporting file open when closed."<<std::endl;
}

TEST(RStreamTest, TestSetBranch) {
    temprstream a("TestFile","RECREATE",MsgStream::INFO);
    rstream::setBranch(a,"TestingBranch");

    ASSERT_EQ(std::string(a.m_branchName),"TestingBranch")<<"Fail: Branch name not set correctly."<<std::endl;
}


#endif
