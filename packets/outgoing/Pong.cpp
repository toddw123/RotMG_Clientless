#include "Pong.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
Pong::Pong()
{
	// Set packet id
	this->id = PacketType::PONG;
}
Pong::Pong(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::PONG;
	Parse();
}
Pong::Pong(Packet &p) : Packet(p)
{
	this->id = PacketType::PONG;
	Parse();
}

void Pong::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(serial);
	this->writeBytes<int>(time);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void Pong::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	serial = this->readBytes<int>();
	time = this->readBytes<int>();
	// done!
}

void Pong::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}