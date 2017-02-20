#include "GotoAck.h"
#include "../PacketType.h"

// Constructors
GotoAck::GotoAck()
{
	// Set packet id
	this->id = PacketType::GOTOACK;
}
GotoAck::GotoAck(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::GOTOACK;
	read();
}
GotoAck::GotoAck(Packet &p) : Packet(p)
{
	this->id = PacketType::GOTOACK;
	read();
}

Packet *GotoAck::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(time);
	// Send the packet
	return this;
}

void GotoAck::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	time = this->readBytes<int>();
	// done!
}