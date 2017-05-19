#pragma once
/////////////////////////////////////////////////////////////////////
// NoSqlDb.h - key/value pair in-memory database                   //
// ver 1.2                                                         //
// Madhusudhan, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package defines two classes that support key/value storage:
* - Element<Data> defines a single value stored in the database.
*   It provides metadata properties:
*   - nam
* - manual information
* - TestExecutive that demonstrates the requirements you meet
*
* Required Files:
* ---------------
*   NoSqlDb.h, NoSqlDb.cpp, CppProperties.h, CppProperties.cpp
*
* Maintenance History:
*---------------------
* ver 1.2 : 06 Feb 2017
* - added children
* - modified Element<Data>::show()
* - added these comments
* ver 1.1 : 28 Jan 2017
* - fixed bug in NoSqlDb::count() by replacing
*   unordered_map<key,Value>::count() with
*   unordered_map<key,Value>::size();
* Ver 1.0 : 25 Jan 2017
* - first release
*/

#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include "../CppProperties/CppProperties.h"
#include "../DateTime/DateTime.h"
#include "../XmlDoc/XmlDocument/XmlDocument.h"
#include "../XmlDoc/XmlElement/XmlElement.h"


namespace NoSQLDB
{
	using Key = std::string;
	//using Keys = std::vector<Key>;
	//using Name = std::string;
	//using Category = std::string;
	using Children = std::vector<std::string>;

	/////////////////////////////////////////////////////////////////////
	// Element class represents a data record in our NoSql database
	// - in our NoSql database that is just the value in a key/value pair
	// - it needs to store child data, something for you to implement
	//
	template<typename Data>
	class Element
	{
	public:

		Element() {}

		Property<Children> children;          // metadata

		void saveChild(Key);
		std::string show();
	};

	template<typename Data>
	void Element<Data>::saveChild(Key key)
	{
		children.getValue().push_back(key);
	}


	template<typename Data>
	std::string Element<Data>::show()
	{
		std::ostringstream out;
		out.setf(std::ios::adjustfield, std::ios::left);

		Children children_ = static_cast<Children>(children);
		if (children_.size() > 0)
		{
			out << "\n    " << std::setw(8) << "children" << " : ";
			for (size_t i = 0; i < children_.size(); ++i)
			{
				cout << children_[i];
				if (i < children_.size())
					out << ", ";
			}
		}
		//out << "\n    " << std::setw(8) << "data" << " : " << data;
		out << "\n";
		return out.str();
	}

	/////////////////////////////////////////////////////////////////////
	// NoSqlDb class is a key/value pair in-memory database
	// - stores and retrieves elements
	// - you will need to add query capability
	//   That should probably be another class that uses NoSqlDb
	//

	using namespace XmlProcessing;
	template<typename Data>
	class NoSqlDb
	{
	public:
		using Key = std::string;
		using Keys = std::vector<Key>;
		using sPtr = std::shared_ptr<AbstractXmlElement>;
		using xmlString = std::string;
		using xml = std::string;
		using allTheKeys = std::vector<std::string>;

		//std::string path;
		Keys keys();
		std::string toXML(std::string depxml);
		std::string toXMLSCC(std::string sccxml);
		bool hasKey(Key key);
		bool saveRecord(Key key, Element<Data> elem);
		bool saveValue(Key key, Element<Data> elem);
		bool removeRecord(Key key);
		Element<Data> value(Key key);
		size_t count();



	private:
		using Item = std::pair<Key, Element<Data>>;

		std::unordered_map<Key, Element<Data>> store;
	};

	//------------------------<FUnction to output DB in XML file>----------------------------------------//

	template<typename Data>
	std::string NoSqlDb<Data>::toXML(std::string depxml) {

	std::string document;
	XmlDocument doc;
	allTheKeys allKeys = keys();

	sPtr pRoot = makeTaggedElement("Database");
	doc.docElement() = pRoot;

	for (xml element : allKeys) {
	Element<Data> ele = value(element);
	sPtr theRecord = makeTaggedElement("Record");
	pRoot->addChild(theRecord);

	sPtr keyElement = makeTaggedElement("Key");
	sPtr keyText = makeTextElement(element);
	theRecord->addChild(keyElement);
	keyElement->addChild(keyText);

	std::vector<std::string> dependentKeys = ele.children;

	int counter = (int)dependentKeys.size(), i = 0;

	sPtr RecordChild = makeTaggedElement("Children");
	theRecord->addChild(RecordChild);

	for (int i = 0; i < counter; i++) {
	xml child = dependentKeys[i];
	sPtr pchildelement = makeTaggedElement("child");
	RecordChild->addChild(pchildelement);
	sPtr pchildText = makeTextElement(child);
	pchildelement->addChild(pchildText);
	}
	}
	document = doc.toString();

	std::ofstream inputfile(depxml);
	inputfile << document;
	inputfile.close();

	return document;
	}
	
	//------------------------<Function that outputs SCC in the from of a XML file>----------------------------------------//


	template<typename Data>
	std::string NoSqlDb<Data>::toXMLSCC(std::string sccxml) {

		std::string document;
		XmlDocument doc;
		allTheKeys allKeys = keys();

		sPtr pRoot = makeTaggedElement("Database");
		doc.docElement() = pRoot;

		for (xml element : allKeys) {
			Element<Data> ele = value(element);
			sPtr theRecord = makeTaggedElement("Record");
			pRoot->addChild(theRecord);

			/*sPtr keyElement = makeTaggedElement("Key");
			sPtr keyText = makeTextElement(element);
			theRecord->addChild(keyElement);
			keyElement->addChild(keyText);*/
		

			std::vector<std::string> dependentKeys = ele.children;

			int counter = (int)dependentKeys.size(), i = 0;

			sPtr RecordChild = makeTaggedElement("Children");
			theRecord->addChild(RecordChild);

			for (int i = 0; i < counter; i++) {
				xml child = dependentKeys[i];
				sPtr pchildelement = makeTaggedElement("child");
				RecordChild->addChild(pchildelement);
				sPtr pchildText = makeTextElement(child);
				pchildelement->addChild(pchildText);
			}
		}
		document = doc.toString();

		std::ofstream inputfile(sccxml);
		inputfile << document;
		inputfile.close();

		return document;
	}
	//------------------------<returns keys>----------------------------------------//

	template<typename Data>
	typename NoSqlDb<Data>::Keys NoSqlDb<Data>::keys()
	{
		Keys keys;
		for (Item item : store)
		{
			keys.push_back(item.first);
		}
		return keys;
	}
	//------------------------<Chwcks for a key>----------------------------------------//

	template<typename Data>
	bool NoSqlDb<Data>::hasKey(Key key)
	{
		if (store.find(key) == store.end())
			return false;
		return true;
	}
	//------------------------<Saves record into DB>----------------------------------------//

	template<typename Data>
	bool NoSqlDb<Data>::saveRecord(Key key, Element<Data> elem)
	{
		if (store.find(key) != store.end())
			return false;
		store[key] = elem;
		return true;
	}
	//------------------------<Removes record from DB>----------------------------------------//

	template<typename Data>
	bool NoSqlDb<Data>::removeRecord(Key key)
	{
		if (store.find(key) == store.end())
			return false;
		size_t numErased = store.erase(key);
		if (numErased == 1)
		{
			// remove key from all other element's children collections
			for (Key k : keys())
			{
				Keys& children = store[k].children.getValue();  // note Keys& - we don't want copy of children
				Keys::iterator iter;
				for (iter = children.begin(); iter != children.end(); ++iter)
				{
					if ((*iter) == key)
					{
						children.erase(iter);
						break;
					}
				}
			}
			return true;
		}
		return false;
	}
	//------------------------<Saves record into DB>----------------------------------------//

	template<typename Data>
	bool NoSqlDb<Data>::saveValue(Key key, Element<Data> elem)
	{
		if (store.find(key) == store.end())
			return false;
		store[key] = elem;
		return true;
	}

	//------------------------<Retreives value of a particular key>----------------------------------------//

	template<typename Data>
	Element<Data> NoSqlDb<Data>::value(Key key)
	{
		if (store.find(key) != store.end())
			return store[key];
		return Element<Data>();
	}
	//------------------------<returns no of records in the DB>----------------------------------------//

	template<typename Data>
	size_t NoSqlDb<Data>::count()
	{
		return store.size();
	}



}

