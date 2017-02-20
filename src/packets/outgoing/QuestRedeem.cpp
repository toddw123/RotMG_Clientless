#include "QuestRedeem.h"
#include "../PacketType.h"


// Constructor
QuestRedeem::QuestRedeem()
{
	this->id = PacketType::QUEST_REDEEM;
}
QuestRedeem::QuestRedeem(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::QUEST_REDEEM;
	read();
}
QuestRedeem::QuestRedeem(Packet &p) : Packet(p)
{
	this->id = PacketType::QUEST_REDEEM;
	read();
}

Packet *QuestRedeem::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	slotObj.Write(this);
	// Send the packet
	return this;
}

void QuestRedeem::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	slotObj.Read(this);
	// done!
}