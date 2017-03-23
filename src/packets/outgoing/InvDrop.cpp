#include "InvDrop.h"



// Constructor
InvDrop::InvDrop()
{
	this->type_ = PacketType::INVDROP;
}
InvDrop::InvDrop(byte *b, int i) : Packet(b, i)
{
	this->type_ = PacketType::INVDROP;
	read();
}
InvDrop::InvDrop(Packet &p) : Packet(p)
{
	this->type_ = PacketType::INVDROP;
	read();
}

Packet *InvDrop::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	slotObj.Write(this);
	// Send the packet
	return this;
}

void InvDrop::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	slotObj.Read(this);
	// done!
}