#include "Escape.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
PlayerEscape::PlayerEscape()
{
	// Set packet id
	this->id = PacketType::TELEPORT;
}
PlayerEscape::PlayerEscape(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::ESCAPE;
	Parse();
}
PlayerEscape::PlayerEscape(Packet &p) : Packet(p)
{
	this->id = PacketType::ESCAPE;
	Parse();
}

void PlayerEscape::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data (none to write)
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void PlayerEscape::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data (...)
	// done!
}

void PlayerEscape::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}