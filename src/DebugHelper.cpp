#include "DebugHelper.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "packets/PacketType.h"

// Add whatever debug info you might want to this
void DebugHelper::pinfo(int pid, int len)
{
	DebugHelper::print("S -> C: %s Packet (size = %d)\n", GetStringPacketType(PacketType(pid)) == NULL ? "UNKNOWN" : GetStringPacketType(PacketType(pid)), len);
}

void DebugHelper::print(const char* format, ...)
{
#ifdef DEBUG_OUTPUT
	va_list argptr;
	va_start(argptr, format);
	vfprintf(stderr, format, argptr);
	va_end(argptr);
#endif
}

std::string DebugHelper::timestamp()
{
	time_t rawtime = time(NULL);
	struct tm* timeinfo = localtime(&rawtime);
	char date[20];
	strftime(date, 20, "%F %T", timeinfo);

	std::string retstr = date;
	return retstr;
}