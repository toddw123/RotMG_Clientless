#include "Teleport.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
Teleport::Teleport()
{
	// Set packet id
	this->id = PacketType::TELEPORT;
}
Teleport::Teleport(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::TELEPORT;
	Parse();
}
Teleport::Teleport(Packet &p) : Packet(p)
{
	this->id = PacketType::TELEPORT;
	Parse();
}

void Teleport::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(objectId);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void Teleport::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	objectId = this->readBytes<int>();
	// done!
}

void Teleport::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}