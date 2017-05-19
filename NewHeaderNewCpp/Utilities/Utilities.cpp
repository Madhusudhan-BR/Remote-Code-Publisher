///////////////////////////////////////////////////////////////////////
// Utilities.cpp - small, generally usefule, helper classes          //
// ver 1.1                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Most Projects, CSE687 - Object Oriented Design       //
// Author:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////

#include <cctype>
#include <iostream>
#include <sstream>
#include <cctype>
#include <locale>
#include "../Utilities/Utilities.h"

using namespace Utilities;

/////////////////////////////////////////////////////////////////////
// next two functions show how to create alias for function name

std::function<void(const std::string&)> Title =
  [](auto src) { StringHelper::Title(src, '='); };

std::function<void(const std::string&)> title =
  [](auto src) { StringHelper::Title(src, '-'); };

//----< write major title to console >-------------------------------

void StringHelper::title(const std::string& src)
{
  std::cout << "\n  " << src;
  std::cout << "\n " << std::string(src.size() + 2, '-');
}
//----< write minor title to console >-------------------------------

void StringHelper::Title(const std::string& src, char underline)
{
  std::cout << "\n  " << src;
  std::cout << "\n " << std::string(src.size() + 2, underline);
}
//----< convert comma separated list into vector<std::string> >------

std::vector<std::string> StringHelper::split(const std::string& src)
{
  std::vector<std::string> accum;
  std::string temp;
  size_t index = 0;
  do
  {
    while ((isspace(src[index]) || src[index] == ',') && src[index] != '\n')
    {
      ++index;
      if (temp.size() > 0)
      {
        accum.push_back(temp);
        temp.clear();
      }
    }
    if(src[index] != '\0')
      temp += src[index];
  } while (index++ < src.size());
  if(temp.size() > 0)
    accum.push_back(temp);
  return accum;
}
//----< remove leading and trailing whitespace >---------------------

std::string StringHelper::trim(const std::string& src)
{
  std::locale loc;
  std::string trimmed = src;
  size_t first = 0;
  while (true)
  {
    if (std::isspace(trimmed[first], loc))
      ++first;
    else
      break;
  }
  size_t last = trimmed.size() - 1;
  while (true)
  {
    if (std::isspace(trimmed[last], loc) && last > 0)
      --last;
    else
      break;

  }
  return trimmed.substr(first, last-first+1);
}
//----< wrap string in lines >---------------------------------------

std::string StringHelper::addHeaderAndFooterLines(const std::string& src)
{
  std::string line = "------------------------------";
  return line + "\n" + src + "\n" + line + "\n";
}
//----< takes any pointer type and displays as a dec string >--------

std::string Utilities::ToDecAddressString(size_t address)
{
  std::ostringstream oss;
  oss << std::uppercase << std::dec << address;
  return oss.str();
}
//----< takes any pointer type and displays as a hex string >--------

std::string Utilities::ToHexAddressString(size_t address)
{
  std::ostringstream oss;
  oss << std::uppercase << " (0x" << std::hex << address << ")";
  return oss.str();
}
//----< write newline to console >-----------------------------------

void Utilities::putline()
{
  std::cout << "\n";
}
//----< test stub >--------------------------------------------------

#ifdef TEST_UTILITIES

int main()
{
  Title("Testing Utilities Package");
  putline();

  title("test StringHelper::trim");

  std::string test1 = "  12345 ";
  std::cout << "\n  test string = \"" << test1 << "\"";

  test1 = StringHelper::trim(test1);
  std::cout << "\n  test string = \"" << test1 << "\"";
  putline();

  title("test StringHelper::split(std::string)");

  std::string test = "a, \n, bc, de, efg, i, j k lm nopq rst";
  std::cout << "\n  test string = " << test;
  
  std::vector<std::string> result = StringHelper::split(test);
  
  std::cout << "\n";
  for (auto item : result)
  {
    if (item == "\n")
      std::cout << "\n  " << "newline";
    else
      std::cout << "\n  " << item;
  }
  std::cout << "\n";

  title("test addHeaderAndFooterLines(const std::string&)");
  std::string test4 = "0123456789";
  std::cout << "\n" << StringHelper::addHeaderAndFooterLines(test4);

  title("test std::string Converter<T>::toString(T)");

  std::string conv1 = Converter<double>::toString(3.1415927);
  std::string conv2 = Converter<int>::toString(73);
  std::string conv3 = Converter<std::string>::toString("a_test_string plus more");

  std::cout << "\n  Converting from values to strings: ";
  std::cout << conv1 << ", " << conv2 << ", " << conv3;
  putline();

  title("test T Converter<T>::toValue(std::string)");

  std::cout << "\n  Converting from strings to values: ";
  std::cout << Converter<double>::toValue(conv1) << ", ";
  std::cout << Converter<int>::toValue(conv2) << ", ";
  std::cout << Converter<std::string>::toValue(conv3);

  std::cout << "\n\n";
  return 0;
}
#endif
