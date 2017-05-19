#pragma once
/////////////////////////////////////////////////////////////////////
// CppProperties.h - Defines read/write and read only properties   //
// ver 1.1                                                         //
// Source: Michael Litvin (see link below)                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////
//
// Got the idea from discussion here (scroll down about halfway):
// http://stackoverflow.com/questions/8368512/does-c11-have-c-style-properties
// This implementation is quite different however.
/*
 * Package Operations:
 * -------------------
 * This package provides two classes, Property<T>, and Property<const T>,
 * a template specialization of Property<T>.
 *
 * Both classes wrap an instance of the template type and provide methods:
 * - Property<T>& operator=(const T& t) - sets internal value by assignment
 * - operator const T() - gets copy of wrapped type 
 *   (properties only changed by assignment)
 * - T& getValue() - returns a reference to wrapped instance so you can change
 *   in a non-property way.  
 *   Without getValue() you need to get the copy with the cast operation, change it
 *   then assign the copy back to the property.
 *
 * Note:
 * -----
 *   These properties provide virtual methods to support changing the
 *   operation of getter and setter.  But, you don't usually want to
 *   override because that changes the name of the property.  For example
 *   see the Y class defined in CppProperties.cpp.
 *
 *   Instead, you should prefer to use the methods to provide new getter
 *   and setter functions to change the property behaviors, as that
 *   does not change the user's interface. That's illustrated in the test stub.
 *
 * Maintenance History:
 * --------------------
 * ver 1.1 : Jan 28 2017
 * - fixed bug in Property class by removing bound data from all lambdas.
 * ver 1.0 : Jan 08 2017
 * - first release
 *
*/
#include <iostream>
#include <functional>

/////////////////////////////////////////////////////////////////////
// Property<T> class provides encapsulated properties for C++
// - getter and setter functions can be customized without changing
//   the client interface for any class that uses Property<T>
//
template<typename T>
class Property
{
public:
  using SetterType = std::function<T(const T& t)>;
  using GetterType = std::function<T(const T& t)>;

  // initialize the property field

  Property() : value(T()) {}
  Property(const T& t) : value(t) {}
  virtual ~Property() {}
  
  // set the property field with assignment operator

  virtual Property<T>& operator=(const T &t)
  { 
    value = setter_(t); 
    return *this; 
  }
  // get the property field with cast operator --- returns copy !

  virtual operator const T() const { return getter_(value); }
  
  // define methods to customize getter and setter behaviors

  void newSetter(SetterType sttr) { setter_ = sttr; }
  void newGetter(GetterType gttr) { getter_ = gttr; }

  T& getValue() { return value; }  // You can get reference to value using getValue()

protected:
  T value;  // property field

  // define default getter and setter behavior using lambdas

  SetterType setter_ = [](const T& t) { return t; };   // default setter
  GetterType getter_ = [](const T& v) { return v; };   // default getter
};

/*----< overload insertion operator for std::string properties >---*/

inline std::ostream& operator<<(std::ostream& out, Property<std::string>& p)
{
  std::string temp = p;
  try {
    out << (temp).c_str();
  }
  catch (std::exception& ex)
  {
    std::cout << "\n  exception:\n  " << ex.what() << "\n";
  }
  return out;
}

/////////////////////////////////////////////////////////////////////
// Property<const T> class provides encapsulated properties for C++
// - getter function can be customized without changing
//   the client interface for any class that uses Property<T>
// - no setter is provided since the class is used for const properties
// - comments for this class would be same as above, and so are omitted
//
template<typename T>
class Property<const T>
{
public:
  using SetterType = std::function<T(const T&t)>;
  using GetterType = std::function<T(const T&t)>;

  Property() : value(T()) {}
  Property(const T& t) : value(t) {}
  virtual ~Property() {}
  void newGetter(GetterType gttr) { getter_ = gttr; }
  virtual operator const T() const { return getter_(value); }
  T getValue() { return value; }                      // used by custom getters
protected:
  T value;
  GetterType getter_ = [](const T& v) { return v; };  // default getter
};

/*----< overload insertion operator for std::string properties >---*/

inline std::ostream& operator<<(std::ostream& out, Property<const std::string>& p)
{
  std::string temp = p;
  try {
    out << (temp).c_str();
  }
  catch (std::exception& ex)
  {
    std::cout << "\n  exception:\n  " << ex.what() << "\n";
  }
  return out;
}

