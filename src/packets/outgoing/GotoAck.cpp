#include "GotoAck.h"


// Constructors
GotoAck::GotoAck()
{
	// Set packet id
	this->type_ = PacketType::GOTOACK;
}
GotoAck::GotoAck(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type_ = PacketType::GOTOACK;
	read();
}
GotoAck::GotoAck(Packet &p) : Packet(p)
{
	this->type_ = PacketType::GOTOACK;
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