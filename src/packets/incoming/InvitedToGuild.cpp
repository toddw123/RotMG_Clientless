#include "InvitedToGuild.h"


// Constructors
InvitedToGuild::InvitedToGuild()
{
	// Set packet id
	this->_type = PacketType::INVITEDTOGUILD;
}
InvitedToGuild::InvitedToGuild(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->_type = PacketType::INVITEDTOGUILD;
	read();
}
InvitedToGuild::InvitedToGuild(const Packet &p) : Packet(p)
{
	this->_type = PacketType::INVITEDTOGUILD;
	read();
}

Packet *InvitedToGuild::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(name);
	this->writeString<short>(guildName);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void InvitedToGuild::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	name = this->readString<short>();
	guildName = this->readString<short>();
}