#include "Goto.h"


// Constructors
Goto::Goto()
{
	// Set packet id
	this->_type = PacketType::GOTO;
}
Goto::Goto(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->_type = PacketType::GOTO;
	read();
}
Goto::Goto(const Packet &p) : Packet(p)
{
	this->_type = PacketType::GOTO;
	read();
}

Packet *Goto::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(objectId);
	pos.Write(this);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void Goto::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	objectId = this->readBytes<int>();
	pos.Read(this);

	// done!
}