#pragma once

#ifndef CANCELTRADE_H
#define CANCELTRADE_H

#include "../Packet.h"

class CancelTrade : public Packet
{
public:

	// Constructor
	CancelTrade();
	CancelTrade(byte*, int);
	CancelTrade(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif