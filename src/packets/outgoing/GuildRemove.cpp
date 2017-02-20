#include "GuildRemove.h"
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
	read();
}
GuildRemove::GuildRemove(Packet &p) : Packet(p)
{
	this->id = PacketType::GUILDREMOVE;
	read();
}

Packet *GuildRemove::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(name);
	// Send the packet
	return this;
}

void GuildRemove::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	name = this->readString<short>();
	// done!
}