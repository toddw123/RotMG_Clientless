#include "GuildInvite.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
GuildInvite::GuildInvite()
{
	// Set packet id
	this->id = PacketType::GUILDINVITE;
}
GuildInvite::GuildInvite(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::GUILDINVITE;
	Parse();
}
GuildInvite::GuildInvite(Packet &p) : Packet(p)
{
	this->id = PacketType::GUILDINVITE;
	Parse();
}

void GuildInvite::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(name);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void GuildInvite::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	name = this->readString<short>();
	// done!
}

void GuildInvite::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}