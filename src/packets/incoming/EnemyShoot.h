#pragma once

#ifndef ENEMYSHOOT_H
#define EMEMYSHOOT_H

#include "../Packet.h"
#include "../data/WorldPosData.h"

class EnemyShoot : public Packet
{
public:
	byte bulletId;
	int ownerId;
	byte bulletType;
	WorldPosData startingPos;
	float angle;
	short damage;
	byte numShots;
	float angleInc;

	// Constructor
	EnemyShoot();
	EnemyShoot(byte*, int);
	EnemyShoot(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif