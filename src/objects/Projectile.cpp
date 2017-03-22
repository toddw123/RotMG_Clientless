#include "Projectile.h"

#include "../packets/incoming/EnemyShoot.h"
#include "ObjectLibrary.h"

#include <math.h>

Projectile::Projectile()
{
	bulletId = 0;
	ownerId = 0;
	containerType = 0;
	bulletType = 0;
	damagesEnemies = false;
	damagesPlayers = false;
	damage = 0;
	startX = 0;
	startY = 0;
	startTime = 0;
	angle = 0;
}

Projectile::Projectile(int ownerObjType, uint time, int bId, int oId, int bType, int dmg, float angle, WorldPosData pos, bool who)
{
	Object* owner = ObjectLibrary::getObject(ownerObjType);
	if (owner != NULL)
	{
		if (owner->projectiles.find(bType) != owner->projectiles.end())
		{
			this->projProps = owner->projectiles[bType];
		}
	}

	this->bulletId = bId;
	this->ownerId = oId;
	containerType = 0; // Not sure what this is exactly
	this->bulletType = bType;
	damagesEnemies = who;
	damagesPlayers = !who;
	this->damage = dmg;
	this->startX = (double)pos.x;
	this->startY = (double)pos.y;
	this->startTime = time;
	this->angle = angle;
}


// This function is just copied from the client, will clean up later
void Projectile::positionAt(uint time, WorldPosData& out)
{
	uint elapsed = time - this->startTime;
	double pi = 3.141592653589793;

	double _loc3_ = elapsed * (this->projProps.speed / 10000);
	double _loc4_ = this->bulletId % 2 == 0 ? 0 : pi;
	if (this->projProps.wavy)
	{
		double _loc5_ = 6 * pi;
		double _loc6_ = pi / 64;
		double _loc7_ = this->angle + _loc6_ * sin(_loc4_ + _loc5_ * elapsed / 1000);
		out.x = this->startX + _loc3_ * cos(_loc7_);
		out.y = this->startY + _loc3_ * sin(_loc7_);
	}
	else if (this->projProps.parametric)
	{
		double _loc8_ = elapsed / this->projProps.lifetime * 2 * pi;
		double _loc9_ = sin(_loc8_) * (this->bulletId % 2 == 0 ? 1 : -1);
		double _loc10_ = sin(2 * _loc8_) * (this->bulletId % 4 < 2 ? 1 : -1);
		double _loc11_ = sin(this->angle);
		double _loc12_ = cos(this->angle);
		out.x = this->startX + (_loc9_ * _loc12_ - _loc10_ * _loc11_) * this->projProps.magnitude;
		out.y = this->startY + (_loc9_ * _loc11_ + _loc10_ * _loc12_) * this->projProps.magnitude;
	}
	else
	{
		if (this->projProps.boomerang)
		{
			double _loc13_ = this->projProps.lifetime * (this->projProps.speed / 10000) / 2;
			if (_loc3_ > _loc13_)
			{
				_loc3_ = _loc13_ - (_loc3_ - _loc13_);
			}
		}
		out.x = this->startX + _loc3_ * cos(this->angle);
		out.y = this->startY + _loc3_ * sin(this->angle);
		if (this->projProps.amplitude != 0)
		{
			double _loc14_ = this->projProps.amplitude * sin(_loc4_ + elapsed / this->projProps.lifetime * this->projProps.frequency * 2 * pi);
			out.x = out.x + _loc14_ * cos(this->angle + pi / 2);
			out.y = out.y + _loc14_ * sin(this->angle + pi / 2);
		}
	}
}