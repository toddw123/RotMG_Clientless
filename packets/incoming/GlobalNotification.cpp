#include "GlobalNotification.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
GlobalNotification::GlobalNotification()
{
	// Set packet id
	this->id = PacketType::GLOBAL_NOTIFICATION;
}
GlobalNotification::GlobalNotification(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::GLOBAL_NOTIFICATION;
	Parse();
}
GlobalNotification::GlobalNotification(const Packet &p) : Packet(p)
{
	this->id = PacketType::GLOBAL_NOTIFICATION;
	Parse();
}

void GlobalNotification::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(type);
	this->writeString<short>(text);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void GlobalNotification::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	type = this->readBytes<int>();
	text = this->readString<short>();
	// done!
}

void GlobalNotification::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}