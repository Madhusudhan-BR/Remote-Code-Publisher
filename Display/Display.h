#pragma once
/////////////////////////////////////////////////////////////////////
// Display.h     - display file  which is used to output stuff     //
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
* This package defines functions that are used to display TypeTable, Dependecy table and SCC
* 
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
#include <iostream>
#include "../NoSqlDb/NoSqlDb.h"

//------------------------<Displays dependency table>----------------------------------------//

void displayDependencyTable(NoSQLDB::NoSqlDb<std::string> table) {
	
	std::cout << "\n Display from the display package \n";
	std::vector<std::string> depKeys; 
	depKeys = table.keys(); 

	for (auto x : depKeys) {
		
		std::cout << " KEY : ";
		std::cout << x <<std::endl;

		std::vector<std::string> depValues = table.value(x).children; 

		std::cout << " Children Dependency Relationships : \n "; 

		for (auto y : depValues) {
			std::cout << y << "\n";
		}
		std::cout << "\n";
	}


}
//------------------------<displays typetable>----------------------------------------//

void displayTypeTable(std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>> db) {

	std::cout << "\n -------------------------------------------------------------------------------------- \n";
	std::cout << "\n ------------------ TYPE TABLE --------------------------------------------------------- \n";

	for (auto x : db)
	{
		std::cout << "\n -------------------------------------------------------------------------------------- \n";
		std::cout << x.first << std::endl;
		//std::cout << x.second << std::endl;
		for (int i = 0; i < x.second.size(); i++)
			for (auto y : x.second[i])
				std::cout << y.first << "     " << y.second << std::endl;
	}
}
//------------------------<displays scc>----------------------------------------//

void displaySCC(std::string s) {

	std::cout << s << "\t" << " "; 

}