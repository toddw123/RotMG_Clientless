#include "TradeStart.h"


// Constructors
TradeStart::TradeStart()
{
	// Set packet id
	this->type_ = PacketType::TRADESTART;
}
TradeStart::TradeStart(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type_ = PacketType::TRADESTART;
	read();
}
TradeStart::TradeStart(const Packet &p) : Packet(p)
{
	this->type_ = PacketType::TRADESTART;
	read();
}

Packet *TradeStart::write()
{
	short i = 0;
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<short>((short)myItems.size());
	if ((short)myItems.size() > 0)
	{
		for (i = 0; i < (short)myItems.size(); i++)
		{
			myItems.at(i).Write(this);
		}
	}
	this->writeString<short>(yourName);
	this->writeBytes<short>((short)yourItems.size());
	if ((short)yourItems.size() > 0)
	{
		for (i = 0; i < (short)yourItems.size(); i++)
		{
			yourItems.at(i).Write(this);
		}
	}
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void TradeStart::read()
{
	short i = 0;
	// Make sure the index is set to 0
	this->setIndex(0);
	// clear data
	myItems.clear();
	yourItems.clear();
	// Read in the data
	short count = this->readBytes<short>();
	if (count > 0)
	{
		for (i = 0; i < count; i++)
		{
			myItems.push_back(TradeItem(this));
		}
	}
	yourName = this->readString<short>();
	count = this->readBytes<short>();
	if (count > 0)
	{
		for (i = 0; i < count; i++)
		{
			yourItems.push_back(TradeItem(this));
		}
	}
	// done!
}