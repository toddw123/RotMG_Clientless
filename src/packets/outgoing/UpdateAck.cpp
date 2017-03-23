#include "UpdateAck.h"



// Constructor
UpdateAck::UpdateAck()
{
	this->type_ = PacketType::UPDATEACK;
}
UpdateAck::UpdateAck(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type_ = PacketType::UPDATEACK;
	read();
}
UpdateAck::UpdateAck(Packet &p) : Packet(p)
{
	this->type_ = PacketType::UPDATEACK;
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