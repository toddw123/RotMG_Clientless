#pragma once

#ifndef WORLDPOSDATA_H
#define WORLDPOSDATA_H

#include <cmath>

class Packet;

class WorldPosData
{
public:
	double x;
	double y;

	WorldPosData() {};
	WorldPosData(Packet*);
	WorldPosData(const WorldPosData&);
	WorldPosData(double, double); // {x,y}

	void Read(Packet*);
	void Write(Packet*);

	bool outOfBounds(int) const;

	double distanceTo(WorldPosData&) const;
	double sqDistanceTo(WorldPosData&) const;
	double angleTo(WorldPosData&) const;

	bool operator==(const WorldPosData&) const;
	bool operator!=(const WorldPosData&) const;
};


#endif
