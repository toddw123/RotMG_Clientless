#include "EnterArena.h"



// Constructor
EnterArena::EnterArena()
{
	this->type = PacketType::LOAD;
}
EnterArena::EnterArena(byte *b, int i) : Packet(b, i)
{
	this->type = PacketType::LOAD;
	read();
}
EnterArena::EnterArena(Packet &p) : Packet(p)
{
	this->type = PacketType::LOAD;
	read();
}

Packet *EnterArena::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(currency);
	// Send the packet
	return this;
}

void EnterArena::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	currency = this->readBytes<int>();
	// done!
}