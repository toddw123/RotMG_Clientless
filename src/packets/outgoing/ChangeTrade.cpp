#include "ChangeTrade.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"


// Constructor
ChangeTrade::ChangeTrade()
{
	this->id = PacketType::CHANGETRADE;
}
ChangeTrade::ChangeTrade(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::CHANGETRADE;
	Parse();
}
ChangeTrade::ChangeTrade(Packet &p) : Packet(p)
{
	this->id = PacketType::CHANGETRADE;
	Parse();
}

void ChangeTrade::Send()
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
	PacketIOHelper::SendPacket(this);
}

void ChangeTrade::Parse()
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
			offer.push_back(this->readBytes<bool>());
		}
	}
	// done!
}

void ChangeTrade::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}