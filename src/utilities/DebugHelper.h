#pragma once

#ifndef DEBUGHELPER_H
#define DEBUGHELPER_H

// _DEBUG_OUTPUT_ is in the project config. In debug mode, its defined, in release mode its not.

class DebugHelper
{
public:
	static void print(const char* format, ...);
	static void pinfo(int, int);  
};

#endif