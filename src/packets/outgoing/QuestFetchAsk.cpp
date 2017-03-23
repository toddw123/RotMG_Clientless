#include "QuestFetchAsk.h"



// Constructor
QuestFetchAsk::QuestFetchAsk()
{
	this->type_ = PacketType::QUEST_FETCH_ASK;
}
QuestFetchAsk::QuestFetchAsk(byte *b, int i) : Packet(b, i)
{
	this->type_ = PacketType::QUEST_FETCH_ASK;
	read();
}
QuestFetchAsk::QuestFetchAsk(Packet &p) : Packet(p)
{
	this->type_ = PacketType::QUEST_FETCH_ASK;
	read();
}

Packet *QuestFetchAsk::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data

	// Send the packet
	return this;
}

void QuestFetchAsk::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data

	// done!
}