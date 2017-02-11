#include "CreateSuccess.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
CreateSuccess::CreateSuccess()
{
	// Set packet id
	this->id = PacketType::CREATE_SUCCESS;
}
CreateSuccess::CreateSuccess(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::CREATE_SUCCESS;
	Parse();
}
CreateSuccess::CreateSuccess(const Packet &p) : Packet(p)
{
	this->id = PacketType::CREATE_SUCCESS;
	Parse();
}


void CreateSuccess::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(objectId);
	this->writeBytes<int>(charId);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void CreateSuccess::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	objectId = this->readBytes<int>();
	charId = this->readBytes<int>();
	// done!
}

void CreateSuccess::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}