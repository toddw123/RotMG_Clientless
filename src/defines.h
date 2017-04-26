#pragma once

#ifndef DEFINES_H
#define DEFINES_H


#if defined(__unix__) || defined(__linux__) || defined(__linux)
	#define SLEEP(x) usleep(x * 1000);
	#define closesocket(s) close(s)

	typedef int SOCKET;
	#define INVALID_SOCKET (SOCKET)(~0)
#elif defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
	#define SLEEP(x) Sleep(x);
#endif




#endif