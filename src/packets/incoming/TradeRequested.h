#pragma once

#ifndef TRADEREQUESTED_H
#define TRADEREQUESTED_H

#include "../Packet.h"

class TradeRequested : public Packet
{
public:
	std::string name;

	// Constructor
	TradeRequested();
	TradeRequested(byte*, int);
	TradeRequested(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif