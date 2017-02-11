#include "KeyInfoResponse.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
KeyInfoResponse::KeyInfoResponse()
{
	// Set packet id
	this->id = PacketType::KEY_INFO_RESPONSE;
}
KeyInfoResponse::KeyInfoResponse(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::KEY_INFO_RESPONSE;
	Parse();
}
KeyInfoResponse::KeyInfoResponse(const Packet &p) : Packet(p)
{
	this->id = PacketType::KEY_INFO_RESPONSE;
	Parse();
}

void KeyInfoResponse::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(name);
	this->writeString<short>(description);
	this->writeString<short>(creator);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void KeyInfoResponse::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	name = this->readString<short>();
	description = this->readString<short>();
	creator = this->readString<short>();
}

void KeyInfoResponse::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}