#include "ClaimDailyRewardResponse.h"



// Constructor
ClaimDailyRewardResponse::ClaimDailyRewardResponse()
{
	this->type = PacketType::LOGIN_REWARD_MSG;
}
ClaimDailyRewardResponse::ClaimDailyRewardResponse(byte *b, int i) : Packet(b, i)
{
	this->type = PacketType::LOGIN_REWARD_MSG;
	read();
}
ClaimDailyRewardResponse::ClaimDailyRewardResponse(Packet &p) : Packet(p)
{
	this->type = PacketType::LOGIN_REWARD_MSG;
	read();
}

Packet *ClaimDailyRewardResponse::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write data
	this->writeBytes<int>(itemId);
	this->writeBytes<int>(quantity);
	this->writeBytes<int>(gold);
	// Send the packet
	return this;
}

void ClaimDailyRewardResponse::read()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	itemId = this->readBytes<int>();
	quantity = this->readBytes<int>();
	gold = this->readBytes<int>();
	// done!
}