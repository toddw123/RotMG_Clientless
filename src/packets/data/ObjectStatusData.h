#pragma once

#ifndef OBJECTSTATUSDATA_H
#define OBJECTSTATUSDATA_H

#include <vector>
#include "WorldPosData.h"
#include "StatData.h"

class Packet;

class ObjectStatusData
{
public:
	int objectId;
	WorldPosData pos;
	std::vector<StatData> stats;

	ObjectStatusData() {};
	ObjectStatusData(Packet*);
	ObjectStatusData(const ObjectStatusData&);

	void Read(Packet*);
	void Write(Packet*);
};


#endif
