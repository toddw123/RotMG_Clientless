#pragma once

#ifndef BUY_H
#define BUY_H

#include "../Packet.h"

class Buy : public Packet
{
public:
	int objectId;
	int quantity;

	// Constructor
	Buy();
	Buy(byte*, int);
	Buy(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif