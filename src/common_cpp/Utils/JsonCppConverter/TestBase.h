#ifndef TESTBASE_H
#define TESTBASE_H
#include <vector>

template<class T>
class TestBase{
 private:
  typedef bool (T::*test_pointer)();
  T* m_t;
 protected:
  std::vector<test_pointer> m_tests;
  
 public:
  TestBase(T* t):m_t(t){}
  virtual ~TestBase(){}
  virtual bool run(){
    bool r = true;
    typename std::vector<test_pointer>::iterator tItr;
    for(tItr=m_tests.begin();tItr!=m_tests.end();++tItr){
      r = r && (m_t->*(*tItr))();
    }
    return r;
  }

};

#endif
