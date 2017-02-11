#include "AoeAck.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
AoeAck::AoeAck()
{
	// Set packet id
	this->id = PacketType::AOEACK;
}
AoeAck::AoeAck(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::AOEACK;
	Parse();
}
AoeAck::AoeAck(Packet &p) : Packet(p)
{
	this->id = PacketType::AOEACK;
	Parse();
}

void AoeAck::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(time);
	position.Write(this);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void AoeAck::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	time = this->readBytes<int>();
	position.Read(this);
	// done!
}

void AoeAck::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}