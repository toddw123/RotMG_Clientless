#pragma once

#ifndef UPDATE_H
#define UPDATE_H

#include "../Packet.h"
#include "../data/GroundTile.h"
#include "../data/ObjectData.h"



class Update : public Packet
{
public:
	std::vector<GroundTile> tiles;
	std::vector<ObjectData> newObjs;
	std::vector<int> drops;

	// Constructor
	Update();
	Update(byte*, int);
	Update(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif