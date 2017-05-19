/////////////////////////////////////////////////////////////////////
// NoSqlDb.cpp - key/value pair in-memory database                 //
// ver 1.2                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////
#include "../NoSqlDb/NoSqlDb.h"
#include <iostream>

#ifdef TEST_NOSQLDB

using namespace NoSQLDB;
using StrData = std::string;
using intData = int;

int main()
{
  std::cout << "\n  Demonstrating NoSql Helper Code";
  std::cout << "\n =================================\n";

  std::cout << "\n  Creating and saving NoSqlDb elements with string data";
  std::cout << "\n -------------------------------------------------------\n";

  NoSqlDb<StrData> db;

  Element<StrData> elem1;
  elem1.name = "elem1";
  elem1.category = "test";
  elem1.data = "elem1's StrData";

  db.saveRecord(elem1.name, elem1);

  Element<StrData> elem2;
  elem2.name = "elem2";
  elem2.category = "test";
  elem2.data = "elem2's StrData";

  db.saveRecord(elem2.name, elem2);

  Element<StrData> elem3;
  elem3.name = "elem3";
  elem3.category = "test";
  elem3.data = "elem3's StrData";

  db.saveRecord(elem3.name, elem3);

  elem1.saveChild(elem2.name);
  elem1.saveChild(elem3.name);
  elem2.saveChild(elem3.name);
  db.saveValue(elem1.name, elem1);
  db.saveValue(elem2.name, elem2);

  std::cout << "\n  Retrieving elements from NoSqlDb<string>";
  std::cout << "\n ------------------------------------------\n";

  std::cout << "\n  size of db = " << db.count() << "\n";
  Keys keys = db.keys();
  for (Key key : keys)
  {
    std::cout << "\n  " << key << ":";
    std::cout << db.value(key).show();
  }

  std::cout << "\n  Erasing element with key elem2";
  if (db.removeRecord("elem2"))
    std::cout << "\n  removed record and all children references to this record";
  std::cout << "\n  size of db = " << db.count() << "\n";
  keys = db.keys();
  for (Key key : keys)
  {
    std::cout << "\n  " << key << ":";
    std::cout << db.value(key).show();
  }


  std::cout << "\n\n";

  std::cout << "\n  Creating and saving NoSqlDb elements with int data";
  std::cout << "\n ----------------------------------------------------\n";

  NoSqlDb<intData> idb;

  Element<intData> ielem1;
  ielem1.name = "elem1";
  ielem1.category = "test";
  ielem1.data = 1;

  idb.saveRecord(ielem1.name, ielem1);

  Element<intData> ielem2;
  ielem2.name = "elem2";
  ielem2.category = "test";
  ielem2.data = 2;

  idb.saveRecord(ielem2.name, ielem2);

  Element<intData> ielem3;
  ielem3.name = "elem3";
  ielem3.category = "test";
  ielem3.data = 3;

  idb.saveRecord(ielem3.name, ielem3);

  Element<intData> ielem4;
  ielem3.name = "elem4";
  ielem3.category = "test";
  ielem3.data = 4;

  idb.saveRecord(ielem3.name, ielem3);

  std::cout << "\n  Retrieving elements from NoSqlDb<int>";
  std::cout << "\n ---------------------------------------\n";

  std::cout << "\n  size of idb = " << idb.count() << "\n";
  Keys ikeys = idb.keys();
  for (Key key : ikeys)
  {
    std::cout << "\n  " << key << ":";
    std::cout << idb.value(key).show();
  }
  std::cout << "\n\n";
}
#endif
