#include "ConnectionHelper.h"
#include "RandomUtil.h"

std::unordered_map<std::string, std::string> ConnectionHelper::servers;

SOCKET ConnectionHelper::connectToServer(const char *ip, short port)
{
	// Create TCP socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		return INVALID_SOCKET;
	}

	// Build sockaddr struct
	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr.s_addr = inet_addr(ip);
	// Create the connection to the server
	if (connect(sock, (sockaddr*)(&sockAddr), sizeof(sockAddr)) != 0)
	{
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
		ConnectionHelper::PrintLastError();
#endif
		closesocket(sock);
		return INVALID_SOCKET;
	}
	// Return the socket for use
	return sock;
}



void ConnectionHelper::PrintLastError()
{
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
	DWORD dwMessageId = WSAGetLastError();
	LPTSTR s;
	// Attempt to get the actual message of an error code
	int ret = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwMessageId, 0, (LPTSTR)&s, 0, NULL);
	if (ret == 0)
	{
		printf("Format message failed with 0x%x\n", GetLastError());
	}
	else
	{
		printf("Error: %s\n", s);
		LocalFree(s);
	}
#endif
}


std::string ConnectionHelper::getRandomServer()
{
	int i = 0, r = RandomUtil::genRandomInt(0, (int)servers.size());
	for (std::unordered_map<std::string, std::string>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		if (r == i)
			return it->second;
		i++;
	}
	// Should never reach here, but return first string anyways
	return servers.begin()->second;
}

std::string ConnectionHelper::getServerName(std::string ip)
{
	for (std::unordered_map<std::string, std::string>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		if (it->second == ip)
			return it->first;
	}
	return "";
}

std::string ConnectionHelper::getServerIp(std::string name)
{
	if (servers.find(name) == servers.end())
		return "";
	
	return servers[name];
}