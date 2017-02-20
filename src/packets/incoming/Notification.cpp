#include "Notification.h"
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
	read();
}
Notification::Notification(const Packet &p) : Packet(p)
{
	this->id = PacketType::NOTIFICATION;
	read();
}

Packet *Notification::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(objectId);
	this->writeString<short>(message);
	this->writeBytes<int>(color);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void Notification::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	objectId = this->readBytes<int>();
	message = this->readString<short>();
	color = this->readBytes<int>();
	// done!
}