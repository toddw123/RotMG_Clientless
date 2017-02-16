#include "TradeRequested.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
TradeRequested::TradeRequested()
{
	// Set packet id
	this->id = PacketType::TRADEREQUESTED;
}
TradeRequested::TradeRequested(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::TRADEREQUESTED;
	Parse();
}
TradeRequested::TradeRequested(const Packet &p) : Packet(p)
{
	this->id = PacketType::TRADEREQUESTED;
	Parse();
}

void TradeRequested::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(name);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void TradeRequested::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	name = this->readString<short>();
	// done!
}

void TradeRequested::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}