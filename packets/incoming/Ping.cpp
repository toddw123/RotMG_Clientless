#include "Ping.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
Ping::Ping()
{
	// Set packet id
	this->id = PacketType::PING;
}
Ping::Ping(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::PING;
	Parse();
}
Ping::Ping(const Packet &p) : Packet(p)
{
	this->id = PacketType::PING;
	Parse();
}

void Ping::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(serial);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void Ping::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	serial = this->readBytes<int>();
	// done!
}

void Ping::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}