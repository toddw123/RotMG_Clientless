#pragma once

#ifndef ClaimDailyRewardResponse_H
#define ClaimDailyRewardResponse_H

#include "../Packet.h"

class ClaimDailyRewardResponse : public Packet
{
public:
	int itemId;
	int quantity;
	int gold;

	// Constructor
	ClaimDailyRewardResponse();
	ClaimDailyRewardResponse(byte*, int);
	ClaimDailyRewardResponse(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif