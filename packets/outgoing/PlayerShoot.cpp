#include "PlayerShoot.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
PlayerShoot::PlayerShoot()
{
	// Set packet id
	this->id = PacketType::PLAYERSHOOT;
}
PlayerShoot::PlayerShoot(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::PLAYERSHOOT;
	Parse();
}
PlayerShoot::PlayerShoot(Packet &p) : Packet(p)
{
	this->id = PacketType::PLAYERSHOOT;
	Parse();
}


void PlayerShoot::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(time);
	this->writeBytes<byte>(bulletId);
	this->writeBytes<short>(containerType);
	startingPos.Write(this);
	this->writeBytes<float>(angle);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void PlayerShoot::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	time = this->readBytes<int>();
	bulletId = this->readBytes<byte>();
	containerType = this->readBytes<short>();
	startingPos.Read(this);
	angle = this->readFloat();
	// done!
}

void PlayerShoot::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}