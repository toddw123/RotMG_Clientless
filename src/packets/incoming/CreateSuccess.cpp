#include "CreateSuccess.h"


// Constructors
CreateSuccess::CreateSuccess()
{
	// Set packet id
	this->type_ = PacketType::CREATE_SUCCESS;
}
CreateSuccess::CreateSuccess(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type_ = PacketType::CREATE_SUCCESS;
	read();
}
CreateSuccess::CreateSuccess(const Packet &p) : Packet(p)
{
	this->type_ = PacketType::CREATE_SUCCESS;
	read();
}


Packet *CreateSuccess::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(objectId);
	this->writeBytes<int>(charId);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void CreateSuccess::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	objectId = this->readBytes<int>();
	charId = this->readBytes<int>();
	// done!
}