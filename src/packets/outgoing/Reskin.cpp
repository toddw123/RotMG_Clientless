#include "Reskin.h"



// Constructor
Reskin::Reskin()
{
	this->type = PacketType::RESKIN;
}
Reskin::Reskin(byte *b, int i) : Packet(b, i)
{
	this->type = PacketType::RESKIN;
	read();
}
Reskin::Reskin(Packet &p) : Packet(p)
{
	this->type = PacketType::RESKIN;
	read();
}

Packet *Reskin::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(skinId);
	// Send the packet
	return this;
}

void Reskin::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	skinId = this->readBytes<int>();
	// done!
}