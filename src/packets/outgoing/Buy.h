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
	Packet *write();
	// Input
	void read();
};

#endif