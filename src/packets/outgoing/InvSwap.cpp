#include "InvSwap.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
InvSwap::InvSwap()
{
	// Set packet id
	this->id = PacketType::INVSWAP;
}
InvSwap::InvSwap(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::INVSWAP;
	Parse();
}
InvSwap::InvSwap(Packet &p) : Packet(p)
{
	this->id = PacketType::INVSWAP;
	Parse();
}


void InvSwap::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(time);
	position.Write(this);
	slotObject1.Write(this);
	slotObject2.Write(this);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void InvSwap::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	time = this->readBytes<int>();
	position.Read(this);
	slotObject1.Read(this);
	slotObject2.Read(this);
	// done!
}

void InvSwap::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}