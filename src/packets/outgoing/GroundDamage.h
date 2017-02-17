#pragma once

#ifndef GROUNDDAMAGE_H
#define GROUNDDAMAGE_H

#include "../Packet.h"
#include "../data/WorldPosData.h"
#include "../data/MoveRecord.h"


class GroundDamage : public Packet
{
public:
	int time;
	WorldPosData curPosition;

	// Constructor
	GroundDamage();
	GroundDamage(byte*, int);
	GroundDamage(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif