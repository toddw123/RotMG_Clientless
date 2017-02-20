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
	Packet *write();
	// Input
	void read();
};

#endif