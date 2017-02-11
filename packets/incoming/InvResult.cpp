#include "InvResult.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
InvResult::InvResult()
{
	// Set packet id
	this->id = PacketType::INVRESULT;
}
InvResult::InvResult(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::INVRESULT;
	Parse();
}
InvResult::InvResult(const Packet &p) : Packet(p)
{
	this->id = PacketType::INVRESULT;
	Parse();
}

void InvResult::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(result);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void InvResult::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	result = this->readBytes<int>();
	// done!
}

void InvResult::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}