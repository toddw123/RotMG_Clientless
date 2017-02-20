#pragma once

#ifndef TRADEDONE_H
#define TRADEDONE_H

#include "../Packet.h"

enum TradeDoneCode
{
	TRADE_SUCCESSFUL= 0,
	PLAYER_CANCELED = 1
};

class TradeDone : public Packet
{
public:
	int code;
	std::string description;

	// Constructor
	TradeDone();
	TradeDone(byte*, int);
	TradeDone(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif