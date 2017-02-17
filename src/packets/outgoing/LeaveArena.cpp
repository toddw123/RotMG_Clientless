#include "LeaveArena.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"


// Constructor
LeaveArena::LeaveArena()
{
	this->id = PacketType::ACCEPT_ARENA_DEATH;
}
LeaveArena::LeaveArena(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::ACCEPT_ARENA_DEATH;
	Parse();
}
LeaveArena::LeaveArena(Packet &p) : Packet(p)
{
	this->id = PacketType::ACCEPT_ARENA_DEATH;
	Parse();
}

void LeaveArena::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(time);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void LeaveArena::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	time = this->readBytes<int>();
	// done!
}

void LeaveArena::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}