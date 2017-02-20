#pragma once

#ifndef TRADEACCEPTED_H
#define TRADEACCEPTED_H

#include "../Packet.h"

class TradeAccepted : public Packet
{
public:
	std::vector<bool> myOffer;
	std::vector<bool> yourOffer;

	// Constructor
	TradeAccepted();
	TradeAccepted(byte*, int);
	TradeAccepted(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif