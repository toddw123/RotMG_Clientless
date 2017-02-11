#include "BuyResult.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
BuyResult::BuyResult()
{
	// Set packet id
	this->id = PacketType::BUYRESULT;
}
BuyResult::BuyResult(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::BUYRESULT;
	Parse();
}
BuyResult::BuyResult(const Packet &p) : Packet(p)
{
	this->id = PacketType::BUYRESULT;
	Parse();
}

void BuyResult::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(result);
	this->writeString<short>(resultString);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void BuyResult::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	result = this->readBytes<int>();
	resultString = this->readString<short>();
	// done!
}

void BuyResult::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}