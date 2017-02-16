#include "NewAbilityMessage.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
NewAbilityMessage::NewAbilityMessage()
{
	// Set packet id
	this->id = PacketType::NEW_ABILITY;
}
NewAbilityMessage::NewAbilityMessage(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::NEW_ABILITY;
	Parse();
}
NewAbilityMessage::NewAbilityMessage(const Packet &p) : Packet(p)
{
	this->id = PacketType::NEW_ABILITY;
	Parse();
}

void NewAbilityMessage::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(type);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void NewAbilityMessage::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	type = this->readBytes<int>();
	// done!
}

void NewAbilityMessage::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}