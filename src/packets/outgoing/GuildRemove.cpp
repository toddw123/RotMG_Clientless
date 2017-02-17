#include "GuildRemove.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
GuildRemove::GuildRemove()
{
	// Set packet id
	this->id = PacketType::GUILDREMOVE;
}
GuildRemove::GuildRemove(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::GUILDREMOVE;
	Parse();
}
GuildRemove::GuildRemove(Packet &p) : Packet(p)
{
	this->id = PacketType::GUILDREMOVE;
	Parse();
}

void GuildRemove::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(name);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void GuildRemove::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	name = this->readString<short>();
	// done!
}

void GuildRemove::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}