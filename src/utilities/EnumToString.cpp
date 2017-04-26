// File name: "EnumToString.cpp"

/// The strings associated with the enums are gererated here
/////////////////////////////////////////////////////////////////////

enum tagPacketType {};

#define GENERATE_ENUM_STRINGS  // Start string generation
#include "../packets/PacketType.h"             
#undef GENERATE_ENUM_STRINGS   // Stop string generation