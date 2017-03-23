#include "ServerPlayerShoot.h"


// Constructors
ServerPlayerShoot::ServerPlayerShoot()
{
	// Set packet id
	this->type_ = PacketType::SERVERPLAYERSHOOT;
}
ServerPlayerShoot::ServerPlayerShoot(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type_ = PacketType::SERVERPLAYERSHOOT;
	read();
}
ServerPlayerShoot::ServerPlayerShoot(const Packet &p) : Packet(p)
{
	this->type_ = PacketType::SERVERPLAYERSHOOT;
	read();
}


Packet *ServerPlayerShoot::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<byte>(bulletId);
	this->writeBytes<int>(ownerId);
	this->writeBytes<int>(containerType);
	startingPos.Write(this);
	this->writeBytes<float>(angle);
	this->writeBytes<short>(damage);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void ServerPlayerShoot::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	bulletId = this->readBytes<byte>();
	ownerId = this->readBytes<int>();
	containerType = this->readBytes<int>();
	startingPos.Read(this);
	angle = this->readBytes<float>();
	damage = this->readBytes<short>();
	// done!
}