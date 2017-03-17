#include "Escape.h"
#include "..\PacketType.h"

// Constructors
PlayerEscape::PlayerEscape()
{
	// Set packet id
	this->type = PacketType::ESCAPE;
}
PlayerEscape::PlayerEscape(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type = PacketType::ESCAPE;
	read();
}
PlayerEscape::PlayerEscape(Packet &p) : Packet(p)
{
	this->type = PacketType::ESCAPE;
	read();
}

Packet *PlayerEscape::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data (none to write)
	// Send the packet
	return this;
}

void PlayerEscape::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data (...)
	// done!
}