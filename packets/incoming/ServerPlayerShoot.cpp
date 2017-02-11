#include "ServerPlayerShoot.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
ServerPlayerShoot::ServerPlayerShoot()
{
	// Set packet id
	this->id = PacketType::SERVERPLAYERSHOOT;
}
ServerPlayerShoot::ServerPlayerShoot(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::SERVERPLAYERSHOOT;
	Parse();
}
ServerPlayerShoot::ServerPlayerShoot(const Packet &p) : Packet(p)
{
	this->id = PacketType::SERVERPLAYERSHOOT;
	Parse();
}


void ServerPlayerShoot::Send()
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
	PacketIOHelper::SendPacket(this);
}

void ServerPlayerShoot::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	bulletId = this->readBytes<byte>();
	ownerId = this->readBytes<int>();
	containerType = this->readBytes<int>();
	startingPos.Read(this);
	angle = this->readFloat();
	damage = this->readBytes<short>();
	// done!
}

void ServerPlayerShoot::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}