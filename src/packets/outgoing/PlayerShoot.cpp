#include "PlayerShoot.h"


// Constructors
PlayerShoot::PlayerShoot()
{
	// Set packet id
	this->type_ = PacketType::PLAYERSHOOT;
}
PlayerShoot::PlayerShoot(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type_ = PacketType::PLAYERSHOOT;
	read();
}
PlayerShoot::PlayerShoot(Packet &p) : Packet(p)
{
	this->type_ = PacketType::PLAYERSHOOT;
	read();
}


Packet *PlayerShoot::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(time);
	this->writeBytes<byte>(bulletId);
	this->writeBytes<short>(containerType);
	startingPos.Write(this);
	this->writeBytes<float>(static_cast<float>(angle));
	// Send the packet
	return this;
}

void PlayerShoot::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	time = this->readBytes<int>();
	bulletId = this->readBytes<byte>();
	containerType = this->readBytes<short>();
	startingPos.Read(this);
	angle = static_cast<double>(this->readBytes<float>());
	// done!
}