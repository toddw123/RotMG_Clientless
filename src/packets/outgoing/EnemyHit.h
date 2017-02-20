#pragma once

#ifndef ENEMYHIT_H
#define ENEMYHIT_H

#include "../Packet.h"

class EnemyHit : public Packet
{
public:
	int time;
	byte bulletId;
	int targetId;
	bool kill;

	// Constructor
	EnemyHit();
	EnemyHit(byte*, int);
	EnemyHit(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif