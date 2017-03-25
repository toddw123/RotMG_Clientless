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
WorldPosData::WorldPosData(double x, double y)
{
	this->x = x;
	this->y = y;
}

void WorldPosData::Read(Packet *p)
{
	x = static_cast<double>(p->readBytes<float>());
	y = static_cast<double>(p->readBytes<float>());
}

void WorldPosData::Write(Packet *p)
{
	p->writeBytes<float>(static_cast<float>(x));
	p->writeBytes<float>(static_cast<float>(y));
}

bool WorldPosData::outOfBounds(int width) const
{
	return x < 0 || y < 0 || x > width || y > width;
}

double WorldPosData::distanceTo(WorldPosData& other) const
{
	return sqrt(sqDistanceTo(other));
}
double WorldPosData::sqDistanceTo(WorldPosData& other) const
{
	double x = other.x - this->x;
	double y = other.y - this->y;
	return x * x + y * y;
}
double WorldPosData::angleTo(WorldPosData& other) const
{
	return atan2(other.y - this->y, other.x - this->x);
}

bool WorldPosData::operator==(const WorldPosData& other) const
{
	return (this->x == other.x && this->y == other.y);
}
bool WorldPosData::operator!=(const WorldPosData& other) const
{
	return !(*this == other);
}