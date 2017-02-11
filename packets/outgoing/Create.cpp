#include "Create.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"


// Constructor
Create::Create()
{
	this->id = PacketType::CREATE;
}
Create::Create(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::CREATE;
	Parse();
}
Create::Create(Packet &p) : Packet(p)
{
	this->id = PacketType::CREATE;
	Parse();
}

void Create::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<short>(classType);
	this->writeBytes<short>(skinType);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void Create::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	classType = this->readBytes<short>();
	skinType = this->readBytes<short>();
	// done!
}

void Create::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}