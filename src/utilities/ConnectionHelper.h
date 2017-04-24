#pragma once

#ifndef CONNECTIONHELPER_H
#define CONNECTIONHELPER_H

#ifdef __WIN32__
	#define _WINSOCK_DEPRECATED_NO_WARNINGS // stupid visual studios throwing errors over fopen and shit
	#include <winsock.h>
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>
#endif

#include <unordered_map>

class ConnectionHelper
{
public:
	static std::unordered_map<std::string, std::string> servers;

	static std::string getRandomServer();
	static std::string getServerName(std::string);
	static std::string getServerIp(std::string);
	#ifdef __WIN32__
	static SOCKET connectToServer(const char*, short);
	#else
	static int connectToServer(const char*, short);
	#endif
	//static void PrintLastError(DWORD);
};

#endif