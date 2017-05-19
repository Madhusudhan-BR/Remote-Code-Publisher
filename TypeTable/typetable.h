
#pragma once
/////////////////////////////////////////////////////////////////////
// typetable.h   - Test EXecutive showing requirements met.        //
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
* This package defines one class that has doTypeAnalysis function
* that does dependency analysis and stores it in the DB. I am populating the typetable in the DFS function
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
*tokeniser.h,semiExp.h,abstractSyntaxTree.h,parser.h
*
* Maintenance History:
*---------------------
* ver 1.0 : 03-10-2017
** var 2.0  05-04-2017
* Functionality extended for project 3
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
#include "../NoSqlDb/NoSqlDb.h"
#include "../Display/Display.h"
#include "../Parser/ActionsAndRules.h"
#include "../CppProperties/CppProperties.h"
#include <iostream>
#include <functional>
#include <stack>
#pragma warning (disable : 4101) 
// disable warning re unused variable x, below


using typetable = std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>>;
namespace CodeAnalysis
{
	class lineNum {
	public:
		int startline=0;
		int endline=0;
};

	class TypeAnal
	{
	public:
		using SPtr = std::shared_ptr<ASTNode*>;
		typetable db;
		std::unordered_map<std::string, std::vector<lineNum>> collapseDB;
		std::vector<int> tempLineCountVec;
		void collapse(ASTNode* pNode);
		std::stack<std::string> namespaceStack;
		TypeAnal();
		void doTypeAnal();
		//NoSqlDb < std::string> db;
		//int i = 0;
		//std::vector<std::string> getKeys(typetable &db);
	private:
		void DFS(ASTNode* pNode);
		AbstrSynTree& ASTref_;
		ScopeStack<ASTNode*> scopeStack_;
		Scanner::Toker& toker_;
	};


	inline TypeAnal::TypeAnal() :
		ASTref_(Repository::getInstance()->AST()),
		scopeStack_(Repository::getInstance()->scopeStack()),
		toker_(*(Repository::getInstance()->Toker()))
	{
		std::function<void()> test = [] { int x; };  // This is here to test detection of lambdas.
	}                                              // It doesn't do anything useful for dep anal.

//<---------------------------------- Displays class struct enum alias and typedef------------------->
	inline bool doDisplay(ASTNode* pNode)
	{
		static std::string toDisplay[] = {
			"class", "struct", "enum", "alias", "typedef"
		};
		for (std::string type : toDisplay)
		{
			if (pNode->type_ == type)
				return true;
		}
		return false;
	}
	//<---------------------------------- Traverses the dfs tree and populates the typetable------------------->
	inline void TypeAnal::DFS(ASTNode* pNode)
	{
		int i = 0;
		i++;
		static std::string path = "";
		if (pNode->path_ != path)
		{
			std::cout << "\n    -- " << "  This is the path  " << pNode->path_ << "\\ This is the package " << pNode->package_;
			path = pNode->path_;
		}
collapse(pNode);
if (pNode->type_ == "namespace") {
		namespaceStack.push(pNode->name_);
}
		if (doDisplay(pNode))
		{
			std::cout << "\n  " << " this is the name " << pNode->name_;
			std::cout << ", " << "this is the type   " << pNode->type_;
			std::unordered_map<std::string, std::string> keyValue;
			keyValue["namespace"] = namespaceStack.top();
			keyValue["type"] = pNode->type_;
			keyValue["file"] = pNode->path_;
			std::vector<std::unordered_map<std::string, std::string>> element = db[pNode->name_];
			element.push_back(keyValue);
			db[pNode->name_] = element;
		}
		if ((pNode->type_ == "namespace") && (pNode->name_ == "Global Namespace")) {
			for (auto pchild : pNode->children_) {
				if (pchild->type_ == "function" && (pchild->name_ != "main")) {
					std::unordered_map<std::string, std::string> keyValue;
					keyValue["namespace"] = pNode->name_;
					keyValue["type"] = pchild->type_;
					keyValue["file"] = pchild->path_;
					std::vector<std::unordered_map<std::string, std::string>> element = db[pchild->name_];
					element.push_back(keyValue);
					db[pchild->name_] = element;
				}}}
		
			for (auto pChild : pNode->children_)
		{
			if (pChild->parentType_ == "namespace") {
				namespaceStack.push(pNode->name_);
			}
			DFS(pChild);
			if (pChild->parentType_ == "namespace") {
				namespaceStack.pop();
}}}


	//<---------------------------------- gets AST root and calls DFS------------------->
	inline void TypeAnal::doTypeAnal()
	{


		std::cout << "\n  starting type analysis:\n";
		std::cout << "\n  scanning AST and displaying important things:";
		std::cout << "\n -----------------------------------------------";
		ASTNode* pRoot = ASTref_.root();
		DFS(pRoot);
		//std::cout << "\n\n  the rest is for you to complete\n";

		displayTypeTable(db);
		
	}

/*-----------------------This function captures the start and end line numbers whereever we find a class or a function*/
	inline void TypeAnal::collapse(ASTNode* pNode) {
		
		if (pNode->type_ == "function" || pNode->type_ == "class") {
			if (pNode->startLineCount_ == pNode->endLineCount_ || pNode->endLineCount_ == pNode->startLineCount_ + 1) {
				;
			}
			else {
				lineNum tempLine;
				std::vector<lineNum> lineVect;
				tempLine.startline = (int)pNode->startLineCount_;
				tempLine.endline = (int)pNode->endLineCount_;
				lineVect = collapseDB[pNode->package_];
				lineVect.push_back(tempLine);
				collapseDB[pNode->package_] = lineVect;
			}
			

		}
		return;

	}
}
