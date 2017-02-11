#include "UsePortal.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
UsePortal::UsePortal()
{
	// Set packet id
	this->id = PacketType::USEPORTAL;
}
UsePortal::UsePortal(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::USEPORTAL;
	Parse();
}
UsePortal::UsePortal(const Packet &p) : Packet(p)
{
	this->id = PacketType::USEPORTAL;
	Parse();
}

void UsePortal::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(objectId);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void UsePortal::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	objectId = this->readBytes<int>();
	// done!
}

void UsePortal::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}