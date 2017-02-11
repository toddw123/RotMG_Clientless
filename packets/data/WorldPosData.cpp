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