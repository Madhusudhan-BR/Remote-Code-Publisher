#pragma once
/////////////////////////////////////////////////////////////////////
// Convert.h - Demonstrate Template Syntax                         //
//                                                                 //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////
/*
 * - Template classes use one or more unspecified types, defined by
 *   arguments in the template declaration: template<typename T, ...>.
 *
 * - Because the arguments are not specified, the C++ compiler cannot
 *   generate object code until it sees an application instantiate
 *   the template type(s).
 *
 * - The consequence of this is that all template definitions must be
 *   placed in a header file, to be included by the application.
 *
 * - Unlike C# and Java, template syntax checking happens when the
 *   application is compiled.  
 *
 * - That means that a template class can use any methods for the 
 *   unspecified type and will successfully compile.  If the type does
 *   not have a method used by the template code, a compile time error
 *   occurs when the application is compiled.
 */
#include <sstream>
#include <string>

/////////////////////////////////////////////////////////////////////
// Convert<T> class
// - (de)serializes objects of type T (from)to strings
// - generates a type identifier using Run-Time Type Identification
//
template<typename T>
class Convert
{
public:
  virtual ~Convert<T>() {}
  static std::string toString(const T& t);
  static T fromString(const std::string& str);
  static std::string id(const T& t);
};
/*----< serialize t to a std::string >-----------------------------*/
/*
 * Assumes that T defines an insertion operator.
 */
template<typename T>
std::string Convert<T>::toString(const T& t)
{
  std::ostringstream out;
  out << *const_cast<T*>(&t);  
  return out.str();
}
/*----< create an instance of T from a std::string >---------------*/
/*
 * Assumes that T defines an extraction operator.
 * To succeed str must be created from Convert<T>::toString(const T& t). 
 */
template<typename T>
T Convert<T>::fromString(const std::string& str)
{
  std::istringstream in(str);
  T value;
  in >> value;    // istringstream extraction provides the conversion
  return value;
}
/*----< create an id string from an instance of T using RTTI >----*/

template<typename T>
std::string Convert<T>::id(const T& t)
{
  return typeid(t).name();
}
