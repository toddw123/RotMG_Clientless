#pragma once

#ifndef NEWTICK_H
#define NEWTICK_H

#include "../Packet.h"
#include "../data/ObjectStatusData.h"



class NewTick : public Packet
{
public:
	int tickId;
	int tickTime;
	std::vector<ObjectStatusData> statuses;

	// Constructor
	NewTick();
	NewTick(byte*, int);
	NewTick(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif