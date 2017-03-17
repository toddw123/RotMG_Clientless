#include "KeyInfoResponse.h"


// Constructors
KeyInfoResponse::KeyInfoResponse()
{
	// Set packet id
	this->type = PacketType::KEY_INFO_RESPONSE;
}
KeyInfoResponse::KeyInfoResponse(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type = PacketType::KEY_INFO_RESPONSE;
	read();
}
KeyInfoResponse::KeyInfoResponse(const Packet &p) : Packet(p)
{
	this->type = PacketType::KEY_INFO_RESPONSE;
	read();
}

Packet *KeyInfoResponse::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(name);
	this->writeString<short>(description);
	this->writeString<short>(creator);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void KeyInfoResponse::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	name = this->readString<short>();
	description = this->readString<short>();
	creator = this->readString<short>();
}