#include "VerifyEmail.h"
#include "../PacketIOHelper.h"
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
	Parse();
}
VerifyEmail::VerifyEmail(const Packet &p) : Packet(p)
{
	this->id = PacketType::VERIFY_EMAIL;
	Parse();
}

void VerifyEmail::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	// nothing to write
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void VerifyEmail::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	// nothing to read
	// done!
}

void VerifyEmail::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}