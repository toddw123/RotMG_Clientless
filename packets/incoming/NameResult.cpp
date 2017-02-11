#include "NameResult.h"
#include "../PacketIOHelper.h"
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
	Parse();
}
NameResult::NameResult(const Packet &p) : Packet(p)
{
	this->id = PacketType::NAMERESULT;
	Parse();
}

void NameResult::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<bool>(success);
	this->writeString<short>(errorText);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void NameResult::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	success = this->readBytes<bool>();
	errorText = this->readString<short>();
	// done!
}

void NameResult::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}