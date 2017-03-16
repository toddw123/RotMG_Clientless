#include "ImminentArenaWave.h"


// Constructors
ImminentArenaWave::ImminentArenaWave()
{
	// Set packet id
	this->type = PacketType::IMMINENT_ARENA_WAVE;
}
ImminentArenaWave::ImminentArenaWave(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type = PacketType::IMMINENT_ARENA_WAVE;
	read();
}
ImminentArenaWave::ImminentArenaWave(const Packet &p) : Packet(p)
{
	this->type = PacketType::IMMINENT_ARENA_WAVE;
	read();
}

Packet *ImminentArenaWave::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(currentRuntime);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void ImminentArenaWave::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	currentRuntime = this->readBytes<int>();
	// done!
}