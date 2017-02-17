#include "HatchPetMessage.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
HatchPetMessage::HatchPetMessage()
{
	// Set packet id
	this->id = PacketType::HATCH_PET;
}
HatchPetMessage::HatchPetMessage(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::HATCH_PET;
	Parse();
}
HatchPetMessage::HatchPetMessage(const Packet &p) : Packet(p)
{
	this->id = PacketType::HATCH_PET;
	Parse();
}

void HatchPetMessage::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(petName);
	this->writeBytes<int>(petSkin);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void HatchPetMessage::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	petName = this->readString<short>();
	petSkin = this->readBytes<int>();
	// done!
}

void HatchPetMessage::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}