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
	Packet *write();
	// Input
	void read();
};

#endif