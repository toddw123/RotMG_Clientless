#include "ClientStat.h"


// Constructors
ClientStat::ClientStat()
{
	// Set packet id
	this->type = PacketType::CLIENTSTAT;
}
ClientStat::ClientStat(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type = PacketType::CLIENTSTAT;
	read();
}
ClientStat::ClientStat(const Packet &p) : Packet(p)
{
	this->type = PacketType::CLIENTSTAT;
	read();
}

Packet *ClientStat::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(name);
	this->writeBytes<int>(value);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void ClientStat::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	name = this->readString<short>();
	value = this->readBytes<int>();
	// done!
}