#include "DeletePetMessage.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
DeletePetMessage::DeletePetMessage()
{
	// Set packet id
	this->id = PacketType::DELETE_PET;
}
DeletePetMessage::DeletePetMessage(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::DELETE_PET;
	Parse();
}
DeletePetMessage::DeletePetMessage(const Packet &p) : Packet(p)
{
	this->id = PacketType::DELETE_PET;
	Parse();
}

void DeletePetMessage::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(petId);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void DeletePetMessage::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	petId = this->readBytes<int>();
	// done!
}

void DeletePetMessage::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}