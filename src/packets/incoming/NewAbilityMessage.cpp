#include "NewAbilityMessage.h"


// Constructors
NewAbilityMessage::NewAbilityMessage()
{
	// Set packet id
	this->_type = PacketType::NEW_ABILITY;
}
NewAbilityMessage::NewAbilityMessage(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->_type = PacketType::NEW_ABILITY;
	read();
}
NewAbilityMessage::NewAbilityMessage(const Packet &p) : Packet(p)
{
	this->_type = PacketType::NEW_ABILITY;
	read();
}

Packet *NewAbilityMessage::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(type);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void NewAbilityMessage::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	type = this->readBytes<int>();
	// done!
}