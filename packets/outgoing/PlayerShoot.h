#pragma once

#ifndef PLAYERSHOOT_H
#define PLAYERSHOOT_H

#include "..\Packet.h"
#include "..\data\WorldPosData.h"

class PlayerShoot : public Packet
{
public:
	int time;
	byte bulletId;
	short containerType;
	WorldPosData startingPos;
	float angle;

	// Constructor
	PlayerShoot();
	PlayerShoot(byte*, int);
	PlayerShoot(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif