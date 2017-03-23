#include "ChooseName.h"


// Constructors
ChooseName::ChooseName()
{
	// Set packet id
	this->type_ = PacketType::CHOOSENAME;
}
ChooseName::ChooseName(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type_ = PacketType::CHOOSENAME;
	read();
}
ChooseName::ChooseName(Packet &p) : Packet(p)
{
	this->type_ = PacketType::CHOOSENAME;
	read();
}

Packet *ChooseName::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(name);
	// Send the packet
	return this;
}

void ChooseName::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	name = this->readString<short>();
	// done!
}