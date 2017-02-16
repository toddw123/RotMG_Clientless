#include "Notification.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
Notification::Notification()
{
	// Set packet id
	this->id = PacketType::NOTIFICATION;
}
Notification::Notification(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::NOTIFICATION;
	Parse();
}
Notification::Notification(const Packet &p) : Packet(p)
{
	this->id = PacketType::NOTIFICATION;
	Parse();
}

void Notification::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(objectId);
	this->writeString<short>(message);
	this->writeBytes<int>(color);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void Notification::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	objectId = this->readBytes<int>();
	message = this->readString<short>();
	color = this->readBytes<int>();
	// done!
}

void Notification::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}