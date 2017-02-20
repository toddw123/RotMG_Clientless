#include "PlaySoundPacket.h"
#include "../PacketType.h"

// Constructors
PlaySoundPacket::PlaySoundPacket()
{
	// Set packet id
	this->id = PacketType::PLAYSOUND;
}
PlaySoundPacket::PlaySoundPacket(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::PLAYSOUND;
	read();
}
PlaySoundPacket::PlaySoundPacket(const Packet &p) : Packet(p)
{
	this->id = PacketType::PLAYSOUND;
	read();
}

Packet *PlaySoundPacket::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(ownerId);
	this->writeBytes<byte>(soundId);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void PlaySoundPacket::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	ownerId = this->readBytes<int>();
	soundId = this->readBytes<byte>();
	// done!
}