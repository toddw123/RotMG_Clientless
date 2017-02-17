#include "CancelTrade.h"
#include "..\PacketIOHelper.h"
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
	Parse();
}
CancelTrade::CancelTrade(Packet &p) : Packet(p)
{
	this->id = PacketType::CANCELTRADE;
	Parse();
}

void CancelTrade::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data (none to write)
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void CancelTrade::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data (...)
	// done!
}

void CancelTrade::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}