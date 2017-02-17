#pragma once

#ifndef LEAVEARENA_H
#define LEAVEARENA_H

#include "../Packet.h"

class LeaveArena : public Packet
{
public:
	int time;

	// Constructor
	LeaveArena();
	LeaveArena(byte*, int);
	LeaveArena(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif