#include "TradeDone.h"


// Constructors
TradeDone::TradeDone()
{
	// Set packet id
	this->_type = PacketType::TRADEDONE;
}
TradeDone::TradeDone(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->_type = PacketType::TRADEDONE;
	read();
}
TradeDone::TradeDone(const Packet &p) : Packet(p)
{
	this->_type = PacketType::TRADEDONE;
	read();
}

Packet *TradeDone::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(code);
	this->writeString<short>(description);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void TradeDone::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	code = this->readBytes<int>();
	description = this->readString<short>();
	// done!
}