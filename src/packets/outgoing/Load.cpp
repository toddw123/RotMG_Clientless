#include "Load.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"


// Constructor
Load::Load()
{
	this->id = PacketType::LOAD;
}
Load::Load(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::LOAD;
	Parse();
}
Load::Load(Packet &p) : Packet(p)
{
	this->id = PacketType::LOAD;
	Parse();
}

void Load::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(charId);
	this->writeBytes<bool>(isFromArena);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void Load::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	charId = this->readBytes<int>();
	isFromArena = this->readBytes<bool>();
	// done!
}

void Load::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}