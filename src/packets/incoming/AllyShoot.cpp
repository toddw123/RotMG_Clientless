#include "AllyShoot.h"


// Constructors
AllyShoot::AllyShoot()
{
	// Set packet id
	this->type_ = PacketType::ALLYSHOOT;
}
AllyShoot::AllyShoot(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type_ = PacketType::ALLYSHOOT;
	read();
}
AllyShoot::AllyShoot(const Packet &p) : Packet(p)
{
	this->type_ = PacketType::ALLYSHOOT;
	read();
}


Packet *AllyShoot::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<byte>(bulletId);
	this->writeBytes<int>(ownerId);
	this->writeBytes<short>(containerType);
	this->writeBytes<float>(static_cast<float>(angle));
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void AllyShoot::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	bulletId = this->readBytes<byte>();
	ownerId = this->readBytes<int>();
	containerType = this->readBytes<short>();
	angle = static_cast<double>(this->readBytes<float>());
	// done!
}