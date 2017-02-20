#include "UpdateAck.h"
#include "../PacketType.h"


// Constructor
UpdateAck::UpdateAck()
{
	this->id = PacketType::UPDATEACK;
}
UpdateAck::UpdateAck(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::UPDATEACK;
	read();
}
UpdateAck::UpdateAck(Packet &p) : Packet(p)
{
	this->id = PacketType::UPDATEACK;
	//read();
}

Packet *UpdateAck::write()
{
	// Clear the packet data
	this->clearData();
	// Literally nothing to write for this packet

	// Send the packet
	return this;
}

void UpdateAck::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// No data to read
}