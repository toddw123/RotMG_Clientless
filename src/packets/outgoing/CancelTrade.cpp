#include "CancelTrade.h"
#include "..\PacketType.h"

// Constructors
CancelTrade::CancelTrade()
{
	// Set packet id
	this->id = PacketType::CANCELTRADE;
}
CancelTrade::CancelTrade(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::CANCELTRADE;
	read();
}
CancelTrade::CancelTrade(Packet &p) : Packet(p)
{
	this->id = PacketType::CANCELTRADE;
	read();
}

Packet *CancelTrade::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data (none to write)
	// Send the packet
	return this;
}

void CancelTrade::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data (...)
	// done!
}