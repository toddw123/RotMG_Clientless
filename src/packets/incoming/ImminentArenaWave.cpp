#include "ImminentArenaWave.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
ImminentArenaWave::ImminentArenaWave()
{
	// Set packet id
	this->id = PacketType::IMMINENT_ARENA_WAVE;
}
ImminentArenaWave::ImminentArenaWave(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::IMMINENT_ARENA_WAVE;
	Parse();
}
ImminentArenaWave::ImminentArenaWave(const Packet &p) : Packet(p)
{
	this->id = PacketType::IMMINENT_ARENA_WAVE;
	Parse();
}

void ImminentArenaWave::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(currentRuntime);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void ImminentArenaWave::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	currentRuntime = this->readBytes<int>();
	// done!
}

void ImminentArenaWave::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}