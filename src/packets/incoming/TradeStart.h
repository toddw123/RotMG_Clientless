#pragma once

#ifndef TRADESTART_H
#define TRADESTART_H

#include "../Packet.h"
#include "../data/TradeItem.h"

class TradeStart : public Packet
{
public:
	std::vector<TradeItem> myItems;
	std::string yourName;
	std::vector<TradeItem> yourItems;

	// Constructor
	TradeStart();
	TradeStart(byte*, int);
	TradeStart(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif