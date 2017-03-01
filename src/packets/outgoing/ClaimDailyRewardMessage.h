#pragma once

#ifndef CLAIMDAILYREWARDMESSAGE_H
#define CLAIMDAILYREWARDMESSAGE_H

#include "../Packet.h"

class ClaimDailyRewardMessage : public Packet
{
public:
	std::string claimKey;
	std::string type;

	// Constructor
	ClaimDailyRewardMessage();
	ClaimDailyRewardMessage(byte*, int);
	ClaimDailyRewardMessage(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif