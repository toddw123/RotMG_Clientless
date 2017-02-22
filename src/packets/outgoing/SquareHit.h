#pragma once

#ifndef SQUAREHIT_H
#define SQUAREHIT_H

#include "../Packet.h"

class SquareHit : public Packet
{
public:
	int time;
	byte bulletId;
	int objectId;

	// Constructor
	SquareHit();
	SquareHit(byte*, int);
	SquareHit(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif