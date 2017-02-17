#include "TradeDone.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
TradeDone::TradeDone()
{
	// Set packet id
	this->id = PacketType::TRADEDONE;
}
TradeDone::TradeDone(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::TRADEDONE;
	Parse();
}
TradeDone::TradeDone(const Packet &p) : Packet(p)
{
	this->id = PacketType::TRADEDONE;
	Parse();
}

void TradeDone::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(code);
	this->writeString<short>(description);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void TradeDone::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	code = this->readBytes<int>();
	description = this->readString<short>();
	// done!
}

void TradeDone::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}