#include "ClientStat.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
ClientStat::ClientStat()
{
	// Set packet id
	this->id = PacketType::CLIENTSTAT;
}
ClientStat::ClientStat(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::CLIENTSTAT;
	Parse();
}
ClientStat::ClientStat(const Packet &p) : Packet(p)
{
	this->id = PacketType::CLIENTSTAT;
	Parse();
}

void ClientStat::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(name);
	this->writeBytes<int>(value);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void ClientStat::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	name = this->readString<short>();
	value = this->readBytes<int>();
	// done!
}

void ClientStat::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}