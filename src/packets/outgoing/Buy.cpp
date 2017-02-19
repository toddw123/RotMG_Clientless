#include "Buy.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"


// Constructor
Buy::Buy()
{
	this->id = PacketType::BUY;
}
Buy::Buy(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::BUY;
	Parse();
}
Buy::Buy(Packet &p) : Packet(p)
{
	this->id = PacketType::BUY;
	Parse();
}

void Buy::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(objectId);
	this->writeBytes<int>(quantity);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void Buy::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	objectId = this->readBytes<int>();
	quantity = this->readBytes<int>();
	// done!
}

void Buy::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}