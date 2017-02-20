#include "TradeChanged.h"
#include "../PacketType.h"

// Constructors
TradeChanged::TradeChanged()
{
	// Set packet id
	this->id = PacketType::TRADECHANGED;
}
TradeChanged::TradeChanged(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::TRADECHANGED;
	read();
}
TradeChanged::TradeChanged(const Packet &p) : Packet(p)
{
	this->id = PacketType::TRADECHANGED;
	read();
}

Packet *TradeChanged::write()
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
	//PacketIOHelper::SendPacket(this);
	return this;
}

void TradeChanged::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Clear out any existing data
	offer.clear();
	// Read in the data
	short count = this->readBytes<short>();
	if (count > 0)
	{
		offer.push_back(this->readBytes<bool>());
	}
	// done!
}