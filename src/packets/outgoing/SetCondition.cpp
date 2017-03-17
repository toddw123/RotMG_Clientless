#include "SetCondition.h"



// Constructor
SetCondition::SetCondition()
{
	this->type = PacketType::SETCONDITION;
}
SetCondition::SetCondition(byte *b, int i) : Packet(b, i)
{
	this->type = PacketType::SETCONDITION;
	read();
}
SetCondition::SetCondition(Packet &p) : Packet(p)
{
	this->type = PacketType::SETCONDITION;
	read();
}

Packet *SetCondition::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<byte>(conditionEffect);
	this->writeBytes<float>(conditionDuration);
	// Send the packet
	return this;
}

void SetCondition::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	conditionEffect = this->readBytes<byte>();
	conditionDuration = this->readBytes<float>();
	// done!
}