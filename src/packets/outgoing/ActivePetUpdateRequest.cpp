#include "ActivePetUpdateRequest.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"


// Constructor
ActivePetUpdateRequest::ActivePetUpdateRequest()
{
	this->id = PacketType::ACTIVE_PET_UPDATE_REQUEST;
}
ActivePetUpdateRequest::ActivePetUpdateRequest(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::ACTIVE_PET_UPDATE_REQUEST;
	Parse();
}
ActivePetUpdateRequest::ActivePetUpdateRequest(Packet &p) : Packet(p)
{
	this->id = PacketType::ACTIVE_PET_UPDATE_REQUEST;
	Parse();
}

void ActivePetUpdateRequest::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<byte>(commandType);
	this->writeBytes<int>(instanceId);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void ActivePetUpdateRequest::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	commandType = this->readBytes<byte>();
	instanceId = this->readBytes<int>();
	// done!
}

void ActivePetUpdateRequest::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}