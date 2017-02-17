#include "GroundDamage.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
GroundDamage::GroundDamage()
{
	// Set packet id
	this->id = PacketType::GROUNDDAMAGE;
}
GroundDamage::GroundDamage(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::GROUNDDAMAGE;
	Parse();
}
GroundDamage::GroundDamage(Packet &p) : Packet(p)
{
	this->id = PacketType::GROUNDDAMAGE;
	Parse();
}

void GroundDamage::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(time);
	curPosition.Write(this);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void GroundDamage::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	time = this->readBytes<int>();
	curPosition.Read(this);
	// done!
}

void GroundDamage::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}