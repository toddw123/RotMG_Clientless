#include "NewTick.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructor
NewTick::NewTick()
{
	this->id = PacketType::NEWTICK;
}
NewTick::NewTick(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::NEWTICK;
	Parse();
}
NewTick::NewTick(const Packet &p) : Packet(p)
{
	this->id = PacketType::NEWTICK;
	Parse();
}

void NewTick::Send()
{
	// Clear the packet data just to be safe
	this->clearData();

	short i = 0; // used in for loops

	this->writeBytes<int>(tickId);
	this->writeBytes<int>(tickTime);

	this->writeBytes<short>(statuses.size());
	if (statuses.size() > 0)
	{
		for (short i = 0; i < statuses.size(); i++)
		{
			statuses.at(i).Write(this);
		}
	}

	PacketIOHelper::SendPacket(this);
}

void NewTick::Parse()
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

void NewTick::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}