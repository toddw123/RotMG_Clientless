#pragma once

#ifndef ACCEPTTRADE_H
#define ACCEPTTRADE_H

#include "../Packet.h"

class AcceptTrade : public Packet
{
public:
	std::vector<bool> myOffer;
	std::vector<bool> yourOffer;

	// Constructor
	AcceptTrade();
	AcceptTrade(byte*, int);
	AcceptTrade(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif