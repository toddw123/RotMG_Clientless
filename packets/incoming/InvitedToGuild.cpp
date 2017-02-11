#include "InvitedToGuild.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
InvitedToGuild::InvitedToGuild()
{
	// Set packet id
	this->id = PacketType::INVITEDTOGUILD;
}
InvitedToGuild::InvitedToGuild(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::INVITEDTOGUILD;
	Parse();
}
InvitedToGuild::InvitedToGuild(const Packet &p) : Packet(p)
{
	this->id = PacketType::INVITEDTOGUILD;
	Parse();
}

void InvitedToGuild::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(name);
	this->writeString<short>(guildName);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void InvitedToGuild::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	name = this->readString<short>();
	guildName = this->readString<short>();
}

void InvitedToGuild::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}