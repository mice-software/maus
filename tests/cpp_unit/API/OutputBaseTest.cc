#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"
#include "src/common_cpp/API/OutputBase.hh"

namespace MAUS {


  class MyOutputter: public OutputBase<int>{
  public:
    MyOutputter():OutputBase<int>("TestClass"){}
    MyOutputter(const MyOutputter& mr) : OutputBase<int>(mr){}
    virtual ~MyOutputter(){}
    
  private:
    virtual void _birth(const std::string&){}
    virtual void _death(){}
    
    virtual bool _save(int* i) {
      return *i==27? true: false ;
    }
    
  private:
    FRIEND_TEST(OutputBaseTest, TestConstructor);
    FRIEND_TEST(OutputBaseTest, TestCopyConstructor);  
  };
  
  class MyOutputter_squeal: public MyOutputter{
  public:
    MyOutputter_squeal():MyOutputter(){}
    
  private:
    virtual bool _save(int*) {
      throw Squeal(Squeal::recoverable,
		   "Expected Test Squeal in _save",
		   "int* _save(int* t) const");
    }
  };
  
  class MyOutputter_exception: public MyOutputter{
  public:
    MyOutputter_exception():MyOutputter(){}
    
  private:
    virtual bool _save(int*) {
      throw std::exception();
    }
  };

  class MyOutputter_otherexcept: public MyOutputter{
  public:
    MyOutputter_otherexcept():MyOutputter(){}
    
  private:
    virtual bool _save(int*) {throw 17;}
  };
  
  
  TEST(OutputBaseTest, TestConstructor) {
    MyOutputter m;
    
    ASSERT_FALSE(strcmp("TestClass",m._classname.c_str()))
      << "Fail: Constructor failed, Classname not set properly"
      << std::endl;
  }
  
  TEST(OutputBaseTest, TestCopyConstructor) {
    MyOutputter tc1;
    MyOutputter tc2(tc1);

    ASSERT_FALSE(strcmp("TestClass",tc2._classname.c_str()))
      << "Fail: Copy Constructor failed, Classname not set properly"
      << std::endl;
  }
  
  TEST(OutputBaseTest, TestBirth) {
    MyOutputter tc1;
    try{
      tc1.birth("TestConfig");
    }
    catch(...){
      ASSERT_TRUE(false)
	<<"Fail: Birth function failed. Check ModuleBaseTest"
	<< std::endl;
    }
  }

  TEST(OutputBaseTest, TestDeath) {
    MyOutputter tc1;
    try{
      tc1.death();
    }
    catch(...){
      ASSERT_TRUE(false)
	<<"Fail: Death function failed. Check ModuleBaseTest"
	<< std::endl;
    }
  }

  TEST(OutputBaseTest, TestSave) {
    MyOutputter mm;
    
    int* i1 = new int(27);
    int* i2 = new int(19);
    
    ASSERT_TRUE( mm.save(i1) )
      <<"Fail: _save method not called properly didn't return 'true' for save(new int(27))"
      <<std::endl;

    ASSERT_FALSE( mm.save(i2) )
      <<"Fail: _save method not called properly didn't return 'false' for save(new int(19))"
      <<std::endl;
    

    /////////////////////////////////////////////////////
    MyOutputter mm2;
    try{
      int* dub = 0;
      mm2.save(dub);
      ASSERT_TRUE(false)
	<< "Fail: No exception thrown"
	<< std::endl;
    }
    catch(NullInputException& e){}
    catch(...){
      ASSERT_TRUE(false)
	<< "Fail: Expected exception of type NullInputException to be thrown"
	<< std::endl;      
    }
    /////////////////////////////////////////////////////
    MyOutputter_squeal mm_s;
    try{
      mm_s.save(i1);
    }
    catch(...){
      ASSERT_TRUE(false)
	<< "Fail: Squeal should have been handled"
	<< std::endl;
    }
    
    /////////////////////////////////////////////////////
    MyOutputter_exception mm_e;
    try{
      mm_e.save(i1);
    }
    catch(...){
      ASSERT_TRUE(false)
	<< "Fail: Exception should have been handled"
	<< std::endl;     
   }

    /////////////////////////////////////////////////////
    MyOutputter_otherexcept mm_oe;
    try{
      mm_oe.save(i1);
      ASSERT_TRUE(false)
	<< "Fail: No exception thrown"
	<< std::endl;
    }
    catch(UnhandledException& e){}
    catch(...){
      ASSERT_TRUE(false)
	<< "Fail: Expected exception of type UnhandledException to be thrown"
	<< std::endl;      
    }

    delete i1;
    delete i2;
  }


}//end of namespace
