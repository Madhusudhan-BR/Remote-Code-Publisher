#ifndef XMLPARSER_H
#define XMLPARSER_H
///////////////////////////////////////////////////////////////////
// XmlParser.h - build XML parse tree                            //
// ver 1.3                                                       //
// Application: Support for XmlDocument, Summer 2015             //
// Platform:    Dell XPS 2720, Win 8.1 Pro, Visual Studio 2013   //
// Author:      Jim Fawcett, CST 4-187, 443-3948                 //
//              jfawcett@twcny.rr.com                            //
///////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* The XML Document Model is essentially a program-friendly wrapper
* around an Abstract Syntax Tree (AST) used to contain the results 
* of parsing XML markup.
*
* This package provides parsing of an XML file or string and builds
* it's AST.  See the prologue of the XmlDocument package for more
* details.
*
* XmlParser objects throw if given an invalid path to an XML file.
*
* Required Files:
* ---------------
*   - XmlParser.h, XmlParser.cpp, 
*   - XmlElementParts.h, XmlElementParts.cpp
*   - XmlDocument.h, XmlDocument.cpp, XmlElement.h, XmlElement.cpp
*   - ITokenCollection.h, Tokenizer.h, Tokenizer.cpp
*   - Utilities.Lib

* Build Process:
* --------------
*   devenv XmlParser.sln /debug rebuild
*
* Maintenance History:
* --------------------
* Ver 1.3 : 01 Jun 15
* - made constr src string const
* - added src_ member string
* Ver 1.2 : 31 May 15
* - factored parsing code to reduce size and complexity
* - renamed a couple of methods to improve readability
* Ver 1.1 : 30 May 15
* - reconfigured code to use and support repository storage
*   - using Repository::Utilities
*   - added Repository\include to Environment's Path to support
*     #include <utilities>
* Ver 1.0 : 28 May 15
* - first release
*
* ToDo:
* -----
* - polish into production code
*   - rigourous testing
*/

#include "../XmlElementParts/itokcollection.h"
#include "../XmlElementParts/xmlElementParts.h"
#include "../XmlElement/XmlElement.h"
#include <vector>
#include <stack>
#include <memory>

namespace XmlProcessing
{
  class XmlDocument;

  class XmlParser
  {
  public:
    using sPtr = std::shared_ptr < AbstractXmlElement > ;
    using attrib = std::pair < std::string, std::string > ;
    using attribs = std::vector <attrib> ;
    using ElemStack = std::stack < sPtr > ;

    enum sourceType { file, str };
    XmlParser(const std::string& src, sourceType type = file);
    bool good();
    XmlDocument* buildDocument();
    bool verbose(bool verb = true);
  private:
    std::string textFileToString(const std::string& fileSpec);
    void compress(std::string& xmlStr);
    std::string enquoteText(const std::string& src);
    std::string dequoteText(const std::string& src);
    sPtr createXmlDeclar();
    sPtr createProcInstr();
    sPtr createComment();
    sPtr createTaggedElem();
    sPtr createTextElem();
    void processMarkup(ElemStack& stack);
    void processText(ElemStack& stack);
    void processEndElem();
    void extractAttributes();
    void showAttributes();
    attribs& attributes();
    attribs attribs_;
    ITokCollection* pTokColl_;
    XmlParts* pXmlParts_;
    Toker* pToker_ = nullptr;
    std::string src_;
    bool verbose_ = false;
    bool good_ = false;
  };

  inline bool XmlParser::good() { return good_; }
  inline XmlParser::attribs& XmlParser::attributes() { return attribs_; }
  inline bool XmlParser::verbose(bool verb) 
  { 
    bool temp = verbose_;  
    verbose_ = verb; 
    return temp; 
  }
}
#endif
