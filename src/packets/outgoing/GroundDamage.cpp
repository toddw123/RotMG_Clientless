#include "GroundDamage.h"
#include "../PacketType.h"

// Constructors
GroundDamage::GroundDamage()
{
	// Set packet id
	this->id = PacketType::GROUNDDAMAGE;
}
GroundDamage::GroundDamage(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::GROUNDDAMAGE;
	read();
}
GroundDamage::GroundDamage(Packet &p) : Packet(p)
{
	this->id = PacketType::GROUNDDAMAGE;
	read();
}

Packet *GroundDamage::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(time);
	position.Write(this);
	// Send the packet
	return this;
}

void GroundDamage::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	time = this->readBytes<int>();
	position.Read(this);
	// done!
}