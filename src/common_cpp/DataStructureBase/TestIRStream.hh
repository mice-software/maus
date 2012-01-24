#ifndef TEST_IRSTREAM_H
#define TEST_IRSTREAM_H
#include "TestBase.h"
#include "rstream.h"

class test_irstream: public TestBase<test_irstream>{
 private:
  bool test_constructor(){
    TFile f("TestFile.root","RECREATE");
    TTree t("TestTree","treetitle");
    t.Write();
    f.Close();
    irstream is("TestFile.root","TestTree");

    if(is.m_evtCount !=0){
      std::cout<<"Fail: event counter not initialised properly"<<std::endl;
      return false;
    }
    if(!is.m_file){
      std::cout<<"Fail: file object not initialised properly"<<std::endl;
      return false;
    }
    if(!is.m_tree){
      std::cout<<"Fail: tree object not initialised properly"<<std::endl;
      return false;
    }

    // irstream close but dont want to introduce dependence on the close method in unit test
    is.m_file->Close();
    strcpy(is.m_branchName,"");
    delete is.m_file;
    //delete m_tree; //crashes root?! ROOT must do this step automatically as part of closing file
    is.m_file=0;
    is.m_tree=0;
    /////////////////////////////////

    std::cout<<"Passed"<<std::endl;
    return true;
  }
  bool test_open(){
    TFile f("TestFile.root","RECREATE");
    TTree t("TestTree","treetitle");
    t.Write();
    f.Close();
    irstream is("TestFile.root","TestTree");
    // irstream close but dont want to introduce dependence on the close method in unit test
    is.m_file->Close();
    strcpy(is.m_branchName,"Test");
    delete is.m_file;
    //delete m_tree; //crashes root?! ROOT must do this step automatically as part of closing file
    is.m_file=0;
    is.m_tree=0;
    is.m_evtCount=100;
    /////////////////////////////////

    is.open("TestFile.root","TestTree");

    if(is.m_evtCount !=0){
      std::cout<<"Fail: event counter not reset"<<std::endl;
      return false;
    }
    if(!is.m_file){
      std::cout<<"Fail: file object not initialised properly"<<std::endl;
      return false;
    }
    if(!is.m_tree){
      std::cout<<"Fail: tree object not initialised properly"<<std::endl;
      return false;
    }
    if(strcmp(is.m_branchName,"")){
      std::cout<<"Fail: branch name not reset"<<std::endl;
      return false;
    }


    // irstream close but dont want to introduce dependence on the close method in unit test
    is.m_file->Close();
    strcpy(is.m_branchName,"");
    delete is.m_file;
    //delete m_tree; //crashes root?! ROOT must do this step automatically as part of closing file
    is.m_file=0;
    is.m_tree=0;
    /////////////////////////////////

    std::cout<<"Passed"<<std::endl;
    return true;
  }
  bool test_close(){
    TFile f("TestFile.root","RECREATE");
    TTree t("TestTree","treetitle");
    t.Write();
    f.Close();
    irstream is("TestFile.root","TestTree");

    is.close();

    if(strcmp(is.m_branchName,"")){
      std::cout<<"Fail: did not reset the branch name."<<std::endl;
      return false;
    }
    if(is.m_file){
      std::cout<<"Fail: did not delete the file object."<<std::endl;
      return false;
    }
    if(is.m_tree){
      std::cout<<"Fail: did not delete the tree object"<<std::endl;
      return false;
    }

    std::cout<<"Passed"<<std::endl;
    return true;
  }

  bool test_readevent(){
    TFile f("TestFile.root","RECREATE");
    TTree t("TestTree","treetitle");
    Int_t* a = new Int_t(17);
    Int_t* b = new Int_t(23);
    t.Branch("testA",a,"testA/I");
    t.Branch("testB",b,"testB/I");
    t.Fill();
    *a=26;
    *b=19;
    t.Fill();
    t.Write();
    f.Close();
    
    a=new (a) int(0);
    delete b;

    irstream is("TestFile.root","TestTree");
    int c=0;    
    is>>branchName("testA")>>a;
    is>>branchName("testB")>>c;

    int evtCount=0;    
    while(is>>readEvent){
      ++evtCount;
      if(evtCount!= is.m_evtCount){
	std::cout<<"Fail: didn't increment the read event counter properly."<<std::endl;
	return false;	
      }
      if(*a !=17 && *a!=26){
	std::cout<<"Fail: didn't read in the correct values from pointer."<<std::endl;
	return false;
      }
      if(c !=23 && c!=19){
	std::cout<<"Fail: didn't read in the correct values from non-pointer."<<std::endl;
	return false;
      }
    }
    
    // irstream close but dont want to introduce dependence on the close method in unit test
    is.m_file->Close();
    strcpy(is.m_branchName,"");
    delete is.m_file;
    //delete m_tree; //crashes root?! ROOT must do this step automatically as part of closing file
    is.m_file=0;
    is.m_tree=0;
    /////////////////////////////////
    delete a;
    std::cout<<"Passed"<<std::endl;  
    return true;
  }
  
 public:
  test_irstream():TestBase<test_irstream>(this){
    m_tests.push_back(&test_irstream::test_constructor);
    m_tests.push_back(&test_irstream::test_open);
    m_tests.push_back(&test_irstream::test_close);
    m_tests.push_back(&test_irstream::test_readevent);
  }

};

#endif
