#include "ChangeTrade.h"
#include "../PacketType.h"


// Constructor
ChangeTrade::ChangeTrade()
{
	this->id = PacketType::CHANGETRADE;
}
ChangeTrade::ChangeTrade(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::CHANGETRADE;
	read();
}
ChangeTrade::ChangeTrade(Packet &p) : Packet(p)
{
	this->id = PacketType::CHANGETRADE;
	read();
}

Packet *ChangeTrade::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<short>((short)offer.size());
	if ((short)offer.size() > 0)
	{
		for (short i = 0; i < (short)offer.size(); i++)
		{
			this->writeBytes<bool>(offer.at(i));
		}
	}
	// Send the packet
	return this;
}

void ChangeTrade::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// clear vector
	offer.clear();
	// Read in the data
	short count = this->readBytes<short>();
	if (count > 0)
	{
		for (short i = 0; i < count; i++)
		{
			offer.push_back(this->readBool());
		}
	}
	// done!
}