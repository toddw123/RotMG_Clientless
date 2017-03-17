#include "Load.h"



// Constructor
Load::Load()
{
	this->type = PacketType::LOAD;
}
Load::Load(byte *b, int i) : Packet(b, i)
{
	this->type = PacketType::LOAD;
	read();
}
Load::Load(Packet &p) : Packet(p)
{
	this->type = PacketType::LOAD;
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
	isFromArena = this->readBool();
	// done!
}