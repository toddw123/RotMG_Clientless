#include "AcceptArenaDeath.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"


// Constructor
AcceptArenaDeath::AcceptArenaDeath()
{
	this->id = PacketType::ACCEPT_ARENA_DEATH;
}
AcceptArenaDeath::AcceptArenaDeath(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::ACCEPT_ARENA_DEATH;
	Parse();
}
AcceptArenaDeath::AcceptArenaDeath(Packet &p) : Packet(p)
{
	this->id = PacketType::ACCEPT_ARENA_DEATH;
	Parse();
}

void AcceptArenaDeath::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data

	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void AcceptArenaDeath::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data

	// done!
}

void AcceptArenaDeath::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}