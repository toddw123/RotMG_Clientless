#include "TradeRequested.h"


// Constructors
TradeRequested::TradeRequested()
{
	// Set packet id
	this->_type = PacketType::TRADEREQUESTED;
}
TradeRequested::TradeRequested(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->_type = PacketType::TRADEREQUESTED;
	read();
}
TradeRequested::TradeRequested(const Packet &p) : Packet(p)
{
	this->_type = PacketType::TRADEREQUESTED;
	read();
}

Packet *TradeRequested::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(name);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void TradeRequested::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	name = this->readString<short>();
	// done!
}