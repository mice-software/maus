#include <exception>
#include <string>

namespace MAUS {

  class NullInputException: public std::exception {
  public:
    NullInputException(const std::string& classname) : std::exception(), _classname(classname){}
    virtual ~NullInputException() throw() {}
    virtual const char* what() const throw() {
      std::string ret = "The input to '";
      ret            += _classname;
      ret            += "' was a null pointer";
      return ret.c_str();
    }
  private:
    std::string _classname;
  };
  
  class UnhandledException: public std::exception {
  public:
    UnhandledException(const std::string& classname) : std::exception(), _classname(classname){}
    virtual ~UnhandledException() throw() {}
    virtual const char* what() const throw() {
      std::string ret = "An unhandled exception was thrown by '";
      ret            += _classname;
      ret            += "' which was not of type 'Squeal' or deriving from 'std::exception'";
      return ret.c_str();
    }
  private:
    std::string _classname;
  };
  
}//end of namespace
