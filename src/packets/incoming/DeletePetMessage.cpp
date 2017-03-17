#include "DeletePetMessage.h"


// Constructors
DeletePetMessage::DeletePetMessage()
{
	// Set packet id
	this->_type = PacketType::DELETE_PET;
}
DeletePetMessage::DeletePetMessage(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->_type = PacketType::DELETE_PET;
	read();
}
DeletePetMessage::DeletePetMessage(const Packet &p) : Packet(p)
{
	this->_type = PacketType::DELETE_PET;
	read();
}

Packet *DeletePetMessage::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(petId);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void DeletePetMessage::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	petId = this->readBytes<int>();
	// done!
}