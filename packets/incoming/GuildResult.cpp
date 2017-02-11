#include "GuildResult.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
GuildResult::GuildResult()
{
	// Set packet id
	this->id = PacketType::GUILDRESULT;
}
GuildResult::GuildResult(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::GUILDRESULT;
	Parse();
}
GuildResult::GuildResult(const Packet &p) : Packet(p)
{
	this->id = PacketType::GUILDRESULT;
	Parse();
}

void GuildResult::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<bool>(success);
	this->writeString<short>(lineBuilderJson);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void GuildResult::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	success = this->readBytes<bool>();
	lineBuilderJson = this->readString<short>();
	// done!
}

void GuildResult::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}