#include "DebugHelper.h"

#include <stdio.h>
#include <stdarg.h>

#include "../packets/PacketType.h"

// Add whatever debug info you might want to this
void DebugHelper::pinfo(int pid, int len)
{
	DebugHelper::print("S -> C: %s Packet (size = %d)\n", GetStringPacketType(PacketType(pid)) == NULL ? "UNKNOWN" : GetStringPacketType(PacketType(pid)), len);
}

void DebugHelper::print(const char* format, ...)
{
#ifdef _DEBUG_OUTPUT_
	va_list argptr;
	va_start(argptr, format);
	vfprintf(stderr, format, argptr);
	va_end(argptr);
#endif
}