/////////////////////////////////////////////////////////////////////////
// MsgServer.cpp - Demonstrates simple two-way HTTP messaging          //
//                                                                     //
// Modified by :Madhusudhan B R, OOD, Spring 2017 .						//
//	original : James Fawcett																	//		
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Macbook Pro  , Windows 10
//
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a client that sends HTTP style messages and
* files to a server that simply displays messages, upload, remove, publish files and stores files.
*
*/
/*
* Required Files:
*   MsgClient.cpp, MsgServer.cpp
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp

*   Utilities.h, Utilities.cpp
*/
/*
*
*/
#pragma warning(disable : 4267)
#include "../NewHeaderNewCpp/HttpMessage/HttpMessage.h"
#include "../NewHeaderNewCpp/Sockets/Sockets.h"
#include "../NewHeaderNewCpp/FileSystem/FileSystem.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "newServerheader.h" 
#include <string>
#include <iostream>
#include <thread>
#include "../Analyzer/Executive.h"
#include "../NoSqlDb/NoSqlDb.h"
using Show = Logging::StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;

/////////////////////////////////////////////////////////////////////
// ClientCounter creates a sequential number for each client
//
class ClientCounter
{
public:
	ClientCounter() { ++clientCount; }
	size_t count() { return clientCount; }
private:
	static size_t clientCount;
};

size_t ClientCounter::clientCount = 0;

/////////////////////////////////////////////////////////////////////
// MsgClient class
// - was created as a class so more than one instance could be 
//   run on child thread
//
class MsgClient
{
public:
	using EndPoint = std::string;
	void execute(const size_t TimeBetweenMessages, const size_t NumMessages,std::string bodyContent,std::string type);
	void listener();
	void remove(int category);
	void display(int category);
	void download(int);
private:
	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& fqname, Socket& socket,int category);
};
/*------------------This is used to display the files in a particular category-----------*/
void MsgClient::display(int category) {
	std::vector<std::string> htmlfiles = FileSystem::Directory::getFiles("../Server/Repository/PublishedFiles/Category" + std::to_string(category) +'/', "*.*");
	std::string allfiles; 
	for (std::string temp : htmlfiles) {
		allfiles = allfiles + temp + ";";
}	std::cout << allfiles;
	execute(100, 1, allfiles, "display");

}
/*------------------This is used to delete the files in a particular category-----------*/
void MsgClient::remove(int category) {
	std::vector<std::string> Sourcefiles = FileSystem::Directory::getFiles("../Server/Repository/SourceCodeFiles/Category" + std::to_string(category) +'/', "*.*");
	std::vector<std::string> htmlfiles = FileSystem::Directory::getFiles("../Server/Repository/PublishedFiles/Category" + std::to_string(category) + '/', "*.html");
	//Sourcefiles = Sourcefiles + htmlfiles; 
	/*for (std::string fname : htmlfiles) {
		Sourcefiles.push_back(fname);
	}*/
	for(std::string file : Sourcefiles){
		std::string temp = "../Server/Repository/SourceCodeFiles/Category" + std::to_string(category) + '/' + file;
		std::remove(temp.c_str());
	}
	

	for (std::string file : htmlfiles) {
		std::string temp = "../Server/Repository/PublishedFiles/Category" + std::to_string(category) + '/' + file;
		std::remove(temp.c_str());
	}
}
/*------------------This is used to download the files in a particular category from the client to the server -----------*/

void MsgClient::download(int category) {
	SocketSystem ss;
	SocketConnecter si;
	while (!si.connect("localhost", 8081))
	{
		Show::write("\n client waiting to connect");
		::Sleep(100);
	}
	std::string fqname = "../Server/Repository/PublishedFiles/Category" + std::to_string(category) + '/';
	std::vector<std::string> files = FileSystem::Directory::getFiles(fqname, "*.*");
	for (size_t i = 0; i < files.size(); ++i)
	{
		Show::write("\n\n  sending file " + files[i]);
		sendFile(files[i], si,category);
	}
	Show::write("\n\nFILES SENT.... Check\n");
	return;
}
//----< factory for creating messages >------------------------------
/*
* This function only creates one type of message for this demo.
* - To do that the first argument is 1, e.g., index for the type of message to create.
* - The body may be an empty string.
* - EndPoints are strings of the form ip:port, e.g., localhost:8081. This argument
*   expects the receiver EndPoint for the toAddr attribute.
*/
HttpMessage MsgClient::makeMessage(size_t n, const std::string& body, const EndPoint& ep)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:8080";  // ToDo: make this a member of the sender
											 // given to its constructor.
	switch (n)
	{
	case 1:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("POST", "Message"));
		msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
		msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));

		msg.addBody(body);
		if (body.size() > 0)
		{
			attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
			msg.addAttribute(attrib);
		}
		break;
	default:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));
	}
	return msg;
}
//----< send message using socket >----------------------------------

void MsgClient::sendMessage(HttpMessage& msg, Socket& socket)
{
	std::string msgString = msg.toString();
	socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}
//----< send file using socket >-------------------------------------
/*
* - Sends a message to tell receiver a file is coming.
* - Then sends a stream of bytes until the entire file
*   has been sent.
* - Sends in binary mode which works for either text or binary.
*/
bool MsgClient::sendFile(const std::string& filename, Socket& socket,int category)
{
	std::string fqname = "../Server/Repository/PublishedFiles/Category" + std::to_string(category) + '/' + filename;
	//std::string fqname = "../TestFiles/" + filename;
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;
	HttpMessage msg = makeMessage(1, "", "localhost::8080");
	msg.addAttribute(HttpMessage::Attribute("file", filename));
	msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	msg.addAttribute(HttpMessage::attribute("Category", std::to_string(category)));
	sendMessage(msg, socket);
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	while (true)
	{
		FileSystem::Block blk = file.getBlock(BlockSize);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); ++i)
			buffer[i] = blk[i];
		socket.send(blk.size(), buffer);
		if (!file.isGood())
			break;
	}
	file.close();
	return true;
}
//----< this defines the behavior of the client >--------------------

void MsgClient::execute(const size_t TimeBetweenMessages, const size_t NumMessages,std::string okbody,std::string type)
{
	ClientCounter counter;
	size_t myCount = counter.count();
	std::string myCountString = Utilities::Converter<size_t>::toString(myCount);
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8081))
		{
			//Show::write("\n client waiting to connect");
			::Sleep(100);
		}

		// send a set of messages

		if (okbody == "ok") {
			HttpMessage okMessage = makeMessage(1, "ok", "toAddr:localhost:8081");
			sendMessage(okMessage, si);
		}

		if (type == "display") {
			HttpMessage displayMessage = makeMessage(1,okbody, "toAddr:localhost:8081");
			displayMessage.addAttribute(HttpMessage::attribute("Type", "display"));
			sendMessage(displayMessage, si);
		}
		
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}
//----< entry point the server listens to responses from the client------
void MsgClient::listener()
{
	::SetConsoleTitle(L"HttpMessage Server - Runs Forever");
	Async::BlockingQueue<HttpMessage> msgQ;
	try{
		SocketSystem ss;
		SocketListener sl(8080, Socket::IP6);
		ClientHandler cp(msgQ);
		sl.start(cp);
		std::string category; 
		while (true){
			HttpMessage msg = msgQ.deQ();
			if (msg.bodyString() == "all done") {
				std::thread t1(
					[&]() { execute(100,1,"ok", "upload"); } // 20 messages 100 millisec apart
				);

				t1.join();}
				std::cout << msg.bodyString() << std::endl;

			if (msg.findValue("Type") == "delete") {
				category = msg.findValue("Category");
					int value = std::atoi(category.c_str());
				remove(value);
			}if (msg.findValue("Type") == "publish") {
				CodeAnalysis::CodeAnalysisExecutive exec;
				category = msg.findValue("Category");
				int value = std::atoi(category.c_str());
				exec.setup(value);
		}if (msg.findValue("Type") == "display") {
				category = msg.findValue("Category");
				int value = std::atoi(category.c_str());
								display(value);
}if (msg.findValue("Type") == "download") {
				category = msg.findValue("Category");
				int value = std::atoi(category.c_str());
				download(value);}
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);

	}


}



int main()
{


	CodeAnalysis::CodeAnalysisExecutive a; 

	MsgClient c1;
	std::thread t1(
		[&]() { c1.listener(); } // 20 messages 100 millisec apart
	);

	
	t1.join();

	Sleep(1000);

	//c1.execute(100, 1," ", " ");

	::SetConsoleTitle(L"HttpMessage Server - Runs Forever");

}