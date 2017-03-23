#include "BuyResult.h"


// Constructors
BuyResult::BuyResult()
{
	// Set packet id
	this->type_ = PacketType::BUYRESULT;
}
BuyResult::BuyResult(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type_ = PacketType::BUYRESULT;
	read();
}
BuyResult::BuyResult(const Packet &p) : Packet(p)
{
	this->type_ = PacketType::BUYRESULT;
	read();
}

Packet *BuyResult::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(result);
	this->writeString<short>(resultString);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void BuyResult::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	result = this->readBytes<int>();
	resultString = this->readString<short>();
	// done!
}