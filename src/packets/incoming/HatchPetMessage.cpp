#include "HatchPetMessage.h"


// Constructors
HatchPetMessage::HatchPetMessage()
{
	// Set packet id
	this->type = PacketType::HATCH_PET;
}
HatchPetMessage::HatchPetMessage(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type = PacketType::HATCH_PET;
	read();
}
HatchPetMessage::HatchPetMessage(const Packet &p) : Packet(p)
{
	this->type = PacketType::HATCH_PET;
	read();
}

Packet *HatchPetMessage::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(petName);
	this->writeBytes<int>(petSkin);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void HatchPetMessage::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	petName = this->readString<short>();
	petSkin = this->readBytes<int>();
	// done!
}