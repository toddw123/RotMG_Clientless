#pragma once

#ifndef OTHERHIT_H
#define OTHERHIT_H

#include "../Packet.h"

class OtherHit : public Packet
{
public:
	int time;
	byte bulletId;
	int objectId;
	int targetId;

	// Constructor
	OtherHit();
	OtherHit(byte*, int);
	OtherHit(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif