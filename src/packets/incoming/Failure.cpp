#include "Failure.h"


// Constructors
Failure::Failure()
{
	// Set packet id
	this->_type = PacketType::FAILURE;
}
Failure::Failure(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->_type = PacketType::FAILURE;
	read();
}
Failure::Failure(const Packet &p) : Packet(p)
{
	this->_type = PacketType::FAILURE;
	read();
}

Packet *Failure::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(errorId);
	this->writeString<short>(errorDescription);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void Failure::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	errorId = this->readBytes<int>();
	errorDescription = this->readString<short>();
	// done!
}