#include "NewTick.h"


// Constructor
NewTick::NewTick()
{
	this->type = PacketType::NEWTICK;
}
NewTick::NewTick(byte *b, int i) : Packet(b, i)
{
	this->type = PacketType::NEWTICK;
	read();
}
NewTick::NewTick(const Packet &p) : Packet(p)
{
	this->type = PacketType::NEWTICK;
	read();
}

Packet *NewTick::write()
{
	// Clear the packet data just to be safe
	this->clearData();

	short i = 0; // used in for loops

	this->writeBytes<int>(tickId);
	this->writeBytes<int>(tickTime);

	this->writeBytes<short>((short)statuses.size());
	if (statuses.size() > 0)
	{
		for (short i = 0; i < (short)statuses.size(); i++)
		{
			statuses.at(i).Write(this);
		}
	}

	//PacketIOHelper::SendPacket(this);
	return this;
}

void NewTick::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);

	short i = 0; // used in for loops

	statuses.clear();

	tickId = this->readBytes<int>();
	tickTime = this->readBytes<int>();

	short count = this->readBytes<short>();
	if (count > 0)
	{
		for (i = 0; i < count; i++)
		{
			statuses.push_back(ObjectStatusData(this));
		}
	}
	
	// done!
}