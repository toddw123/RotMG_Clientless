#include "Move.h"


// Constructors
Move::Move()
{
	// Set packet id
	this->type = PacketType::MOVE;
}
Move::Move(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type = PacketType::MOVE;
	read();
}
Move::Move(Packet &p) : Packet(p)
{
	this->type = PacketType::MOVE;
	read();
}

Packet *Move::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(tickId);
	this->writeBytes<int>(time);
	newPosition.Write(this);
	this->writeBytes<short>((short)records.size());
	if ((short)records.size() > 0)
	{
		for (short i = 0; i < (short)records.size(); i++)
		{
			records.at(i).Write(this);
		}
	}
	// Send the packet
	return this;
}

void Move::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	tickId = this->readBytes<int>();
	time = this->readBytes<int>();
	newPosition.Read(this);

	records.clear();
	short count = this->readBytes<short>();
	if (count > 0)
	{
		for (short i = 0; i < count; i++)
		{
			records.push_back(MoveRecord(this));
		}
	}

	// done!
}