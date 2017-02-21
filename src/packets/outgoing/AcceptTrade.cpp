#include "AcceptTrade.h"
#include "../PacketType.h"


// Constructor
AcceptTrade::AcceptTrade()
{
	this->id = PacketType::ACCEPTTRADE;
}
AcceptTrade::AcceptTrade(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::ACCEPTTRADE;
	read();
}
AcceptTrade::AcceptTrade(Packet &p) : Packet(p)
{
	this->id = PacketType::ACCEPTTRADE;
	read();
}

Packet *AcceptTrade::write()
{
	short i = 0;
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<short>((short)myOffer.size());
	if ((short)myOffer.size() > 0)
	{
		for (i = 0; i < (short)myOffer.size(); i++)
		{
			this->writeBytes<bool>(myOffer.at(i));
		}
	}
	this->writeBytes<short>((short)yourOffer.size());
	if ((short)yourOffer.size() > 0)
	{
		for (i = 0; i < (short)yourOffer.size(); i++)
		{
			this->writeBytes<bool>(yourOffer.at(i));
		}
	}
	// Send the packet
	return this;
}

void AcceptTrade::read()
{
	short i = 0;
	// Make sure the index is set to 0
	this->setIndex(0);
	// clear vectors
	myOffer.clear();
	yourOffer.clear();
	// Read in the data
	short count = this->readBytes<short>();
	if (count > 0)
	{
		for (i = 0; i < count; i++)
		{
			myOffer.push_back(this->readBool());
		}
	}
	count = this->readBytes<short>();
	if (count > 0)
	{
		for (i = 0; i < count; i++)
		{
			yourOffer.push_back(this->readBool());
		}
	}
	// done!
}