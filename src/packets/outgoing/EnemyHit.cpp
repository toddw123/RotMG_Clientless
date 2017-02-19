#include "EnemyHit.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"


// Constructor
EnemyHit::EnemyHit()
{
	this->id = PacketType::ENEMYHIT;
}
EnemyHit::EnemyHit(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::ENEMYHIT;
	Parse();
}
EnemyHit::EnemyHit(Packet &p) : Packet(p)
{
	this->id = PacketType::ENEMYHIT;
	Parse();
}

void EnemyHit::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(time);
	this->writeBytes<byte>(bulletId);
	this->writeBytes<int>(targetId);
	this->writeBytes<bool>(kill);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void EnemyHit::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	time = this->readBytes<int>();
	bulletId = this->readBytes<byte>();
	targetId = this->readBytes<int>();
	kill = this->readBytes<bool>();
	// done!
}

void EnemyHit::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}