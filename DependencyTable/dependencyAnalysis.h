#pragma once
/////////////////////////////////////////////////////////////////////
// DependencyAnalysis.h -								           //
// ver 1                                                           //
//-----------------------------------------------------------------//
// Madhusudhan B R SUID-914198627                                  //
// All rights granted provided this copyright notice is retained   //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Platform:    Mac 15inch   , Windows 10                          //
// Application: Project #2, CSE687 - Object Oriented Design, S2017 //
// Author:      Madhusudhan.B.R, Syracuse University.		       //
//									                               //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package defines one class that has dependencyAnalysis function
* that does dependency analysis and stores it in the DB
*   
*   
* - manual information
* - maintenance information
* 
* 
* 
*
* Required Files:
* ---------------
*   
*
* Maintenance History:
*---------------------
* ver 1.0 : 03-10-2017
* 
* 
* 
* 
* 
*   
*   
* 
* - first release
*/
#pragma warning(disable : 4503)
#include <iostream>
#include <locale>
#include <string>
#include <vector>
#include "../Tokenizer/Tokenizer.h"
#include "../Utilities/Utilities.h"
#include "../TypeTable/typetable.h"
#include "../NoSqlDb/NoSqlDb.h"
#include "../FileMgr/FileMgr.h"
#include "../FileSystem/FileSystem.h"
#include "../HtmlParsing/htmlparsing.h"

using Helper = Utilities::StringHelper;
using namespace Utilities;
using File = std::string;
using Files = std::vector<File>;

//------------------------<Class that has functions for dependency analysis>----------------------------------------//
class dependencyAnalysis {
	CodeAnalysis::TypeAnal a;
	//dependencyAnalysis(); 
public: NoSQLDB::NoSqlDb<std::string> getAccess(CodeAnalysis::TypeAnal &ta, Files allfiles);
		NoSQLDB::NoSqlDb<std::string> depAnal(CodeAnalysis::TypeAnal &ta, Files allfiles);
		
};

//------------------------<Function that perfroms depencency Analysis>----------------------------------------//

NoSQLDB::NoSqlDb<std::string> dependencyAnalysis::depAnal(CodeAnalysis::TypeAnal &ta, Files allfiles) {

	CodeAnalysis::TypeAnal ta_ = ta;
	NoSQLDB::NoSqlDb<std::string> table = getAccess(ta_, allfiles);

	//CodeAnalysis::TypeAnal t;

	return table;
}
//------------------------<Function that gets access to typetable and performs dependency analysis>----------------------------------------//

NoSQLDB::NoSqlDb<std::string>  dependencyAnalysis::getAccess(CodeAnalysis::TypeAnal &ta, Files allfiles) {

	NoSQLDB::NoSqlDb<std::string> dependencyTable;
	NoSQLDB::Element<std::string> ele; 
	htmlHandle html; 
	for (File f : allfiles) {
		try{
			std::ifstream in(f);
			if (!in.good())
			{
				std::cout << "\n  can't open " << f << "\n\n";
			}
			{	int i = 0;
				Scanner::Toker toker;
				std::vector<std::string> depFileVect;
				toker.attach(&in);
				do
				{
					std::string tok = toker.getTok();
					std::vector<std::string> emptyVect;
					if (ta.db.find(tok) != ta.db.end()) {
						std::vector<std::unordered_map<std::string, std::string>> mapVect = ta.db[tok];
					for (auto tt : mapVect) {
						bool flag = true;
							for (auto x : depFileVect) {
								if (x == tt["file"])
									flag = false;
							}
							if(flag)
							depFileVect.push_back(tt["file"]);
						}}
				} while (in.good());
				ele.children = depFileVect;
				dependencyTable.saveRecord(f, ele);
				}}
		catch (std::logic_error& ex){
			std::cout << "\n  " << ex.what();}
		std::cout << "\n";
}	
	return dependencyTable;
}



