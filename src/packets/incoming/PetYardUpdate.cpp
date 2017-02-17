#include "PetYardUpdate.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
PetYardUpdate::PetYardUpdate()
{
	// Set packet id
	this->id = PacketType::PETYARDUPDATE;
}
PetYardUpdate::PetYardUpdate(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::PETYARDUPDATE;
	Parse();
}
PetYardUpdate::PetYardUpdate(const Packet &p) : Packet(p)
{
	this->id = PacketType::PETYARDUPDATE;
	Parse();
}

void PetYardUpdate::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(type);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void PetYardUpdate::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	type = this->readBytes<int>();
	// done!
}

void PetYardUpdate::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}