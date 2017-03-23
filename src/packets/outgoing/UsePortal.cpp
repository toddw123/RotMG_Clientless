#include "UsePortal.h"


// Constructors
UsePortal::UsePortal()
{
	// Set packet id
	this->type_ = PacketType::USEPORTAL;
}
UsePortal::UsePortal(byte *b, int i) : Packet(b, i)
{
	this->type_ = PacketType::USEPORTAL;
	read();
}
UsePortal::UsePortal(const Packet &p) : Packet(p)
{
	this->type_ = PacketType::USEPORTAL;
	read();
}

Packet *UsePortal::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(objectId);
	// Send the packet
	return this;
}

void UsePortal::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	objectId = this->readBytes<int>();
	// done!
}