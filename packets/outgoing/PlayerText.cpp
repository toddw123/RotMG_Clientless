#include "PlayerText.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
PlayerText::PlayerText()
{
	// Set packet id
	this->id = PacketType::PLAYERTEXT;
}
PlayerText::PlayerText(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::PLAYERTEXT;
	Parse();
}
PlayerText::PlayerText(Packet &p) : Packet(p)
{
	this->id = PacketType::PLAYERTEXT;
	Parse();
}

void PlayerText::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(text);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void PlayerText::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	text = this->readString<short>();
	// done!
}

void PlayerText::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}