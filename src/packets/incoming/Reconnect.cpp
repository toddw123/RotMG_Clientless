#include "Reconnect.h"
#include "../PacketType.h"

// Constructors
Reconnect::Reconnect()
{
	this->id = PacketType::RECONNECT;
}
Reconnect::Reconnect(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::RECONNECT;
	read();
}
Reconnect::Reconnect(const Packet &p) : Packet(p)
{
	this->id = PacketType::RECONNECT;
	read();
}

Packet *Reconnect::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write the data from Reconnect to the packet
	this->writeString<short>(name);
	this->writeString<short>(host);
	this->writeBytes<int>(port);
	this->writeBytes<int>(gameId);
	this->writeBytes<int>(keyTime);
	this->writeBytes<bool>(isFromArena);
	this->writeBytes<short>((short)keys.size());
	if ((short)keys.size() > 0)
	{
		for (short i = 0; i < (short)keys.size(); i++)
		{
			this->writeBytes<byte>(keys.at(i));
		}
	}

	//PacketIOHelper::SendPacket(this);
	return this;
}

void Reconnect::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);

	name = this->readString<short>();
	host = this->readString<short>();
	port = this->readBytes<int>();
	gameId = this->readBytes<int>();
	keyTime = this->readBytes<int>();
	isFromArena = this->readBool();

	short count = this->readBytes<short>();
	keys.clear();
	if (count > 0)
	{
		for (short i = 0; i < count; i++)
		{
			keys.push_back(this->readBytes<byte>());
		}
	}
	// done!
}