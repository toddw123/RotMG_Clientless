#include "ChangeGuildRank.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"


// Constructor
ChangeGuildRank::ChangeGuildRank()
{
	this->id = PacketType::CHANGEGUILDRANK;
}
ChangeGuildRank::ChangeGuildRank(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::CHANGEGUILDRANK;
	Parse();
}
ChangeGuildRank::ChangeGuildRank(Packet &p) : Packet(p)
{
	this->id = PacketType::CHANGEGUILDRANK;
	Parse();
}

void ChangeGuildRank::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(name);
	this->writeBytes<int>(guildRank);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void ChangeGuildRank::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	name = this->readString<short>();
	guildRank = this->readBytes<int>();
	// done!
}

void ChangeGuildRank::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}