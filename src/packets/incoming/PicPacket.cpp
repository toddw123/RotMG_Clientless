#include "PicPacket.h"


// Constructors
PicPacket::PicPacket()
{
	// Set packet id
	this->_type = PacketType::PIC;
}
PicPacket::PicPacket(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->_type = PacketType::PIC;
	read();
}
PicPacket::PicPacket(const Packet &p) : Packet(p)
{
	this->_type = PacketType::PIC;
	read();
}

Packet *PicPacket::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(width);
	this->writeBytes<int>(height);
	for (int i = 0; i < (width * height * 4); i++)
	{
		this->writeBytes<byte>(bitmapData.at(i));
	}
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void PicPacket::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Clear the bitmapData vector
	bitmapData.clear();
	// Read in the data
	width = this->readBytes<int>();
	height = this->readBytes<int>();
	// This might be slow, i might need to make a new function to read a block of data
	for (int i = 0; i < (width * height * 4); i++)
	{
		bitmapData.push_back(this->readBytes<byte>());
	}
	// done!
}