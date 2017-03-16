#include "Create.h"



// Constructor
Create::Create()
{
	this->type = PacketType::CREATE;
}
Create::Create(byte *b, int i) : Packet(b, i)
{
	this->type = PacketType::CREATE;
	read();
}
Create::Create(Packet &p) : Packet(p)
{
	this->type = PacketType::CREATE;
	read();
}

Packet *Create::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<short>(classType);
	this->writeBytes<short>(skinType);
	// Send the packet
	return this;
}

void Create::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	classType = this->readBytes<short>();
	skinType = this->readBytes<short>();
	// done!
}