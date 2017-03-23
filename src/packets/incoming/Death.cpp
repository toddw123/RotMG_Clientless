#include "Death.h"


// Constructors
Death::Death()
{
	// Set packet id
	this->type_ = PacketType::DEATH;
}
Death::Death(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type_ = PacketType::DEATH;
	read();
}
Death::Death(const Packet &p) : Packet(p)
{
	this->type_ = PacketType::DEATH;
	read();
}

Packet *Death::write()
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
	//PacketIOHelper::SendPacket(this);
	return this;
}

void Death::read()
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