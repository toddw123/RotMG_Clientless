#include "Teleport.h"
#include "../PacketType.h"

// Constructors
Teleport::Teleport()
{
	// Set packet id
	this->type_ = PacketType::TELEPORT;
}
Teleport::Teleport(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type_ = PacketType::TELEPORT;
	read();
}
Teleport::Teleport(Packet &p) : Packet(p)
{
	this->type_ = PacketType::TELEPORT;
	read();
}

Packet *Teleport::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(objectId);
	// Send the packet
	return this;
}

void Teleport::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	objectId = this->readBytes<int>();
	// done!
}