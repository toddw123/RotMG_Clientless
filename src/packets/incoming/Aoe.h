#pragma once

#ifndef AOE_H
#define AOE_H

#include "../Packet.h"
#include "../data/WorldPosData.h"

class Aoe : public Packet
{
public:
	WorldPosData pos;
	float radius;
	int damage;
	byte effect;
	float duration;
	int origType;

	// Constructor
	Aoe();
	Aoe(byte*, int);
	Aoe(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif