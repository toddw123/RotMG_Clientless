#include "Failure.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
Failure::Failure()
{
	// Set packet id
	this->id = PacketType::FAILURE;
}
Failure::Failure(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::FAILURE;
	Parse();
}
Failure::Failure(const Packet &p) : Packet(p)
{
	this->id = PacketType::FAILURE;
	Parse();
}

void Failure::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(errorId);
	this->writeString<short>(errorDescription);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void Failure::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	errorId = this->readBytes<int>();
	errorDescription = this->readString<short>();
	// done!
}

void Failure::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}