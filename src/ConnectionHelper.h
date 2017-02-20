#pragma once

#ifndef CONNECTIONHELPER_H
#define CONNECTIONHELPER_H

#define _WINSOCK_DEPRECATED_NO_WARNINGS // stupid visual studios throwing errors over fopen and shit

#include <winsock.h>
#include <unordered_map>

class ConnectionHelper
{
public:
	static std::unordered_map<std::string, std::string> servers;

	static SOCKET connectToServer(const char*, short);
	static void PrintLastError(DWORD);
};

#endif