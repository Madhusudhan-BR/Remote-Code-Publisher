#pragma warning(disable : 4503)
#pragma warning(disable : 4221)

/////////////////////////////////////////////////////////////////////
// Executive.cpp - Test EXecutive showing requirements met.        //
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
* - manual information
* - maintenance information
*
*
*
*
* Required Files:
*display ---------------
*stringlyConnectedComponents.h
*AST
Typetable.h

* Maintenance History:
*---------------------
* ver 1.0 : 03-10-2017
* excutive.cpp
* var 2.0  05-04-2017
* Functionality extended for project 3
*
*
*
*
*
* - first release
*/

#include "../Analyzer\Executive.h"
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
#include <Windows.h>
#include "testexecutive.h"
#include "../Display/Display.h"
#include "../StrongConnnectedComponent/stronglyConnectedComp.h"
#include "../DependencyTable/dependencyAnalysis.h"
#include "../TypeTable/typetable.h"
#include "../Parser/Parser.h"
#include "../FileSystem/FileSystem.h"
#include "../FileMgr/FileMgr.h"
#include "../Parser/ActionsAndRules.h"
#include "../Parser/ConfigureParser.h"
#include "../AbstractSyntaxTree/AbstrSynTree.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <fstream>
#include "../HtmlParsing/htmlparsing.h"

//void printTestExDepTable(){
//	
//
//	std::cout << "\n -------------------------------------------------------------------------------------- \n";
//	std::cout << "\n DependencyTable \n";
//	std::cout << "\n -------------------------------------------------------------------------------------- \n";
//}
//
//void execSetup(CodeAnalysisExecutive &exec) {
//	exec.getSourceFiles();
//	exec.processSourceCode(true);
//	exec.complexityAnalysis();
//	exec.dispatchOptionalDisplays();
//	exec.flushLogger();
//	exec.displayMetricSummary(50, 10);
//	exec.flushLogger();
//	Rslt::write("\n");
//	std::ostringstream out;
//	out << "\n  " << std::setw(10) << "searched" << std::setw(6) << exec.numDirs() << " dirs";
//	out << "\n  " << std::setw(10) << "processed" << std::setw(6) << exec.numFiles() << " files";
//	Rslt::write(out.str());
//	Rslt::write("\n");
//	exec.stopLogger();
//	std::cout << "\n  Code Analysis completed";
//
//}
//
//void execSetup2(CodeAnalysisExecutive &exec) {
//	exec.setDisplayModes();
//	exec.startLogger(std::cout);
//	std::ostringstream tOut("CodeAnalysis - Version 1.4");
//	Utils::sTitle(tOut.str(), 3, 92, tOut, '=');
//	Rslt::write(tOut.str());
//	Rslt::write("\n     " + exec.systemTime());
//	Rslt::flush();
//	
//}
//void requirements() {
//	std::cout << "\n -------------------------------------------------------------------------------------- \n";
//	std::cout << "\n Test Executive \n";
//	std::cout << "\n -------------------------------------------------------------------------------------- \n";
//	std::cout << "\n -------------------------------------------------------------------------------------- \n";
//	std::cout << "\n Requirements 1 - 10 including collapse Satisfied \n";
//	std::cout << "\n -------------------------------------------------------------------------------------- \n";
//
//}
//void	indexCreator(std::vector<std::string> keys) {
//
//	std::ofstream fout;
//	//FileMap fileMap = exec.getFileMap();
//	std::string temphtmlfile = "../PublishedFiles/index.html";
//	fout.open(temphtmlfile);
//	if (fout.fail())
//	{
//		std::cout << "Output index file opening failed.\n";
//		exit(1);
//	}
//	fout << "<!----------------------------------------------------------------------------" << temphtmlfile << " - output index.html HTML file for Project #3 Published 5 April 2017 Madhusudhan.B.R, CSE687 - Object Oriented Design, Spring 2017----------------------------------------------------------------------------->";
//	fout << "<html> \n <head> \n" << "\n    <link rel=\"" << "stylesheet\"" << " type = \"text/css\"" << "  href=\"" << "stylesheet.css" << "\"/> </head> <body> \n";
//	using Pair = std::pair<Pattern, Files>;
//	for (std::string key : keys) {
//		//for (File f : pair.second)
//		{
//			string temp = FileSystem::Path::getName(key) + ".html";
//			fout << "<a href =\"";
//			fout << temp;
//			fout << "\">";
//			fout << temp;
//			fout << " </a> <br> ";
//		}
//	}
//	fout.close();
//
//	std::string path = "file:///" + FileSystem::Path::getFullFileSpec("..\\PublishedFiles\\index.html");
//	std::wstring spath = std::wstring(path.begin(), path.end());
//	LPCWSTR swpath = spath.c_str();
//	LPCWSTR a = L"open";
//	LPCWSTR ie = L"iexplore.exe";
//	ShellExecute(NULL, a, ie, swpath, NULL, SW_SHOWDEFAULT);
//
//}


#ifdef TESTEXEC
int main(int argc, char* argv[]) {
	using namespace CodeAnalysis;
	CodeAnalysisExecutive exec;
	try {
		requirements();
		exec.defaultDependency = "../depXml.xml"; exec.defaultScc = "../SccXml.xml";
		bool succeeded = exec.ProcessCommandLine(argc, argv);
		if (!succeeded)
		{
			return 1;
		}
		execSetup2(exec);
		exec.showCommandLineArguments(argc, argv);
		Rslt::write("\n");
		execSetup(exec);
		FileMap fileMap = exec.getFileMap();
		Files allFiles;
		using Pair = std::pair<Pattern, Files>;
		for (Pair pair : fileMap) {
			for (File f : pair.second)
				allFiles.push_back(f);
		}
		TypeAnal ta;
		ta.doTypeAnal();
		dependencyAnalysis da;
		NoSQLDB::NoSqlDb<std::string> table;
		table = da.depAnal(ta, allFiles);
		printTestExDepTable();
		displayDependencyTable(table);
		int i = 0;
		std::ofstream js;
		std::string tempjsfile = "../PublishedFiles/script.js";
		js.open(tempjsfile);
		if (js.fail()) {
			std::cout << "Output file opening failed.\n"; exit(1);
		}
		js << "/*----------------------------------------------------------------------------   - External Javacript  file for Project #3 Published 5 April 2017 Madhusudhan.B.R, CSE687 - Object Oriented Design, Spring 2017-----------------------------------------------------------------------------*/";
		htmlHandle html;
		std::vector<std::string> tableKeys = table.keys();
		for (string tableKey : tableKeys) {
			std::vector<std::string> tableChildren = table.value(tableKey).children;
			std::vector<std::string> temp;
			auto last = std::unique(tableChildren.begin(), tableChildren.end());
			tableChildren.erase(last, tableChildren.end());
			html.htmlCreator(tableKey, tableChildren, ta.collapseDB, js);
		}
		indexCreator(tableKeys);
		std::string xml = table.toXML(exec.defaultDependency);
		std::cout << xml;
	}
	catch (std::exception& except) {
		exec.flushLogger();
		std::cout << "\n\n  caught exception in Executive::main: " + std::string(except.what()) + "\n\n";
		exec.stopLogger();
		return 1;
	}
	return 0; }
#endif // TEST





