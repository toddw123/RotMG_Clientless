#include "InvDrop.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"


// Constructor
InvDrop::InvDrop()
{
	this->id = PacketType::INVDROP;
}
InvDrop::InvDrop(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::INVDROP;
	Parse();
}
InvDrop::InvDrop(Packet &p) : Packet(p)
{
	this->id = PacketType::INVDROP;
	Parse();
}

void InvDrop::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	slotObj.Write(this);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void InvDrop::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	slotObj.Read(this);
	// done!
}

void InvDrop::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}