#include "ShootAck.h"
#include "../PacketType.h"

// Constructors
ShootAck::ShootAck()
{
	// Set packet id
	this->id = PacketType::SHOOTACK;
}
ShootAck::ShootAck(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::SHOOTACK;
	read();
}
ShootAck::ShootAck(Packet &p) : Packet(p)
{
	this->id = PacketType::SHOOTACK;
	read();
}

Packet *ShootAck::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(time);
	// Send the packet
	return this;
}

void ShootAck::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	time = this->readBytes<int>();
	// done!
}