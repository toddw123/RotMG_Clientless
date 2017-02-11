#include "EvolvedPetMessage.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
EvolvedPetMessage::EvolvedPetMessage()
{
	// Set packet id
	this->id = PacketType::EVOLVE_PET;
}
EvolvedPetMessage::EvolvedPetMessage(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::EVOLVE_PET;
	Parse();
}
EvolvedPetMessage::EvolvedPetMessage(const Packet &p) : Packet(p)
{
	this->id = PacketType::EVOLVE_PET;
	Parse();
}

void EvolvedPetMessage::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(petId);
	this->writeBytes<int>(initialSkin);
	this->writeBytes<int>(finalSkin);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void EvolvedPetMessage::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	petId = this->readBytes<int>();
	initialSkin = this->readBytes<int>();
	finalSkin = this->readBytes<int>();
	// done!
}

void EvolvedPetMessage::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}