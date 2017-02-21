#include "WorldPosData.h"
#include "../Packet.h"


WorldPosData::WorldPosData(Packet *p)
{
	// Just pass the packet to the Read function
	Read(p);
}
WorldPosData::WorldPosData(const WorldPosData &pos)
{
	this->x = pos.x;
	this->y = pos.y;
}
WorldPosData::WorldPosData(float x, float y)
{
	this->x = x;
	this->y = y;
}

void WorldPosData::Read(Packet *p)
{
	x = p->readBytes<float>();
	y = p->readBytes<float>();
}

void WorldPosData::Write(Packet *p)
{
	p->writeBytes<float>(x);
	p->writeBytes<float>(y);
}

float WorldPosData::distanceTo(WorldPosData other)
{
	return sqrt(sqDistanceTo(other));
}

float WorldPosData::sqDistanceTo(WorldPosData other)
{
	float x = other.x - this->x;
	float y = other.y - this->y;
	return x * x + y * y;
}

float WorldPosData::angleTo(WorldPosData other)
{
	return atan2(other.y - this->y, other.x - this->x);
}
