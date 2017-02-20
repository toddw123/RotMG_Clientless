#include "VerifyEmail.h"
#include "../PacketType.h"

// Constructors
VerifyEmail::VerifyEmail()
{
	// Set packet id
	this->id = PacketType::VERIFY_EMAIL;
}
VerifyEmail::VerifyEmail(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::VERIFY_EMAIL;
	read();
}
VerifyEmail::VerifyEmail(const Packet &p) : Packet(p)
{
	this->id = PacketType::VERIFY_EMAIL;
	read();
}

Packet *VerifyEmail::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	// nothing to write
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void VerifyEmail::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	// nothing to read
	// done!
}