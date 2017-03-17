#include "InvSwap.h"


// Constructors
InvSwap::InvSwap()
{
	// Set packet id
	this->_type = PacketType::INVSWAP;
}
InvSwap::InvSwap(byte *b, int i) : Packet(b, i)
{
	this->_type = PacketType::INVSWAP;
	read();
}
InvSwap::InvSwap(Packet &p) : Packet(p)
{
	this->_type = PacketType::INVSWAP;
	read();
}


Packet *InvSwap::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(time);
	position.Write(this);
	slotObject1.Write(this);
	slotObject2.Write(this);
	// Send the packet
	return this;
}

void InvSwap::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	time = this->readBytes<int>();
	position.Read(this);
	slotObject1.Read(this);
	slotObject2.Read(this);
	// done!
}