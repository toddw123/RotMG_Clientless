#include "UpdateAck.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"


// Constructor
UpdateAck::UpdateAck()
{
	this->id = PacketType::UPDATEACK;
}
UpdateAck::UpdateAck(Packet &p) : Packet(p)
{
	this->id = PacketType::UPDATEACK;
	//Parse();
}

void UpdateAck::Send()
{
	// Clear the packet data
	this->clearData();
	// Literally nothing to write for this packet

	// Send the packet
	PacketIOHelper::SendPacket(this);
}