#include "GuildResult.h"


// Constructors
GuildResult::GuildResult()
{
	// Set packet id
	this->type = PacketType::GUILDRESULT;
}
GuildResult::GuildResult(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type = PacketType::GUILDRESULT;
	read();
}
GuildResult::GuildResult(const Packet &p) : Packet(p)
{
	this->type = PacketType::GUILDRESULT;
	read();
}

Packet *GuildResult::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<bool>(success);
	this->writeString<short>(lineBuilderJson);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void GuildResult::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	success = this->readBool();
	lineBuilderJson = this->readString<short>();
	// done!
}