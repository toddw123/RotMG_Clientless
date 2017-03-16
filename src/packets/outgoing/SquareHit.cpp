#include "SquareHit.h"



// Constructor
SquareHit::SquareHit()
{
	this->type = PacketType::SQUAREHIT;
}
SquareHit::SquareHit(byte *b, int i) : Packet(b, i)
{
	this->type = PacketType::SQUAREHIT;
	read();
}
SquareHit::SquareHit(Packet &p) : Packet(p)
{
	this->type = PacketType::SQUAREHIT;
	read();
}

Packet *SquareHit::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(time);
	this->writeBytes<byte>(bulletId);
	this->writeBytes<int>(objectId);
	// Send the packet
	return this;
}

void SquareHit::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	time = this->readBytes<int>();
	bulletId = this->readBytes<byte>();
	objectId = this->readBytes<int>();
	// done!
}