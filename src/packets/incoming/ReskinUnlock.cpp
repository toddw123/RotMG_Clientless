#include "ReskinUnlock.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
ReskinUnlock::ReskinUnlock()
{
	// Set packet id
	this->id = PacketType::RESKIN_UNLOCK;
}
ReskinUnlock::ReskinUnlock(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::RESKIN_UNLOCK;
	Parse();
}
ReskinUnlock::ReskinUnlock(const Packet &p) : Packet(p)
{
	this->id = PacketType::RESKIN_UNLOCK;
	Parse();
}

void ReskinUnlock::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(skinId);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void ReskinUnlock::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	skinId = this->readBytes<int>();
	// done!
}

void ReskinUnlock::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}