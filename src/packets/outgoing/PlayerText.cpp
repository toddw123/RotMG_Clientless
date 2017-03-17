#include "PlayerText.h"


// Constructors
PlayerText::PlayerText()
{
	// Set packet id
	this->_type = PacketType::PLAYERTEXT;
}
PlayerText::PlayerText(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->_type = PacketType::PLAYERTEXT;
	read();
}
PlayerText::PlayerText(Packet &p) : Packet(p)
{
	this->_type = PacketType::PLAYERTEXT;
	read();
}

Packet *PlayerText::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(text);
	// Send the packet
	return this;
}

void PlayerText::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	text = this->readString<short>();
	// done!
}