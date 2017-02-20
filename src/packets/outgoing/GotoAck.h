#pragma once

#ifndef GOTOACK_H
#define GOTOACK_H

#include "../Packet.h"

class GotoAck : public Packet
{
public:
	int time;

	// Constructor
	GotoAck();
	GotoAck(byte*, int);
	GotoAck(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif