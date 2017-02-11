#include "QuestFetchResponse.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
QuestFetchResponse::QuestFetchResponse()
{
	// Set packet id
	this->id = PacketType::QUEST_FETCH_RESPONSE;
}
QuestFetchResponse::QuestFetchResponse(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::QUEST_FETCH_RESPONSE;
	Parse();
}
QuestFetchResponse::QuestFetchResponse(const Packet &p) : Packet(p)
{
	this->id = PacketType::QUEST_FETCH_RESPONSE;
	Parse();
}

void QuestFetchResponse::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(tier);
	this->writeString<short>(goal);
	this->writeString<short>(description);
	this->writeString<short>(image);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void QuestFetchResponse::Parse()
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

void QuestFetchResponse::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}