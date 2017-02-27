#pragma once

#ifndef WORLDPOSDATA_H
#define WORLDPOSDATA_H

class Packet;

class WorldPosData
{
public:
	float x;
	float y;

	WorldPosData() {};
	WorldPosData(Packet*);
	WorldPosData(const WorldPosData&);
	WorldPosData(float, float); // {x,y}

	void Read(Packet*);
	void Write(Packet*);

	float distanceTo(WorldPosData&) const;
	float sqDistanceTo(WorldPosData&) const;
	float angleTo(WorldPosData&) const;
};


#endif
