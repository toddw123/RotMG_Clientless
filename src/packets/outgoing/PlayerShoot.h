#pragma once

#ifndef PLAYERSHOOT_H
#define PLAYERSHOOT_H

#include "../Packet.h"
#include "../data/WorldPosData.h"

class PlayerShoot : public Packet
{
public:
	int time;
	byte bulletId;
	short containerType;
	WorldPosData startingPos;
	float angle;

	// Constructor
	PlayerShoot();
	PlayerShoot(byte*, int);
	PlayerShoot(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif