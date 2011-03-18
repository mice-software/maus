#include "json/json.h"

#include "Interface/Squeal.hh"

#ifndef CppErrorHandler_h 
#define CppErrorHandler_h 1

// TODO (Rogers): If I am worried about bad memory allocation I may not want to
//                pass by value...

class CppErrorHandler {

 public:
  /** \brief Call default Cpp exception handler
   *
   *  \param val Json document that will take any error
   *  \param exc the (MAUS Squeal) exception
   *
   *  For now this just calls Squeal::Print(), but at some point I intend to
   *  do something more sophisticated (mimicing or even using PyErrorHandler 
   *  functionality).
   */
  static Json::Value HandleSqueal
                          (Json::Value val, Squeal exc, std::string class_name);

  /** \brief Call default Cpp exception handler
   *
   *  \param exc the (MAUS Squeal) exception
   *
   *  For now this just calls Squeal::Print(), but at some point I intend to
   *  do something more sophisticated (mimicing or even using PyErrorHandler 
   *  functionality).
   */
  static void HandleSquealNoJson(Squeal exc, std::string class_name);

  /** \brief Call default Cpp exception handler
   *
   *  \param val Json document that will take any error
   *  \param exc the std::exception
   *
   *  For now this just prints std::exception::what(), but at some point I
   *  intend to do something more sophisticated.
   */
  static Json::Value HandleStdExc
                  (Json::Value val, std::exception exc, std::string class_name);

  /** \brief Call default Cpp exception handler when Json not initialised
   *
   *  \param exc the std::exception
   *
   *  For now this just prints std::exception::what(), but at some point I
   *  intend to do something more sophisticated.
   */
  static void HandleStdExcNoJson(std::exception exc, std::string class_name);

 private:
  CppErrorHandler();
  ~CppErrorHandler();
  
};

#endif

