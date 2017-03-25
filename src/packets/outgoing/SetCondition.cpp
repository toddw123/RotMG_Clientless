#include "SetCondition.h"



// Constructor
SetCondition::SetCondition()
{
	this->type_ = PacketType::SETCONDITION;
}
SetCondition::SetCondition(byte *b, int i) : Packet(b, i)
{
	this->type_ = PacketType::SETCONDITION;
	read();
}
SetCondition::SetCondition(Packet &p) : Packet(p)
{
	this->type_ = PacketType::SETCONDITION;
	read();
}

Packet *SetCondition::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<byte>(conditionEffect);
	this->writeBytes<float>(static_cast<float>(conditionDuration));
	// Send the packet
	return this;
}

void SetCondition::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	conditionEffect = this->readBytes<byte>();
	conditionDuration = static_cast<double>(this->readBytes<float>());
	// done!
}