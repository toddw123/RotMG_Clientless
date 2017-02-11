#include "ShootAck.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
ShootAck::ShootAck()
{
	// Set packet id
	this->id = PacketType::SHOOTACK;
}
ShootAck::ShootAck(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::SHOOTACK;
	Parse();
}
ShootAck::ShootAck(Packet &p) : Packet(p)
{
	this->id = PacketType::SHOOTACK;
	Parse();
}

void ShootAck::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(time);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void ShootAck::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	time = this->readBytes<int>();
	// done!
}

void ShootAck::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}