#include "JoinGuild.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
JoinGuild::JoinGuild()
{
	// Set packet id
	this->id = PacketType::JOINGUILD;
}
JoinGuild::JoinGuild(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::JOINGUILD;
	Parse();
}
JoinGuild::JoinGuild(Packet &p) : Packet(p)
{
	this->id = PacketType::JOINGUILD;
	Parse();
}

void JoinGuild::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(guildName);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void JoinGuild::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	guildName = this->readString<short>();
	// done!
}

void JoinGuild::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}