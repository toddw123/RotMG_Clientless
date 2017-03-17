#include "ArenaDeath.h"


// Constructors
ArenaDeath::ArenaDeath()
{
	// Set packet id
	this->_type = PacketType::ARENA_DEATH;
}
ArenaDeath::ArenaDeath(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->_type = PacketType::ARENA_DEATH;
	read();
}
ArenaDeath::ArenaDeath(const Packet &p) : Packet(p)
{
	this->_type = PacketType::ARENA_DEATH;
	read();
}

Packet *ArenaDeath::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(cost);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void ArenaDeath::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	cost = this->readBytes<int>();
	// done!
}