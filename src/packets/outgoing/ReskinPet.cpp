#include "ReskinPet.h"
#include "../PacketType.h"


// Constructor
ReskinPet::ReskinPet()
{
	this->id = PacketType::PET_CHANGE_FORM_MSG;
}
ReskinPet::ReskinPet(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::PET_CHANGE_FORM_MSG;
	read();
}
ReskinPet::ReskinPet(Packet &p) : Packet(p)
{
	this->id = PacketType::PET_CHANGE_FORM_MSG;
	read();
}

Packet *ReskinPet::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(petInstanceId);
	this->writeBytes<int>(pickedNewPetType);
	item.Write(this);
	// Send the packet
	return this;
}

void ReskinPet::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	petInstanceId = this->readBytes<int>();
	pickedNewPetType = this->readBytes<int>();
	item.Read(this);
	// done!
}