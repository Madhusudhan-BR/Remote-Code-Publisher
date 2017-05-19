#pragma once
/////////////////////////////////////////////////////////////////////////
// newServerHeader.h      - Implements   simple two-way HTTP style messaging    //
//                 and file transfer                                   //
//                                                                     //
// Modified by :Madhusudhan B R, OOD, Spring 2017 .						//
//	original : James Fawcett																	//		
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Macbook Pro  , Windows 10          //
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a server that receives HTTP style messages and
* files from multiple concurrent clients and simply displays the messages
* and stores files, deletes files, publish dependency files and so on.
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

Maintanance History:
version 1.0 ::
Implements   simple two-way HTTP style messaging
and file transfer

*/
#include "../NewHeaderNewCpp/HttpMessage/HttpMessage.h"
#include "../NewHeaderNewCpp/Sockets/Sockets.h"
#include "../NewHeaderNewCpp/FileSystem/FileSystem.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../Logger/Logger.h"

#include "../Utilities/Utilities.h"
#include <string>
#include <fstream>
#include <iostream>

#include "../FileSystem/FileSystem.h"

#include "../FileMgr/FileMgr.h"

#include "../Analyzer/Executive.h"
#include <Windows.h>
#include <shellapi.h>

using Show = Logging::StaticLogger<1>;
using namespace Utilities;


class ClientHandler
{
public:
	ClientHandler(Async::BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
	std::vector<std::string> fileNames;
	
private:
	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	bool readFile(const std::string& filename, size_t fileSize, Socket& socket, int category, std::string type);
	Async::BlockingQueue<HttpMessage>& msgQ_;
};




//----< this defines processing to frame messages >------------------

HttpMessage ClientHandler::readMessage(Socket& socket)
{	connectionClosed_ = false;
	HttpMessage msg;
	while (true){
		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1){
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);}
		else{break;}}
	if (msg.attributes().size() == 0){
		connectionClosed_ = true;
		return msg;
}		if (msg.attributes()[0].first == "POST"){
		std::string filename = msg.findValue("file");
		if (filename != ""){
			size_t contentSize;
			std::string sizeString = msg.findValue("content-length");
			if (sizeString != "")
				contentSize = Converter<size_t>::toValue(sizeString);
			else
				return msg;
			std::string category = msg.findValue("Category");
			std::string type = msg.findValue("Type");
			readFile(filename, contentSize, socket, std::atoi(category.c_str()), type);
}if (filename != ""){
			msg.removeAttribute("content-length");
			std::string bodyString = "<file>" + filename + "</file>";
			std::string sizeString = Converter<size_t>::toString(bodyString.size());
			msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
			msg.addBody(bodyString);
}else{		size_t numBytes = 0;
			size_t pos = msg.findAttribute("content-length");
			if (pos < msg.attributes().size()){
				numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
				Socket::byte* buffer = new Socket::byte[numBytes + 1];
				socket.recv(numBytes, buffer);
				buffer[numBytes] = '\0';
				std::string msgBody(buffer);
				msg.addBody(msgBody);
				delete[] buffer;
			}}}
	return msg;}
//----< read a binary file from socket and save >--------------------
/*
* This function expects the sender to have already send a file message,
* and when this function is running, continuosly send bytes until
* fileSize bytes have been sent.
*/
bool ClientHandler::readFile(const std::string& filename, size_t fileSize, Socket& socket, int category, std::string type)
{
	std::string fqname = "../Server/Repository/SourceCodeFiles/Category" + std::to_string(category);
	fqname = fqname + '/' + filename ;

	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood()){
		Show::write("\n\n  can't open file " + fqname);
		return false;
}	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	size_t bytesToRead;
	while (true){
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;

		socket.recv(bytesToRead, buffer);

		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);

		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	file.close();
	return true;
}
//----< receiver functionality is defined by this function >---------

void ClientHandler::operator()(Socket socket)
{
	while (true)
	{
		HttpMessage msg = readMessage(socket);
		if (connectionClosed_ || msg.bodyString() == "quit")
		{
			Show::write("\n\n  clienthandler thread is terminating");
			break;
		}

		msgQ_.enQ(msg);
	}
}
