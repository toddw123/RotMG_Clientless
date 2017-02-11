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
	float angle;

	// Constructor
	AllyShoot();
	AllyShoot(byte*, int);
	AllyShoot(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif