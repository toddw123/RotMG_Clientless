#include "Move.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
Move::Move()
{
	// Set packet id
	this->id = PacketType::MOVE;
}
Move::Move(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::MOVE;
	Parse();
}
Move::Move(Packet &p) : Packet(p)
{
	this->id = PacketType::MOVE;
	Parse();
}

void Move::Send()
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
	PacketIOHelper::SendPacket(this);
}

void Move::Parse()
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

void Move::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}