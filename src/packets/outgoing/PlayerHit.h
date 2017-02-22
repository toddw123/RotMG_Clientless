#pragma once

#ifndef PLAYERHIT_H
#define PLAYERHIT_H

#include "../Packet.h"

class PlayerHit : public Packet
{
public:
	byte bulletId;
	int objectId;

	// Constructor
	PlayerHit();
	PlayerHit(byte*, int);
	PlayerHit(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif