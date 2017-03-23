#include "ReskinUnlock.h"


// Constructors
ReskinUnlock::ReskinUnlock()
{
	// Set packet id
	this->type_ = PacketType::RESKIN_UNLOCK;
}
ReskinUnlock::ReskinUnlock(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type_ = PacketType::RESKIN_UNLOCK;
	read();
}
ReskinUnlock::ReskinUnlock(const Packet &p) : Packet(p)
{
	this->type_ = PacketType::RESKIN_UNLOCK;
	read();
}

Packet *ReskinUnlock::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(skinId);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void ReskinUnlock::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	skinId = this->readBytes<int>();
	// done!
}