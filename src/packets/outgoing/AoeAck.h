#pragma once

#ifndef AOEACK_H
#define AOEACK_H

#include "../Packet.h"
#include "../data/WorldPosData.h"

class AoeAck : public Packet
{
public:
	int time;
	WorldPosData position;

	// Constructor
	AoeAck();
	AoeAck(byte*, int);
	AoeAck(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif