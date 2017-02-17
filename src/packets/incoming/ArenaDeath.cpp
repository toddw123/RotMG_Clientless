#include "ArenaDeath.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
ArenaDeath::ArenaDeath()
{
	// Set packet id
	this->id = PacketType::ARENA_DEATH;
}
ArenaDeath::ArenaDeath(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::ARENA_DEATH;
	Parse();
}
ArenaDeath::ArenaDeath(const Packet &p) : Packet(p)
{
	this->id = PacketType::ARENA_DEATH;
	Parse();
}

void ArenaDeath::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(cost);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void ArenaDeath::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	cost = this->readBytes<int>();
	// done!
}

void ArenaDeath::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}