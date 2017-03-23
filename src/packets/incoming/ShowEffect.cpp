#include "ShowEffect.h"


// Constructors
ShowEffect::ShowEffect()
{
	// Set packet id
	this->type_ = PacketType::SHOWEFFECT;
}
ShowEffect::ShowEffect(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type_ = PacketType::SHOWEFFECT;
	read();
}
ShowEffect::ShowEffect(const Packet &p) : Packet(p)
{
	this->type_ = PacketType::SHOWEFFECT;
	read();
}

Packet *ShowEffect::write()
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
	//PacketIOHelper::SendPacket(this);
	return this;
}

void ShowEffect::read()
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