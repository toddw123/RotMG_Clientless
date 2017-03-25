#include "EnemyShoot.h"


// Constructors
EnemyShoot::EnemyShoot()
{
	// Set packet id
	this->type_ = PacketType::ENEMYSHOOT;
}
EnemyShoot::EnemyShoot(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type_ = PacketType::ENEMYSHOOT;
	read();
}
EnemyShoot::EnemyShoot(const Packet &p) : Packet(p)
{
	this->type_ = PacketType::ENEMYSHOOT;
	read();
}


Packet *EnemyShoot::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<byte>(bulletId);
	this->writeBytes<int>(ownerId);
	this->writeBytes<byte>(bulletType);
	startingPos.Write(this);
	this->writeBytes<float>(static_cast<float>(angle));
	this->writeBytes<short>(damage);
	this->writeBytes<byte>(numShots);
	this->writeBytes<float>(static_cast<float>(angleInc));
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void EnemyShoot::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	bulletId = this->readBytes<byte>();
	ownerId = this->readBytes<int>();
	bulletType = this->readBytes<byte>();
	startingPos.Read(this);
	angle = static_cast<double>(this->readBytes<float>());
	damage = this->readBytes<short>();
	// Check if there is more data to be read or not
	if ((int)this->data.size() > this->index)
	{
		numShots = this->readBytes<byte>();
		angleInc = static_cast<double>(this->readBytes<float>());
	}
	else
	{
		numShots = 1;
		angleInc = 0.0f;
	}
	// done!
}