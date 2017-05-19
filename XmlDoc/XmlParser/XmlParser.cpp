///////////////////////////////////////////////////////////////////
// XmlParser.cpp - build XML parse tree                          //
// ver 1.3                                                       //
// Application: Support for XmlDocument, Summer 2015             //
// Platform:    Dell XPS 2720, Win 8.1 Pro, Visual Studio 2013   //
// Author:      Jim Fawcett, CST 4-187, 443-3948                 //
//              jfawcett@twcny.rr.com                            //
///////////////////////////////////////////////////////////////////

#include "XmlParser.h"
#include "../XmlElementParts/xmlElementParts.h"
#include "../XmlDocument/XmlDocument.h"
#include "../XmlElement/XmlElement.h"
#include "../XmlElementParts/Tokenizer.h"
#include <stack>
#include <string>
#include <cctype>
#include <locale>
#include <fstream>
#include <sstream>
#include "../Utilities/Utilities.h"

using namespace XmlProcessing;

//----< read text file contents into string >--------------------------------

std::string XmlParser::textFileToString(const std::string& fileName)
{
  std::ifstream in(fileName);
  if (!in.good())
    throw(std::exception(("can't open source file " + fileName).c_str()));
  std::ostringstream out;
  out << in.rdbuf();
  return std::move(out.str());
}
//----< remove non-essential whitespace from XML string >--------------------

void XmlParser::compress(std::string& srcStr)
{
  std::locale loc("");
  enum state_ { inTag, inText, outSide } state = outSide;
  std::string::iterator src, dst;
  src = dst = srcStr.begin();

  // state machine to process text string

  for (src = srcStr.begin(); src != srcStr.end(); ++src)
  {
    switch (state)
    {
    case outSide:
      if (!isspace(*src, loc))  // efficiently remove whitespace
      {
        *dst++ = *src;
        state = inTag;
      }
      break;
    case inTag:
      *dst++ = *src;
      if ((src + 1) != srcStr.end() && *src == '>' && *(src + 1) != '<')
      {
        state = inText;
      }
      break;
    case inText:
      if (*src == '<')
      {
        *dst++ = *src;
        state = inTag;
      }
      else if (!(*(dst - 1) == '>' && isspace(*src, loc)))
      {
        *dst++ = *src;
      }
      break;
    }
  }
  srcStr.erase(dst, srcStr.end());
}
//----< wrap text elements in quotes so they become one token >--------------

std::string XmlParser::enquoteText(const std::string& src)
{
  std::string temp;
  temp.push_back(src[0]);
  for (size_t i = 1; i < src.size(); ++i)
  {
    temp.push_back(src[i]);
    if (src[i] == '>' && src[i + 1] != '<')
      if (i + 1 < src.size())  // don't quote end
        temp.push_back('\"');
    if (src[i + 1] == '<' && src[i] != '>')
      temp.push_back('\"');
  }
  return temp;
}
//----< remove quotes, if any, from text string >----------------------------

std::string XmlParser::dequoteText(const std::string& src)
{
  std::string temp;
  for (size_t i = 0; i < src.size(); ++i)
    if (src[i] != '\"' && src[i] != '\'')
      temp.push_back(src[i]);
  return temp;
}
//----< initialize XmlParser object with string ready for parsing >----------

XmlParser::XmlParser(const std::string& src, sourceType type) : src_(src)
{
  if (type == file)
    src_ = textFileToString(src_);
  compress(src_);
  src_ = enquoteText(src_);
  pToker_ = new Toker(src_, false);
  pToker_->setMode(Toker::xml);
  pXmlParts_ = new XmlParts(pToker_);
}
//----< extract attributes, if any, from XmlParts token collection >---------

void XmlParser::extractAttributes()
{
  attribs_.clear();
  XmlParts xmlParts = *pXmlParts_;
  for (size_t pos = 0; pos < (size_t)xmlParts.length(); ++pos)
  {
    if (xmlParts[(int)pos] == "=")
    {
      auto item = attrib(xmlParts[(int)pos - 1], xmlParts[(int)pos + 1]);
      attribs_.push_back(item);
    }
  }
}
//----< display all attribute name-value pairs in current XmlParts >---------

void XmlParser::showAttributes()
{
  if (attribs_.size() == 0)
    return;
  for (size_t i = 0; i < attribs_.size(); ++i)
  {
    std::cout << "\n      " << attribs_[i].first << ", " << attribs_[i].second;
  }
}
//----< factory for XmlDeclaration node >------------------------------------

XmlParser::sPtr XmlParser::createXmlDeclar()
{
  extractAttributes();
  sPtr pDeclar = makeXmlDeclarElement();
  for (auto item : attribs_)
  {
    std::string name = item.first;
    std::string value = dequoteText(item.second);
    pDeclar->addAttrib(name, value);
  }
  if (verbose_)
  {
    std::cout << "\n  " << pXmlParts_->show();
    std::cout << "\n    xml declaration";
    showAttributes();
  }
  return pDeclar;
}
//----< factory for Processing Instruction node >----------------------------

XmlParser::sPtr XmlParser::createProcInstr()
{
  extractAttributes();
  sPtr pProcInstr = makeProcInstrElement("");
  for (auto item : attribs_)
  {
    std::string name = item.first;
    std::string value = dequoteText(item.second);
    pProcInstr->addAttrib(name, value);
  }
  if (verbose_)
  {
    std::cout << "\n  " << pXmlParts_->show();
    std::cout << "\n    processing instruction";
    showAttributes();
  }
  return pProcInstr;
}
//----< factory for Comment node >-------------------------------------------

XmlParser::sPtr XmlParser::createComment()
{
  std::string comment;
  XmlParts& xmlParts = *pXmlParts_;
  for (size_t i = 3; i < (size_t)xmlParts.length() - 2; ++i)
  {
    comment += xmlParts[(int)i];
    if (i < (size_t)xmlParts.length() - 3)
      comment += " ";
  }
  if (verbose_)
  {
    std::cout << "\n  " << pXmlParts_->show();
    std::cout << "\n    comment";
    std::cout << "\n      " << comment;
  }
  sPtr pComment = makeCommentElement(comment);
  return pComment;
}
//----< factory for Tagged Element node >------------------------------------

XmlParser::sPtr XmlParser::createTaggedElem()
{
  XmlParts& xmlParts = *pXmlParts_;
  sPtr pTaggedElem = makeTaggedElement(xmlParts[1]);
  extractAttributes();
  for (auto item : attribs_)
  {
    std::string name = item.first;
    std::string value = dequoteText(item.second);
    pTaggedElem->addAttrib(name, value);
  }
  if (verbose_)
  {
    std::cout << "\n  " << pXmlParts_->show();
    std::cout << "\n    tagged element " << "\"" << xmlParts[1] << "\"";
    showAttributes();
  }
  return pTaggedElem;
}
//----< factory for Text Element node >--------------------------------------

XmlParser::sPtr XmlParser::createTextElem()
{
  XmlParts& xmlParts = *pXmlParts_;
  std::string text = dequoteText(xmlParts[0]);
  sPtr pTextElem = makeTextElement(text);
  if (verbose_)
  {
    std::cout << "\n  " << pXmlParts_->show();
    std::cout << "\n    Text Element";
  }
  return pTextElem;
}
//----< show end element parts >---------------------------------------------

void XmlParser::processEndElem()
{
  if (verbose_)
  {
    std::cout << "\n  " << pXmlParts_->show();
    std::cout << "\n    end element";
  }
}
//----< parse markup and add to XML parse tree >-----------------------------

void XmlParser::processMarkup(ElemStack& elemStack)
{
  XmlParts& xmlParts = *pXmlParts_;
  if (xmlParts[1] == "?")
  {
    if (xmlParts[2] == "xml")
    {
      sPtr pDeclar = createXmlDeclar();
      elemStack.top()->addChild(pDeclar);
    }
    else
    {
      sPtr pProcInstr = createProcInstr();
      elemStack.top()->addChild(pProcInstr);
    }
    if (verbose_) std::cout << "\n";
    return;
  }
  if (xmlParts[1] == "!")
  {
    sPtr pComment = createComment();
    elemStack.top()->addChild(pComment);
    if (verbose_) std::cout << "\n";
    return;
  }
  if (xmlParts[1] == "/")
  {
    processEndElem();
    elemStack.pop();
  }
  else
  {
    sPtr pTaggedElem = createTaggedElem();
    elemStack.top()->addChild(pTaggedElem);
    elemStack.push(pTaggedElem);
  }
  if (verbose_) std::cout << "\n";
  return;
}
//----< add text to XML parse tree >-----------------------------------------

void XmlParser::processText(ElemStack& elemStack)
{
  sPtr pTextElem = createTextElem();
  elemStack.top()->addChild(pTextElem);
  if (verbose_) std::cout << "\n";
}
//----< build XmlDocument from XmlParts >------------------------------------

XmlDocument* XmlParser::buildDocument()
{
  XmlDocument* pDoc = new XmlDocument(makeDocElement());
  using sPtr = std::shared_ptr < AbstractXmlElement >;
  sPtr pDocElem = pDoc->docElement();

  std::stack <sPtr> elemStack_;
  elemStack_.push(pDocElem);

  std::locale loc;
  XmlParts& xmlParts = *pXmlParts_;
  while (xmlParts.get())
  {
    if (xmlParts[0] == "<")
    {
      processMarkup(elemStack_);
      continue;
    }
    if (xmlParts[0][0] == '\"' || isalnum(xmlParts[0][0]) || isspace(xmlParts[0][0], loc))
    {
      processText(elemStack_);
      continue;
    }
    throw(std::exception("ill-formed XML"));
  }
  if(verbose_) std::cout << "\n";
  return pDoc;
}

#ifdef TEST_XMLPARSER

using namespace::Utilities;
using Utils = StringHelper;

int main()
{
  Utils::Title("Testing XmlParser");
  putline();
  std::string src = "../XmlElementParts/LectureNote.xml";
  XmlParser parser(src);
  parser.verbose();
  Utils::title("Compressed form of formatted XML:");
  std::cout << "\n" << src << "\n";
  Utils::title("Parsing compressed XML:");
  XmlDocument* pDoc = parser.buildDocument();
  Utils::title("Resulting XML Parse Tree:");
  std::cout << "\n" << pDoc->toString();
  std::cout << "\n\n";
}

#endif

