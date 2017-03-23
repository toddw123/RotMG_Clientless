#include "ClaimDailyRewardMessage.h"



// Constructor
ClaimDailyRewardMessage::ClaimDailyRewardMessage()
{
	this->type_ = PacketType::CLAIM_LOGIN_REWARD_MSG;
}
ClaimDailyRewardMessage::ClaimDailyRewardMessage(byte *b, int i) : Packet(b, i)
{
	this->type_ = PacketType::CLAIM_LOGIN_REWARD_MSG;
	read();
}
ClaimDailyRewardMessage::ClaimDailyRewardMessage(Packet &p) : Packet(p)
{
	this->type_ = PacketType::CLAIM_LOGIN_REWARD_MSG;
	read();
}

Packet *ClaimDailyRewardMessage::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeString<short>(claimKey);
	this->writeString<short>(type);
	// Send the packet
	return this;
}

void ClaimDailyRewardMessage::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	claimKey = this->readString<short>();
	type = this->readString<short>();
	// done!
}