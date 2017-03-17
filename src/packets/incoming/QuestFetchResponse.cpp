#include "QuestFetchResponse.h"


// Constructors
QuestFetchResponse::QuestFetchResponse()
{
	// Set packet id
	this->type = PacketType::QUEST_FETCH_RESPONSE;
}
QuestFetchResponse::QuestFetchResponse(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type = PacketType::QUEST_FETCH_RESPONSE;
	read();
}
QuestFetchResponse::QuestFetchResponse(const Packet &p) : Packet(p)
{
	this->type = PacketType::QUEST_FETCH_RESPONSE;
	read();
}

Packet *QuestFetchResponse::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(tier);
	this->writeString<short>(goal);
	this->writeString<short>(description);
	this->writeString<short>(image);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void QuestFetchResponse::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	tier = this->readBytes<int>();
	goal = this->readString<short>();
	description = this->readString<short>();
	image = this->readString<short>();
	// done!
}