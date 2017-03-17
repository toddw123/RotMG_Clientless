#include "Ping.h"


// Constructors
Ping::Ping()
{
	// Set packet id
	this->_type = PacketType::PING;
}
Ping::Ping(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->_type = PacketType::PING;
	read();
}
Ping::Ping(const Packet &p) : Packet(p)
{
	this->_type = PacketType::PING;
	read();
}

Packet *Ping::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(serial);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void Ping::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	serial = this->readBytes<int>();
	// done!
}