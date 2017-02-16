#include "ShowEffect.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
ShowEffect::ShowEffect()
{
	// Set packet id
	this->id = PacketType::SHOWEFFECT;
}
ShowEffect::ShowEffect(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::SHOWEFFECT;
	Parse();
}
ShowEffect::ShowEffect(const Packet &p) : Packet(p)
{
	this->id = PacketType::SHOWEFFECT;
	Parse();
}

void ShowEffect::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<byte>(effectType);
	this->writeBytes<int>(targetObjectId);
	pos1.Write(this);
	pos2.Write(this);
	this->writeBytes<int>(color);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void ShowEffect::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	effectType = this->readBytes<byte>();
	targetObjectId = this->readBytes<int>();
	pos1.Read(this);
	pos2.Read(this);
	color = this->readBytes<int>();
	// done!
}

void ShowEffect::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}