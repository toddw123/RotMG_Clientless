#include "ActivePetUpdateRequest.h"



// Constructor
ActivePetUpdateRequest::ActivePetUpdateRequest()
{
	this->type_ = PacketType::ACTIVE_PET_UPDATE_REQUEST;
}
ActivePetUpdateRequest::ActivePetUpdateRequest(byte *b, int i) : Packet(b, i)
{
	this->type_ = PacketType::ACTIVE_PET_UPDATE_REQUEST;
	read();
}
ActivePetUpdateRequest::ActivePetUpdateRequest(Packet &p) : Packet(p)
{
	this->type_ = PacketType::ACTIVE_PET_UPDATE_REQUEST;
	read();
}

Packet *ActivePetUpdateRequest::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<byte>(commandType);
	this->writeBytes<int>(instanceId);
	// Send the packet
	return this;
}

void ActivePetUpdateRequest::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	commandType = this->readBytes<byte>();
	instanceId = this->readBytes<int>();
	// done!
}