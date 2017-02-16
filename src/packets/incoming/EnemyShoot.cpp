#include "EnemyShoot.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
EnemyShoot::EnemyShoot()
{
	// Set packet id
	this->id = PacketType::ENEMYSHOOT;
}
EnemyShoot::EnemyShoot(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::ENEMYSHOOT;
	Parse();
}
EnemyShoot::EnemyShoot(const Packet &p) : Packet(p)
{
	this->id = PacketType::ENEMYSHOOT;
	Parse();
}


void EnemyShoot::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<byte>(bulletId);
	this->writeBytes<int>(ownerId);
	this->writeBytes<byte>(bulletType);
	startingPos.Write(this);
	this->writeBytes<float>(angle);
	this->writeBytes<short>(damage);
	this->writeBytes<byte>(numShots);
	this->writeBytes<float>(angleInc);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void EnemyShoot::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	bulletId = this->readBytes<byte>();
	ownerId = this->readBytes<int>();
	bulletType = this->readBytes<byte>();
	startingPos.Read(this);
	angle = this->readFloat();
	damage = this->readBytes<short>();
	// Check if there is more data to be read or not
	if (this->data.size() > this->index)
	{
		numShots = this->readBytes<byte>();
		angleInc = this->readFloat();
	}
	else
	{
		numShots = 1;
		angleInc = 0.0f;
	}
	// done!
}

void EnemyShoot::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}