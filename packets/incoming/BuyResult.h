#pragma once

#ifndef BUYRESULT_H
#define BUYRESULT_H

#include "..\Packet.h"

enum ResultCodes : int
{
	UNKNOWN_ERROR_BRID = -1,
	SUCCESS_BRID = 0,
	INVALID_CHARACTER_BRID = 1,
	ITEM_NOT_FOUND_BRID = 2,
	NOT_ENOUGH_GOLD_BRID = 3,
	INVENTORY_FULL_BRID = 4,
	TOO_LOW_RANK_BRID = 5,
	NOT_ENOUGH_FAME_BRID = 6,
	PET_FEED_SUCCESS_BRID = 7
};

class BuyResult : public Packet
{
public:
	int result;
	std::string resultString;

	// Constructor
	BuyResult();
	BuyResult(byte*, int);
	BuyResult(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif