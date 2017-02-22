#include "PlayerHit.h"
#include "../PacketType.h"


// Constructor
PlayerHit::PlayerHit()
{
	this->id = PacketType::PLAYERHIT;
}
PlayerHit::PlayerHit(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::PLAYERHIT;
	read();
}
PlayerHit::PlayerHit(Packet &p) : Packet(p)
{
	this->id = PacketType::PLAYERHIT;
	read();
}

Packet *PlayerHit::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<byte>(bulletId);
	this->writeBytes<int>(objectId);
	// Send the packet
	return this;
}

void PlayerHit::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	bulletId = this->readBytes<byte>();
	objectId = this->readBytes<int>();
	// done!
}