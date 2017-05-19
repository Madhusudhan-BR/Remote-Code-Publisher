#ifndef UTILITIES_H
#define UTILITIES_H
///////////////////////////////////////////////////////////////////////
// Utilities.h - small, generally useful, helper classes             //
// ver 1.4                                                           //
// Language:    C++, Visual Studio 2015                              //
// Platform:    Dell XPS 8900, Windows 10                            //
// Application: Most Projects, CSE687 - Object Oriented Design       //
// Author:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides classes StringHelper and Converter and a global
* function putline().  This class will be extended continuously for 
* awhile to provide convenience functions for general C++ applications.
*
* Build Process:
* --------------
* Required Files: Utilities.h, Utilities.cpp
*
* Build Command: devenv Utilities.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.4 : 26 Feb 2017
* - changed StringHelper::split to use isspace from <locale> instead of <cctype>
* ver 1.3 : 12 Aug 2016
* - added overloads of title functions for std::ostringstreams
* ver 1.2 : 10 Aug 2016
* - added new function StringHelper::sTitle(...)
* - changed the way string arguments are passed to each of the
*   title functions
* ver 1.1 : 06 Feb 2015
* - fixed bug in split which turns a comma separated string into
*   a vector of tokens.
* - added comments
* ver 1.0 : 05 Feb 2016
* - first release
*
* Planned Additions and Changes:
* ------------------------------
* - none yet
*/
#include<iostream>
#include <string>
#include <vector>
#include <sstream>
#include <functional>

namespace Utilities
{
  class test
  {
  public:
    std::vector<std::string> publicVector;
  };

  class StringHelper
  {
  public:
    static std::vector<std::string> split(const std::string& src);
    static void Title(std::string src, std::ostream&  out = std::cout);
    static void Title(std::string src, std::ostringstream& out);
    static void title(std::string src, std::ostream& out = std::cout);
	static std::string trim(const std::string& src);
    static void title(std::string src, std::ostringstream& out);
    static void sTitle(
      std::string src, size_t offset, size_t width, std::ostream& out = std::cout, char underline = '-'
    );
    static void sTitle(
      std::string src, size_t offset, size_t width, std::ostringstream& out, char underline = '-'
    );
  };

  void putline();

  template <typename T>
  class Converter
  {
  public:
    static std::string toString(const T& t);
    static T toValue(const std::string& src);
  };

  template <typename T>
  std::string Converter<T>::toString(const T& t)
  {
    std::ostringstream out;
    out << t;
    return out.str();
  }

  template<typename T>
  T Converter<T>::toValue(const std::string& src)
  {
    std::istringstream in(src);
    T t;
    in >> t;
    return t;
  }
}
#endif
