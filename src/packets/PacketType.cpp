#include "PacketType.h"

char const* GetStringPacketType(enum tagPacketType index)
{ 
	return gs_PacketType [index];
}
