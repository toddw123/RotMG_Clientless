#include "CheckCredits.h"
#include "..\PacketType.h"

// Constructors
CheckCredits::CheckCredits()
{
	// Set packet id
	this->type = PacketType::CHECKCREDITS;
}
CheckCredits::CheckCredits(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type = PacketType::CHECKCREDITS;
	read();
}
CheckCredits::CheckCredits(Packet &p) : Packet(p)
{
	this->type = PacketType::CHECKCREDITS;
	read();
}

Packet *CheckCredits::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data (none to write)
	// Send the packet
	return this;
}

void CheckCredits::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data (...)
	// done!
}