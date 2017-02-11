#include "Death.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
Death::Death()
{
	// Set packet id
	this->id = PacketType::DEATH;
}
Death::Death(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::DEATH;
	Parse();
}
Death::Death(const Packet &p) : Packet(p)
{
	this->id = PacketType::DEATH;
	Parse();
}

void Death::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(accountId);
	this->writeBytes<int>(charId);
	this->writeString<short>(killedBy);
	this->writeBytes<int>(zombieId);
	this->writeBytes<int>(zombieType);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void Death::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	accountId = this->readString<short>();
	charId = this->readBytes<int>();
	killedBy = this->readString<short>();
	zombieId = this->readBytes<int>();
	zombieType = this->readBytes<int>();
	isZombie = (zombieId != 1);
	// done!
}

void Death::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}