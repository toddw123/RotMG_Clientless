#pragma once

#ifndef ALLYSHOOT_H
#define ALLYSHOOT_H

#include "../Packet.h"

class AllyShoot : public Packet
{
public:
	byte bulletId;
	int ownerId;
	short containerType;
	double angle;

	// Constructor
	AllyShoot();
	AllyShoot(byte*, int);
	AllyShoot(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif