#include "EnterArena.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"


// Constructor
EnterArena::EnterArena()
{
	this->id = PacketType::LOAD;
}
EnterArena::EnterArena(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::LOAD;
	Parse();
}
EnterArena::EnterArena(Packet &p) : Packet(p)
{
	this->id = PacketType::LOAD;
	Parse();
}

void EnterArena::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(currency);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void EnterArena::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	currency = this->readBytes<int>();
	// done!
}

void EnterArena::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}