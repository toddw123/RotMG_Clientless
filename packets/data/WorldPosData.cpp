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
	x = p->readFloat();
	y = p->readFloat();
}

void WorldPosData::Write(Packet *p)
{
	p->writeBytes<float>(x);
	p->writeBytes<float>(y);
}

double WorldPosData::distanceTo(WorldPosData other)
{
	return sqrt(sqDistanceTo(other));
}

double WorldPosData::sqDistanceTo(WorldPosData other)
{
	double x = other.x - this->x;
	double y = other.y - this->y;
	return x * x + y * y;
}

double WorldPosData::angleTo(WorldPosData other)
{
	return atan2(other.y - this->y, other.x - this->x);
}
