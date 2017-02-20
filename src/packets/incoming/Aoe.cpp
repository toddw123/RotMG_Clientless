#include "Aoe.h"
#include "../PacketType.h"

// Constructors
Aoe::Aoe()
{
	// Set packet id
	this->id = PacketType::AOE;
}
Aoe::Aoe(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::AOE;
	read();
}
Aoe::Aoe(const Packet &p) : Packet(p)
{
	this->id = PacketType::AOE;
	read();
}

Packet *Aoe::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	pos.Write(this);
	this->writeBytes<float>(radius);
	this->writeBytes<ushort>(damage);
	this->writeBytes<byte>(effect);
	this->writeBytes<float>(duration);
	this->writeBytes<ushort>(origType);

	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void Aoe::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	pos.Read(this);
	radius = this->readFloat();
	damage = this->readBytes<ushort>();
	effect = this->readBytes<byte>();
	duration = this->readFloat();
	origType = this->readBytes<ushort>();

	// done!
}