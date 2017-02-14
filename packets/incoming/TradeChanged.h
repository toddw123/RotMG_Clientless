#pragma once

#ifndef TRADECHANGED_H
#define TRADECHANGED_H

#include "../Packet.h"

class TradeChanged : public Packet
{
public:
	std::vector<bool> offer;

	// Constructor
	TradeChanged();
	TradeChanged(byte*, int);
	TradeChanged(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif