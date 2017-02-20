#pragma once

#ifndef GOTO_H
#define GOTO_H

#include "../Packet.h"
#include "../data/WorldPosData.h"

class Goto : public Packet
{
public:
	int objectId;
	WorldPosData pos;

	// Constructor
	Goto();
	Goto(byte*, int);
	Goto(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif