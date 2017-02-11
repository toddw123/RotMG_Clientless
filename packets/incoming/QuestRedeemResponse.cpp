#include "QuestRedeemResponse.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructors
QuestRedeemResponse::QuestRedeemResponse()
{
	// Set packet id
	this->id = PacketType::QUEST_REDEEM_RESPONSE;
}
QuestRedeemResponse::QuestRedeemResponse(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->id = PacketType::QUEST_REDEEM_RESPONSE;
	Parse();
}
QuestRedeemResponse::QuestRedeemResponse(const Packet &p) : Packet(p)
{
	this->id = PacketType::QUEST_REDEEM_RESPONSE;
	Parse();
}

void QuestRedeemResponse::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<bool>(ok);
	this->writeString<short>(message);
	// Send the packet
	PacketIOHelper::SendPacket(this);
}

void QuestRedeemResponse::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	ok = this->readBytes<bool>();
	message = this->readString<short>();
	// done!
}

void QuestRedeemResponse::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}