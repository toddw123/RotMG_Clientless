#include "Damage.h"
#include "../PacketType.h"


// Constructor
Damage::Damage()
{
	this->id = PacketType::DAMAGE;
}
Damage::Damage(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::DAMAGE;
	read();
}
Damage::Damage(const Packet &p) : Packet(p)
{
	this->id = PacketType::DAMAGE;
	read();
}

Packet *Damage::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data

	this->writeBytes<int>(targetId);
	this->writeBytes<byte>((byte)effects.size());
	if ((byte)effects.size() > 0)
	{
		for (byte i = 0; i < (byte)effects.size(); i++)
		{
			this->writeBytes<byte>(effects.at(i));
		}
	}
	this->writeBytes<ushort>(damageAmount);
	this->writeBytes<bool>(kill);
	this->writeBytes<byte>(bulletId);
	this->writeBytes<int>(objectId);
	
	// Send the packet
	//acketIOHelper::SendPacket(this);
	return this;
}

void Damage::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	effects.clear();

	// Read in the data
	targetId = this->readBytes<int>();

	byte count = this->readBytes<byte>();
	for (byte i = 0; i < count; i++)
	{
		effects.push_back(this->readBytes<byte>());
	}
	damageAmount = this->readBytes<ushort>();
	kill = this->readBytes<bool>();
	bulletId = this->readBytes<byte>();
	objectId = this->readBytes<int>();

	// done!
}