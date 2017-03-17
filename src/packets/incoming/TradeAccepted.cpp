#include "TradeAccepted.h"


// Constructors
TradeAccepted::TradeAccepted()
{
	// Set packet id
	this->type = PacketType::TRADEACCEPTED;
}
TradeAccepted::TradeAccepted(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type = PacketType::TRADEACCEPTED;
	read();
}
TradeAccepted::TradeAccepted(const Packet &p) : Packet(p)
{
	this->type = PacketType::TRADEACCEPTED;
	read();
}

Packet *TradeAccepted::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<short>((short)myOffer.size());
	if ((short)myOffer.size() > 0)
	{
		for (short i = 0; i < (short)myOffer.size(); i++)
		{
			this->writeBytes<bool>(myOffer.at(i));
		}
	}

	this->writeBytes<short>((short)yourOffer.size());
	if ((short)yourOffer.size() > 0)
	{
		for (short i = 0; i < (short)yourOffer.size(); i++)
		{
			this->writeBytes<bool>(yourOffer.at(i));
		}
	}
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void TradeAccepted::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Clear out any existing data
	myOffer.clear();
	yourOffer.clear();
	// Read in the data
	short count = this->readBytes<short>();
	if (count > 0)
	{
		myOffer.push_back(this->readBool());
	}

	count = this->readBytes<short>();
	if (count > 0)
	{
		yourOffer.push_back(this->readBool());
	}
	// done!
}