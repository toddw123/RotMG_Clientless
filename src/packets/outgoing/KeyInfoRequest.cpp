#include "KeyInfoRequest.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

KeyInfoRequest::KeyInfoRequest()
{
	this->id = PacketType::KEY_INFO_REQUEST;
}
KeyInfoRequest::KeyInfoRequest(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::KEY_INFO_REQUEST;
	Parse();
}
KeyInfoRequest::KeyInfoRequest(Packet &p) : Packet(p)
{
	this->id = PacketType::KEY_INFO_REQUEST;
	Parse();
}

void KeyInfoRequest::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Now write the data
	this->writeBytes<int>(itemType);
	
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void KeyInfoRequest::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	itemType = this->readBytes<int>();
	// done!
}

void KeyInfoRequest::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}