#include "EnemyHit.h"



// Constructor
EnemyHit::EnemyHit()
{
	this->type = PacketType::ENEMYHIT;
}
EnemyHit::EnemyHit(byte *b, int i) : Packet(b, i)
{
	this->type = PacketType::ENEMYHIT;
	read();
}
EnemyHit::EnemyHit(Packet &p) : Packet(p)
{
	this->type = PacketType::ENEMYHIT;
	read();
}

Packet *EnemyHit::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(time);
	this->writeBytes<byte>(bulletId);
	this->writeBytes<int>(targetId);
	this->writeBytes<bool>(kill);
	// Send the packet
	return this;
}

void EnemyHit::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	time = this->readBytes<int>();
	bulletId = this->readBytes<byte>();
	targetId = this->readBytes<int>();
	kill = this->readBool();
	// done!
}