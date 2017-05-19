#pragma once
/////////////////////////////////////////////////////////////////////
// htmlparsing.h - tHIS HEADER FILE CREATES HTML FILLES            //
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
* Thisfile shows all the requiremnts met.
*
*
*
*
* -manual information
* -maintenance information
*
*
*
*
* Required Files :
*y-------------- -
*stringlyConnectedComponents.h
*AST
T

* Maintenance History :
*-------------------- -
*ver 1.0 : 04 - 5 - 2017
* HTMLPARSING.H
*
*
*
*
*
*
*
* -first release
*/
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <exception>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <fstream>
#include <cstdio>
#include <iostream>
#include "../FileSystem/FileSystem.h"
#include "../TypeTable/typetable.h"

class htmlHandle {
public :
	void htmlCreator(std::string Files,std::vector<std::string> depFiles, std::unordered_map<std::string, std::vector<CodeAnalysis::lineNum>> collapseDB, std::ofstream& js,int);
	void addPlusPlus(std::ifstream& inStream, std::ofstream& outStream, std::unordered_map<std::string, std::vector<CodeAnalysis::lineNum>> collapseDB, std::string filename, std::ofstream& js);
	
};


/*----------------- This cretes html filesand  associated style sheet ---------------------------------*/
void htmlHandle::htmlCreator(std::string allFiles, std::vector<std::string> depFiles, std::unordered_map<std::string,std::vector<CodeAnalysis::lineNum>> collapseDB, std::ofstream& js,int category) {
std::string files = allFiles;
std::string filename = allFiles;
		std::string just_name = FileSystem::Path::getName(filename);
		std::ifstream fin;
		std::ofstream fout;
		std::ofstream css;
		fin.open(filename); //input file (must in the same folder)
		if (fin.fail())
		{
			std::cout << "Input file opening failed.\n" <<filename<<std::endl;
			exit(1);}
std::string temphtmlfile ="..\\Server\\Repository\\PublishedFiles\\Category"+ std::to_string(category)+ "\\" + just_name + ".html";
		fout.open(temphtmlfile); //output file (in the same folder)
		if (fout.fail())
		{
			std::cout << "Output file opening failed.\n"<<temphtmlfile<<std::endl;
			exit(1);
		}

		std::string tempCSS = "..\\Server\\Repository\\PublishedFiles\\Category"+ std::to_string(category) +"\\stylesheet.css";
		css.open(tempCSS);
		if (css.fail())
		{
			std::cout << "Output file opening failed.\n";
			exit(1);
		}
		css <<  "/*----------------------------------------------------------------------------   - External CSS style sheet file for Project #3 Published 5 April 2017 Madhusudhan.B.R, CSE687 - Object Oriented Design, Spring 2017-----------------------------------------------------------------------------*/";
		css << "body  { margin:20px; color:black;background-color:#eee;font-family:Consolas; font-weight:600; font-size:110%;}";
		css.close();
		fout << "<!----------------------------------------------------------------------------"<<temphtmlfile<<" - output HTML file for Project #3 Published 5 April 2017 Madhusudhan.B.R, CSE687 - Object Oriented Design, Spring 2017----------------------------------------------------------------------------->";
		fout << " <html> <head>\n "<<"\n    <link rel=\""<<"stylesheet\""<<" type = \"text/css\""<<"  href=\""<<"stylesheet.css"<<"\"/>  \n"<<" <script type=\"text/javascript\" src=\"script.js\"></script>" <<"</head>" <<" <body> <h4>" <<just_name<<"<br>"<<" Dependent files : "<< std::endl;
		for (int i = 0; i < depFiles.size(); i++) {
			std::string anchortarget = FileSystem::Path::getName(depFiles[i]) + ".html";
			fout << "<h4> <a href = \"" << anchortarget
				<< "\">" << FileSystem::Path::getName(depFiles[i]) << " </a> </h4>" << std::endl;
		}
		fout << "<PRE>" << std::endl; 
		addPlusPlus(fin, fout,collapseDB,filename,js);
		fout << "</PRE> </body> </html>" << std::endl;
		fin.close();
		fout.close();
	
}
/*----------------- This checks the line numbers where '{' is encountered ---------------------------------*/

bool checkLineNum(std::unordered_map<std::string,std::vector<CodeAnalysis::lineNum>> collapseDB,std::string filename, int lineCount) {
		
	std::vector<CodeAnalysis::lineNum> tempClassVect;
	filename = FileSystem::Path::getName(filename);
	for (auto x : collapseDB)
	{
		//std::cout << x.first << std::endl;
		if(x.first == filename){
			for (int i = 0; i < x.second.size(); i++) {
				if (lineCount == x.second[i].startline)
					return true;
			}
		}
	}
	return false;
}

/*----------------- This checks the line numbers where '}' is encountered ---------------------------------*/

bool checkEndLineNum(std::unordered_map<std::string, std::vector<CodeAnalysis::lineNum>> collapseDB, std::string filename, int lineCount) {

	std::vector<CodeAnalysis::lineNum> tempClassVect;
	filename = FileSystem::Path::getName(filename);
	for (auto x : collapseDB)
	{
		//std::cout << x.first << std::endl;
		if (x.first == filename) {
			for (int i = 0; i < x.second.size(); i++) {
				if (lineCount == x.second[i].endline)
					return true;
			}
		}
	}
	return false;
	return false;
}

/*----------------------------------------- This functions reads line by line and and then checks characters for <,>,{,} -------------------------*/
void htmlHandle::addPlusPlus(std::ifstream& inStream, std::ofstream& outStream, std::unordered_map<std::string, std::vector<CodeAnalysis::lineNum>> collapseDB,std::string filename, std::ofstream& js)
{
	
	std::string line;
	int lineCount = 0;
	//std::ofstream fout;
	int i = 0; int a = 10000;
	while (std::getline(inStream, line)){	
		 lineCount++;
		outStream << "\n";
		//outStream << std::endl;
		for (int j = 0; j < line.size(); j++) {
		if (line[j] == '}' && checkEndLineNum(collapseDB, filename, lineCount)){
				outStream << "</div>";
				outStream << line[j];
			}
				if (line[j] == '{' && checkLineNum(collapseDB, filename, lineCount)) {
				outStream << line[j];
				outStream << "<button id =\"" << a <<"\"" <<"onclick=\"myFunction";
				outStream << i;
				outStream << "(";
				outStream << ");toggleText" << i << "(this." << "id" << ");" << "\">-</button>";
				//outStream << "<script>";
				
				js << "function myFunction" << i << "() {var x = document.getElementById('";
				js << i;
				js << "'); if (x.style.display ==  'none') {x.style.display = 'block';}else {x.style.display = 'none';}}" << "function toggleText"<<i<<"("<<"buttonid" <<") {var el = document.getElementById("<<"buttonid"<<");if (el.firstChild.data == \"-\"){el.firstChild.data = \"+\";}else{el.firstChild.data = \"-\";}}";
				//outStream << "</script>";
				js << "\n";
				outStream << "<div id=\"" << i  << "\">";
				i++; a++; 
			}

			else if (line[j] == '<')
				outStream << "&lt;";
			else if (line[j] == '>')
				outStream << "&gt;";
			else
				outStream << line[j];	
}}}



