#include "EvolvedPetMessage.h"


// Constructors
EvolvedPetMessage::EvolvedPetMessage()
{
	// Set packet id
	this->type_ = PacketType::EVOLVE_PET;
}
EvolvedPetMessage::EvolvedPetMessage(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type_ = PacketType::EVOLVE_PET;
	read();
}
EvolvedPetMessage::EvolvedPetMessage(const Packet &p) : Packet(p)
{
	this->type_ = PacketType::EVOLVE_PET;
	read();
}

Packet *EvolvedPetMessage::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(petId);
	this->writeBytes<int>(initialSkin);
	this->writeBytes<int>(finalSkin);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void EvolvedPetMessage::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	petId = this->readBytes<int>();
	initialSkin = this->readBytes<int>();
	finalSkin = this->readBytes<int>();
	// done!
}