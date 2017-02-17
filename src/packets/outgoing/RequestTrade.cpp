#include "RequestTrade.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
RequestTrade::RequestTrade()
{
	// Set packet id
	this->id = PacketType::REQUESTTRADE;
}
RequestTrade::RequestTrade(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::REQUESTTRADE;
	Parse();
}
RequestTrade::RequestTrade(Packet &p) : Packet(p)
{
	this->id = PacketType::REQUESTTRADE;
	Parse();
}

void RequestTrade::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(name);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void RequestTrade::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	name = this->readString<short>();
	// done!
}

void RequestTrade::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}