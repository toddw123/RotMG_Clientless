#include "GlobalNotification.h"


// Constructors
GlobalNotification::GlobalNotification()
{
	// Set packet id
	this->type_ = PacketType::GLOBAL_NOTIFICATION;
}
GlobalNotification::GlobalNotification(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type_ = PacketType::GLOBAL_NOTIFICATION;
	read();
}
GlobalNotification::GlobalNotification(const Packet &p) : Packet(p)
{
	this->type_ = PacketType::GLOBAL_NOTIFICATION;
	read();
}

Packet *GlobalNotification::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(type);
	this->writeString<short>(text);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void GlobalNotification::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	type = this->readBytes<int>();
	text = this->readString<short>();
	// done!
}