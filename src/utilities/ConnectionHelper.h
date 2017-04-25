#pragma once

#ifndef CONNECTIONHELPER_H
#define CONNECTIONHELPER_H

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
	#define _WINSOCK_DEPRECATED_NO_WARNINGS // stupid visual studios throwing errors over fopen and shit
	#include <winsock.h>
#elif defined(__unix__) || defined(__linux__) || defined (__linux)
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>

	#define closesocket(s) close(s)
#endif

#ifndef _SOCKET
	#define _SOCKET
	typedef int SOCKET;
	#define INVALID_SOCKET (SOCKET)(~0)
#endif

#include <unordered_map>

class ConnectionHelper
{
public:
	static std::unordered_map<std::string, std::string> servers;

	static std::string getRandomServer();
	static std::string getServerName(std::string);
	static std::string getServerIp(std::string);
	static SOCKET connectToServer(const char*, short);
	//static void PrintLastError(DWORD);
};

#endif