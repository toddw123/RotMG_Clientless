#include "KeyInfoRequest.h"
#include "../PacketType.h"

KeyInfoRequest::KeyInfoRequest()
{
	this->id = PacketType::KEY_INFO_REQUEST;
}
KeyInfoRequest::KeyInfoRequest(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::KEY_INFO_REQUEST;
	read();
}
KeyInfoRequest::KeyInfoRequest(Packet &p) : Packet(p)
{
	this->id = PacketType::KEY_INFO_REQUEST;
	read();
}

Packet *KeyInfoRequest::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Now write the data
	this->writeBytes<int>(itemType);
	
	// Send the packet
	return this;
}

void KeyInfoRequest::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	itemType = this->readBytes<int>();
	// done!
}