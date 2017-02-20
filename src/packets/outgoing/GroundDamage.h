#pragma once

#ifndef GROUNDDAMAGE_H
#define GROUNDDAMAGE_H

#include "../Packet.h"
#include "../data/WorldPosData.h"


class GroundDamage : public Packet
{
public:
	int time;
	WorldPosData position;

	// Constructor
	GroundDamage();
	GroundDamage(byte*, int);
	GroundDamage(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif