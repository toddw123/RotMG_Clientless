#include "JoinGuild.h"


// Constructors
JoinGuild::JoinGuild()
{
	// Set packet id
	this->type_ = PacketType::JOINGUILD;
}
JoinGuild::JoinGuild(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type_ = PacketType::JOINGUILD;
	read();
}
JoinGuild::JoinGuild(Packet &p) : Packet(p)
{
	this->type_ = PacketType::JOINGUILD;
	read();
}

Packet *JoinGuild::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(guildName);
	// Send the packet
	return this;
}

void JoinGuild::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	guildName = this->readString<short>();
	// done!
}