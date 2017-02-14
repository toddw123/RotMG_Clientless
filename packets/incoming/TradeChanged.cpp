#include "TradeChanged.h"
#include "../PacketIOHelper.h"
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
	Parse();
}
TradeChanged::TradeChanged(const Packet &p) : Packet(p)
{
	this->id = PacketType::TRADECHANGED;
	Parse();
}

void TradeChanged::Send()
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

void TradeChanged::Parse()
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

void TradeChanged::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}