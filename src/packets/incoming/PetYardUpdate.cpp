#include "PetYardUpdate.h"


// Constructors
PetYardUpdate::PetYardUpdate()
{
	// Set packet id
	this->type = PacketType::PETYARDUPDATE;
}
PetYardUpdate::PetYardUpdate(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type = PacketType::PETYARDUPDATE;
	read();
}
PetYardUpdate::PetYardUpdate(const Packet &p) : Packet(p)
{
	this->type = PacketType::PETYARDUPDATE;
	read();
}

Packet *PetYardUpdate::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(type);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void PetYardUpdate::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	type = this->readBytes<int>();
	// done!
}