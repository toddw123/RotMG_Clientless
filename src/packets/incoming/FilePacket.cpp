#include "FilePacket.h"


// Constructors
FilePacket::FilePacket()
{
	// Set packet id
	this->_type = PacketType::FILE_PACKET;
}
FilePacket::FilePacket(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->_type = PacketType::FILE_PACKET;
	read();
}
FilePacket::FilePacket(const Packet &p) : Packet(p)
{
	this->_type = PacketType::FILE_PACKET;
	read();
}

Packet *FilePacket::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(filename);
	this->writeString<int>(file);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void FilePacket::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	filename = this->readString<short>();
	file = this->readString<int>(); // This might not work, needs testings
	// done!
}