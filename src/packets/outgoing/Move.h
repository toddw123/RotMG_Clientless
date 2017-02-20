#pragma once

#ifndef MOVE_H
#define MOVE_H

#include "../Packet.h"
#include "../data/WorldPosData.h"
#include "../data/MoveRecord.h"


class Move : public Packet
{
public:
	int tickId;
	int time;
	WorldPosData newPosition;
	std::vector<MoveRecord> records;

	// Constructor
	Move();
	Move(byte*, int);
	Move(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif