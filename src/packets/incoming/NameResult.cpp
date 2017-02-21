#include "NameResult.h"
#include "../PacketType.h"

// Constructors
NameResult::NameResult()
{
	// Set packet id
	this->id = PacketType::NAMERESULT;
}
NameResult::NameResult(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::NAMERESULT;
	read();
}
NameResult::NameResult(const Packet &p) : Packet(p)
{
	this->id = PacketType::NAMERESULT;
	read();
}

Packet *NameResult::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<bool>(success);
	this->writeString<short>(errorText);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void NameResult::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	success = this->readBool();
	errorText = this->readString<short>();
	// done!
}