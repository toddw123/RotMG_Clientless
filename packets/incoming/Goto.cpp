#include "Goto.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
Goto::Goto()
{
	// Set packet id
	this->id = PacketType::GOTO;
}
Goto::Goto(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::GOTO;
	Parse();
}
Goto::Goto(const Packet &p) : Packet(p)
{
	this->id = PacketType::GOTO;
	Parse();
}

void Goto::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(objectId);
	pos.Write(this);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void Goto::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	objectId = this->readBytes<int>();
	pos.Read(this);

	// done!
}

void Goto::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}