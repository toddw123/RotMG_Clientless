#include "Load.h"
#include "../PacketType.h"


// Constructor
Load::Load()
{
	this->id = PacketType::LOAD;
}
Load::Load(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::LOAD;
	read();
}
Load::Load(Packet &p) : Packet(p)
{
	this->id = PacketType::LOAD;
	read();
}

Packet *Load::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(charId);
	this->writeBytes<bool>(isFromArena);
	// Send the packet
	return this;
}

void Load::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	charId = this->readBytes<int>();
	isFromArena = this->readBytes<bool>();
	// done!
}