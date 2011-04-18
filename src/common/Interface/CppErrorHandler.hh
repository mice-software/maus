#include "json/json.h"

#include "Interface/Squeal.hh"

#ifndef CppErrorHandler_h 
#define CppErrorHandler_h 1

// TODO (Rogers): If I am worried about bad memory allocation I may not want to
//                pass by value...
//                Also would be nice to raise to python error handler (common
//                interface)
//                Need to think about how we handle "non recoverable" (meaning
//                memory leak in the Squeal) 

class CppErrorHandler {

 public:
  /** @brief Call default Cpp exception handler
   *
   *  For now this just calls Squeal::Print(), but at some point I intend to
   *  do something more sophisticated (mimicing or even using PyErrorHandler 
   *  functionality).
   *
   *  @param val Json document that will take any error
   *  @param exc the (MAUS Squeal) exception
   */
  static Json::Value HandleSqueal
                          (Json::Value val, Squeal exc, std::string class_name);

  /** @brief Call default Cpp exception handler
   *
   *  For now this just calls Squeal::Print(), but at some point I intend to
   *  do something more sophisticated (mimicing or even using PyErrorHandler 
   *  functionality).
   *
   *  @param exc the (MAUS Squeal) exception
   */
  static void HandleSquealNoJson(Squeal exc, std::string class_name);

  /** @brief Call default Cpp exception handler
   *
   *  For now this just prints std::exception::what(), but at some point I
   *  intend to do something more sophisticated.
   *
   *  @param val Json document that will take any error
   *  @param exc the std::exception
   */
  static Json::Value HandleStdExc
                  (Json::Value val, std::exception exc, std::string class_name);

  /** @brief Call default Cpp exception handler when Json not initialised
   *
   *  For now this just prints std::exception::what(), but at some point I
   *  intend to do something more sophisticated.
   *
   *  @param exc the std::exception
   */
  static void HandleStdExcNoJson(std::exception exc, std::string class_name);

 private:
  CppErrorHandler();
  ~CppErrorHandler();
  
};

#endif

