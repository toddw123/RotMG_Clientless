#include "ConnectionHelper.h"

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
	sockAddr.sin_addr.S_un.S_addr = inet_addr(ip);
	// Create the connection to the server
	if (connect(sock, (sockaddr*)(&sockAddr), sizeof(sockAddr)) != 0)
	{
		closesocket(sock);
		return INVALID_SOCKET;
	}
	// Return the socket for use
	return sock;
}

void ConnectionHelper::PrintLastError(DWORD dwMessageId)
{
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
}