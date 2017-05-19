/////////////////////////////////////////////////////////////////////////
// MsgClient.cpp - Demonstrates simple one-way HTTP messaging          //
//                                                                     //
// Modified by :Madhusudhan B R, OOD, Spring 2017 .						//
//	original : James Fawcett																	//		
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Macbook Pro  , Windows 10       //
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a client that sends HTTP style messages and
* files to a server that simply displays messages,publishes dependency files, removes files from the client and stores files.
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
======================================================================
Maintanance History:
version 1.0 ::
Implements   simple two-way HTTP style messaging
and file transfer
*/
#pragma warning(disable : 4267)
#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../../Logger/Logger.h"
#include "../../Utilities/Utilities.h"
#include "../../Client/server.h"
#include <string>
#include <iostream>
#include <thread>

using Show = Logging::StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;


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
	void execute(const size_t TimeBetweenMessages, const size_t NumMessages);
	void listener();
	void uploadFiles(int);
	void remove(int);
	void publish(int);
	void download(int);
	void display(int); 
	void autoOpen(int);
	//Async::BlockingQueue<HttpMessage> msgQ;
private:
	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep, std::string type, int catrgory);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& fqname, Socket& socket,int);
};
//----< auto open browser from client >------------------------------

void MsgClient::autoOpen(int category) {
	std::string path = "file:///" + FileSystem::Path::getFullFileSpec("..\\Client\\Repository\\PublishedFiles\\Category" + std::to_string(category) + "\\index.html");
	std::wstring spath = std::wstring(path.begin(), path.end());
	LPCWSTR swpath = spath.c_str();
	LPCWSTR a = L"open";
	LPCWSTR ie = L"iexplore.exe";
	ShellExecute(NULL, a, ie, swpath, NULL, SW_SHOWDEFAULT);
}
//----< factory for creating messages >------------------------------
/*
* This function only creates one type of message for this demo.
* - To do that the first argument is 1, e.g., index for the type of message to create.
* - The body may be an empty string.
* - EndPoints are strings of the form ip:port, e.g., localhost:8081. This argument
*   expects the receiver EndPoint for the toAddr attribute.
*/


HttpMessage MsgClient::makeMessage(size_t n, const std::string& body, const EndPoint& ep, std::string type, int category)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:8081";  
	switch (n){
	case 1:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("POST", "Message"));
		msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
		msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));
		msg.addAttribute(HttpMessage::attribute("Category",std::to_string(category)));
		if (type == "upload") {
			msg.addAttribute(HttpMessage::attribute("Type", "upload"));
			msg.addAttribute(HttpMessage::attribute("Category", std::to_string(category)));
}else if (type == "download") {
			msg.addAttribute(HttpMessage::attribute("Type", "download"));
			msg.addAttribute(HttpMessage::attribute("Category", std::to_string(category)));
}else if (type == "publish") {
			msg.addAttribute(HttpMessage::attribute("Type", "publish"));
			msg.addAttribute(HttpMessage::attribute("Category", std::to_string(category)));
}else if (type == "display") {
			msg.addAttribute(HttpMessage::attribute("Type", "display"));
			msg.addAttribute(HttpMessage::attribute("Category", std::to_string(category)));
}else if (type == "delete") {
			msg.addAttribute(HttpMessage::attribute("Type","delete"));
			msg.addAttribute(HttpMessage::attribute("Category", std::to_string(category)));
}		msg.addBody(body);
		if (body.size() > 0){
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

	std::string fqname = "../TestFiles/" + filename;
	//std::string fqname = "../Repository/SourceCodeFiles/Category1"  + '/' + filename + ".snt";
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;

	HttpMessage msg = makeMessage(1, "", "localhost::8080", "upload",category);
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
//----< this function uploads files to server>--------------------
void MsgClient::uploadFiles(int category) {
	//  send all *.cpp files from TestFiles folder
	SocketSystem ss;
	SocketConnecter si;
	while (!si.connect("localhost", 8080))
	{
		//Show::write("\n client waiting to connect");
		::Sleep(100);
	}
	std::vector<std::string> files = FileSystem::Directory::getFiles("../TestFiles", "*.cpp");
	std::vector<std::string> files2 = FileSystem::Directory::getFiles("../TestFiles", "*.h");
	for (std::string str : files2)
	{
		files.push_back(str);

	}
	for (size_t i = 0; i < files.size(); ++i)
	{
		Show::write("\n\n  sending file " + files[i]);
		sendFile(files[i], si, category);
	}
	std::cout << "Uploaded successfully" << std::endl;
	HttpMessage doneMsg;

	doneMsg = makeMessage(1, "all done", "toAddr:localhost:8080", "upload", category);
	sendMessage(doneMsg, si);
	std::thread t1(
		[&]() { listener(); } // 20 messages 100 millisec apart
	);

	t1.join();
	return;
}
//----< this function sends message to delete files in server >--------------------
void MsgClient::remove(int category)
{
	SocketSystem ss;
	SocketConnecter si;
	while (!si.connect("localhost", 8080))
	{
		//Show::write("\n client waiting to connect");
		::Sleep(100);
	}

	HttpMessage msg;
	std::string msgBody = "remove body";
		
	msg = makeMessage(1, msgBody, "localhost:8080", "delete", category);
	sendMessage(msg, si);
}

//----< this function displays files from a particular category>--------------------
void MsgClient::display(int category)
{
	SocketSystem ss;
	SocketConnecter si;
	while (!si.connect("localhost", 8080))
	{
		//Show::write("\n client waiting to connect");
		::Sleep(100);
	}

	HttpMessage msg;
	std::string msgBody = "remove body";

	msg = makeMessage(1, msgBody, "localhost:8080", "display", category);
	sendMessage(msg, si);
}
//----< this function publishes files at  a specific category in a server>--------------------
void MsgClient::publish(int category)
{
	SocketSystem ss;
	SocketConnecter si;
	while (!si.connect("localhost", 8080))
	{
		//Show::write("\n client waiting to connect");
		::Sleep(100);
	}

	HttpMessage msg;
	std::string msgBody = "remove body";

	msg = makeMessage(1, msgBody, "localhost:8080", "publish", category);
	sendMessage(msg, si);
}
//----< this function is used to download files from the client>--------------------
void MsgClient::download(int category) {
	SocketSystem ss;
	SocketConnecter si;
	while (!si.connect("localhost", 8080))
	{
		//	Show::write("\n client waiting to connect");
		::Sleep(100);
	}

	HttpMessage msg;
	//MsgClient c1;
	std::string msgBody = "DLRequest";
	msg = makeMessage(1, msgBody, "localhost:8080","download",category);
	msg.addAttribute(HttpMessage::attribute("Category", std::to_string(category)));
	sendMessage(msg, si);
	

	autoOpen(category);
}

void MsgClient::execute(const size_t TimeBetweenMessages, const size_t NumMessages)
{
	// send NumMessages messages

	ClientCounter counter;
	size_t myCount = counter.count();
	std::string myCountString = Utilities::Converter<size_t>::toString(myCount);
	try{
		SocketSystem ss;
		SocketConnecter si; 
		while (!si.connect("localhost", 8080)){
			::Sleep(100);
}	HttpMessage msg;
	for (size_t i = 0; i < NumMessages; ++i){
				std::string msgBody =
				"<msg>Message #" + Converter<size_t>::toString(i + 1) +
				" from client #" + myCountString + "</msg>";
			msg = makeMessage(1, msgBody, "localhost:8080", "upload",1);
			sendMessage(msg, si);
			::Sleep(TimeBetweenMessages);
}
		// shut down server's client handler

		msg = makeMessage(1, "quit", "toAddr:localhost:8080", "upload",1);
		}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}
//----< the client listens to responses from the  >------

void MsgClient::listener()
{
::SetConsoleTitle(L"HttpMessage Client ");
	Async::BlockingQueue<HttpMessage> msgQ;
	try{
		SocketSystem ss;
		SocketListener sl(8081, Socket::IP6);
		ClientHandler cp(msgQ);
		sl.start(cp);
		while (true)
		{
			HttpMessage msg = msgQ.deQ();
			//Show::write("\n\n  server recvd message with body contents:\n" + msg.bodyString());
			if (msg.bodyString() == "ok") {
				sl.close();
				break;
			}

			if (msg.findValue("Type") == "display") {
				std::cout << std::endl;
				std::cout << msg.bodyString();
			}

		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}}



int main()
{	
	try {
		::SetConsoleTitle(L"Clients Running on Threads");

		MsgClient c1;

		std::cout << "*********************************\n Requirement 9 Test Executive *********************************\n";
		std::cout << " *********************************\nRequirement 1 Partly Satisfied. My project #4 doesn't have a GUI********************************* \n";
		std::cout << "*********************************\n Requirement 2 Satisfied *********************************\n";
		Sleep(1000);

		//c1.execute(100, 1);
		std::cout << "*********************************\n Requirement 4 Satisfied. My project #4 uses project #3 publishing functionality *********************************\n";
		std::cout << " *********************************\nRequirement 6 Project #4 consists of message passing communication *********************************\n";
		std::cout << " *********************************\nRequirement 5 Files from the directoet TestFiles is being uploaded to the server *********************************\n";
		std::cout << " *********************************\nRequirement 8 Satisfied *********************************\n";
		c1.uploadFiles(3);
		std::cout << "*********************************\n Upload Successful to category 3*********************************\n";
		std::cout << " *********************************\nRequirement 7 this project consists of asynchronous communication the message body contents are printed on the console \n";
		c1.remove(3);
		std::cout << "*********************************\n Files deleted from category 3 category 3*********************************\n";
		c1.uploadFiles(3);
		std::cout << "*********************************\n Upload Successful to category 3*********************************\n";
		c1.publish(3);
		std::cout << "*********************************\n Successfully published category 3 on the server side*********************************\n";
		c1.download(3);
		std::cout << "*********************************\n Successfully downloaded files to category 3 on the client side*********************************\n";
		std::cout << "*********************************\n Displaying the list of files from Category 3 on the server side*********************************\n";
		c1.display(3);
		std::thread t1(
			[&]() { c1.listener(); }
		);

		t1.join();

	}catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}

}

