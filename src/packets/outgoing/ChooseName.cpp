#include "ChooseName.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
ChooseName::ChooseName()
{
	// Set packet id
	this->id = PacketType::CHOOSENAME;
}
ChooseName::ChooseName(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::CHOOSENAME;
	Parse();
}
ChooseName::ChooseName(Packet &p) : Packet(p)
{
	this->id = PacketType::CHOOSENAME;
	Parse();
}

void ChooseName::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(name);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void ChooseName::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	name = this->readString<short>();
	// done!
}

void ChooseName::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}