#pragma once

#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Object.h"

class EnemyShoot;
class WorldPosData;

class Projectile
{
public:
	Object containerProps;
	ProjectileProperties projProps;

	int bulletId;
	int ownerId;
	int containerType;
	int bulletType;
	bool damagesEnemies;
	bool damagesPlayers;
	int damage;
	double startX;
	double startY;
	uint startTime;
	float angle;

	Projectile();
	Projectile(const Projectile&);
	Projectile(int, uint, int, int, int, int, float, WorldPosData, bool);

	void positionAt(uint, WorldPosData&);


};

#endif