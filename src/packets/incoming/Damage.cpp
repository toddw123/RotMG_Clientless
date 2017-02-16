#include "Damage.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"


// Constructor
Damage::Damage()
{
	this->id = PacketType::DAMAGE;
}
Damage::Damage(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::DAMAGE;
	Parse();
}
Damage::Damage(const Packet &p) : Packet(p)
{
	this->id = PacketType::DAMAGE;
	Parse();
}

void Damage::Send()
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
	PacketIOHelper::SendPacket(this);
}

void Damage::Parse()
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

void Damage::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}