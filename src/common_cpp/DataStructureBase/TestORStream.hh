#ifndef TEST_ORSTREAM_H
#define TEST_ORSTREAM_H
#include "TestBase.h"
#include "rstream.h"

class test_orstream: public TestBase<test_orstream>{
 private:
  bool test_constructor(){
    orstream os("TestFile.root","TestTree","TestTreeTitle","RECREATE");
    if(!os.m_file){
      std::cout<<"Fail: super class not initialised properly."<<std::endl;
      return false;
    }
    if(os.m_evtCount!=0){
      std::cout<<"Fail: event counter not initialised properly"<<std::endl;
      return false;
    }
    if(!os.m_tree){
      std::cout<<"Fail: tree not properly initialised"<<std::endl;
      return false;
    }

    // orstream close but dont want to introduce dependence on the close method in unit test
    os.m_file->Close();
    delete os.m_file;
    //delete os.m_tree;//crashes root?! ROOT must do this step automatically as part of closing file.
    os.m_file=0;
    os.m_tree=0;
    strcpy(os.m_branchName,"");
    os.m_evtCount=0;
    ///////////////////////////////

    std::cout<<"Passed"<<std::endl;
    return true;
  }
  bool test_open(){
    orstream os("TestFile.root","TestTree","TestTreeTitle","RECREATE");
    // orstream close but dont want to introduce dependence on the close method in unit test
    os.m_file->Close();
    delete os.m_file;
    //delete os.m_tree;//crashes root?! ROOT must do this step automatically as part of closing file.
    os.m_file=0;
    os.m_tree=0;
    strcpy(os.m_branchName,"Test");
    os.m_evtCount=100;
    //////////////////////////////

    os.open("TestFile.root","TestTree","TestTreeTitle","RECREATE");
    if(!os.m_file){
      std::cout<<"Fail: file object not created correctly."<<std::endl;
      return false;
    }
    if(!os.m_tree){
      std::cout<<"Fail: tree object not created correctly."<<std::endl;
      return false;
    }
    if(strcmp(os.m_branchName,"")){
      std::cout<<"Fail: branch name not reset"<<std::endl;
      return false;
    }
    if(os.m_evtCount!=0){
      std::cout<<"Fail: event counter not reset"<<std::endl;
      return false;
    }

    // orstream close but dont want to introduce dependence on the close method in unit test
    os.m_file->Close();
    delete os.m_file;
    //delete os.m_tree;//crashes root?! ROOT must do this step automatically as part of closing file.
    os.m_file=0;
    os.m_tree=0;
    strcpy(os.m_branchName,"");
    os.m_evtCount=0;
    ///////////////////////////////

    std::cout<<"Passed"<<std::endl;
    return true;
  }
  bool test_close(){
    orstream os("TestFile.root","TestTree","TestTreeTitle","RECREATE");
    strcpy(os.m_branchName,"HelloWorld");
    os.close();
    if(os.m_file){
      std::cout<<"Fail: file object not deleted properly."<<std::endl;
      return false;
    }
    if(os.m_tree){
      std::cout<<"Fail: tree object not deleted properly."<<std::endl;
      return false;
    }
    if(strcmp(os.m_branchName,"")){
      std::cout<<"Fail: branch name not reset"<<std::endl;
      return false;
    }

    std::cout<<"Passed"<<std::endl;
    return true;
  }
  bool test_fillevent(){
    int a=25;
    int *b = new int(72);
    orstream os("TestFile.root","TestTree","TestTreeTitle","RECREATE");
    os<<branchName("TestBranchA")<<a;
    os<<branchName("TestBranchB")<<b;
    os<<fillEvent;
    a=14;
    *b=22;
    os<<fillEvent;

    if( os.m_tree->GetEntriesFast() !=2){
      std::cout<<"Fail: root tree not filled properly."<<std::endl;
      return false;
    }
    if(os.m_evtCount!=2){
      std::cout<<"Fail: event counted not incremented properly."<<std::endl;
      return false;
    }

    os.m_tree->Write();
    // orstream close but dont want to introduce dependence on the close method in unit test
    os.m_file->Close();
    delete os.m_file;
    //delete os.m_tree;//crashes root?! ROOT must do this step automatically as part of closing file.
    os.m_file=0;
    os.m_tree=0;
    strcpy(os.m_branchName,"");
    os.m_evtCount=0;
    /////////////////////////////////////////

    int * c = new int(0);
    *b=0;

    TFile f("TestFile.root");
    TTree* tt = (TTree*)f.Get("TestTree");
    if(!tt){
	std::cout<<"Fail: couldn't find tree in the file."<<std::endl;
	return false;
    }

    tt->SetBranchAddress("TestBranchA",c);
    tt->SetBranchAddress("TestBranchB",b);

    long nevent=0;

    while(nevent<tt->GetEntriesFast()){

      tt->GetEntry(nevent);

      if(*c!=25 && *c!=14){
	std::cout<<"Fail: incorrect value stored with non-pointer"<<std::endl;
	return false;
      }
      if(*b!=72 && *b!=22){
	std::cout<<"Fail: incorrect value stored with pointer"<<std::endl;
	return false;
      }
      
      ++nevent;
    }


    std::cout<<"Passed"<<std::endl;
    return true;
  }
  
 public:
  test_orstream():TestBase<test_orstream>(this){
    m_tests.push_back(&test_orstream::test_constructor);
    m_tests.push_back(&test_orstream::test_open);
    m_tests.push_back(&test_orstream::test_close);
    m_tests.push_back(&test_orstream::test_fillevent);
  }

};

#endif
