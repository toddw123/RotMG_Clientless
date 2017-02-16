#include "GotoAck.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
GotoAck::GotoAck()
{
	// Set packet id
	this->id = PacketType::GOTOACK;
}
GotoAck::GotoAck(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::GOTOACK;
	Parse();
}
GotoAck::GotoAck(Packet &p) : Packet(p)
{
	this->id = PacketType::GOTOACK;
	Parse();
}

void GotoAck::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(time);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void GotoAck::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	time = this->readBytes<int>();
	// done!
}

void GotoAck::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}