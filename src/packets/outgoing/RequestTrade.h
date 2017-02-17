#pragma once

#ifndef REQUESTTRADE_H
#define REQUESTTRADE_H

#include "../Packet.h"

class RequestTrade : public Packet
{
public:
	std::string name;

	// Constructor
	RequestTrade();
	RequestTrade(byte*, int);
	RequestTrade(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif