#include "ActivePetUpdate.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
ActivePetUpdate::ActivePetUpdate()
{
	// Set packet id
	this->id = PacketType::ACTIVEPETUPDATE;
}
ActivePetUpdate::ActivePetUpdate(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::ACTIVEPETUPDATE;
	Parse();
}
ActivePetUpdate::ActivePetUpdate(const Packet &p) : Packet(p)
{
	this->id = PacketType::ACTIVEPETUPDATE;
	Parse();
}

void ActivePetUpdate::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(instanceId);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void ActivePetUpdate::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	instanceId = this->readBytes<int>();
	// done!
}

void ActivePetUpdate::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}