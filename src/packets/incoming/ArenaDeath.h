#pragma once

#ifndef ARENADEATH_H
#define ARENADEATH_H

#include "../Packet.h"

class ArenaDeath : public Packet
{
public:
	int cost;

	// Constructor
	ArenaDeath();
	ArenaDeath(byte*, int);
	ArenaDeath(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif