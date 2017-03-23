#include "QuestRedeemResponse.h"


// Constructors
QuestRedeemResponse::QuestRedeemResponse()
{
	// Set packet id
	this->type_ = PacketType::QUEST_REDEEM_RESPONSE;
}
QuestRedeemResponse::QuestRedeemResponse(byte *b, int i) : Packet(b, i)
{
	// Set id and pass data to Parse
	this->type_ = PacketType::QUEST_REDEEM_RESPONSE;
	read();
}
QuestRedeemResponse::QuestRedeemResponse(const Packet &p) : Packet(p)
{
	this->type_ = PacketType::QUEST_REDEEM_RESPONSE;
	read();
}

Packet *QuestRedeemResponse::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<bool>(ok);
	this->writeString<short>(message);
	// Send the packet
	//PacketIOHelper::SendPacket(this);
	return this;
}

void QuestRedeemResponse::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	ok = this->readBool();
	message = this->readString<short>();
	// done!
}