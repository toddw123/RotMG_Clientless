#include "AllyShoot.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
AllyShoot::AllyShoot()
{
	// Set packet id
	this->id = PacketType::ALLYSHOOT;
}
AllyShoot::AllyShoot(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::ALLYSHOOT;
	Parse();
}
AllyShoot::AllyShoot(const Packet &p) : Packet(p)
{
	this->id = PacketType::ALLYSHOOT;
	Parse();
}


void AllyShoot::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<byte>(bulletId);
	this->writeBytes<int>(ownerId);
	this->writeBytes<short>(containerType);
	this->writeBytes<float>(angle);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void AllyShoot::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	bulletId = this->readBytes<byte>();
	ownerId = this->readBytes<int>();
	containerType = this->readBytes<short>();
	angle = this->readFloat();
	// done!
}

void AllyShoot::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}