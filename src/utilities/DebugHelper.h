#pragma once

#ifndef DEBUGHELPER_H
#define DEBUGHELPER_H

// Comment out this line if you dont want to see debug output
#define DEBUG_OUTPUT

class DebugHelper
{
public:
	static void print(const char* format, ...);
	static void pinfo(int, int);
};

#endif