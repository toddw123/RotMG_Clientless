#include "RequestTrade.h"


// Constructors
RequestTrade::RequestTrade()
{
	// Set packet id
	this->type = PacketType::REQUESTTRADE;
}
RequestTrade::RequestTrade(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type = PacketType::REQUESTTRADE;
	read();
}
RequestTrade::RequestTrade(Packet &p) : Packet(p)
{
	this->type = PacketType::REQUESTTRADE;
	read();
}

Packet *RequestTrade::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(name);
	// Send the packet
	return this;
}

void RequestTrade::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	name = this->readString<short>();
	// done!
}