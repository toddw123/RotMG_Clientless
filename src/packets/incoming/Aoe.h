#pragma once

#ifndef AOE_H
#define AOE_H

#include "../Packet.h"
#include "../data/WorldPosData.h"

class Aoe : public Packet
{
public:
	WorldPosData pos;
	double radius;
	int damage;
	byte effect;
	double duration;
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