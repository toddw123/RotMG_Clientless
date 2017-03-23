#include "OtherHit.h"



// Constructor
OtherHit::OtherHit()
{
	this->type_ = PacketType::OTHERHIT;
}
OtherHit::OtherHit(byte *b, int i) : Packet(b, i)
{
	this->type_ = PacketType::OTHERHIT;
	read();
}
OtherHit::OtherHit(Packet &p) : Packet(p)
{
	this->type_ = PacketType::OTHERHIT;
	read();
}

Packet *OtherHit::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(time);
	this->writeBytes<byte>(bulletId);
	this->writeBytes<int>(objectId);
	this->writeBytes<int>(targetId);
	// Send the packet
	return this;
}

void OtherHit::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	time = this->readBytes<int>();
	bulletId = this->readBytes<byte>();
	objectId = this->readBytes<int>();
	targetId = this->readBytes<int>();
	// done!
}