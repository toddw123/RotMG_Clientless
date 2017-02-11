#pragma once

#ifndef SERVERPLAYERSHOOT_H
#define SERVERPLAYERSHOOT_H

#include "../Packet.h"
#include "../data/WorldPosData.h"

class ServerPlayerShoot : public Packet
{
public:
	byte bulletId;
	int ownerId;
	int containerType;
	WorldPosData startingPos;
	float angle;
	short damage;

	// Constructor
	ServerPlayerShoot();
	ServerPlayerShoot(byte*, int);
	ServerPlayerShoot(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif