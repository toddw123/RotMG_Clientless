#include "FilePacket.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
FilePacket::FilePacket()
{
	// Set packet id
	this->id = PacketType::FILE_PACKET;
}
FilePacket::FilePacket(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::FILE_PACKET;
	Parse();
}
FilePacket::FilePacket(const Packet &p) : Packet(p)
{
	this->id = PacketType::FILE_PACKET;
	Parse();
}

void FilePacket::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(filename);
	this->writeString<int>(file);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void FilePacket::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	filename = this->readString<short>();
	file = this->readString<int>(); // This might not work, needs testings
	// done!
}

void FilePacket::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}