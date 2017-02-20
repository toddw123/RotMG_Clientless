#pragma once

#ifndef PONG_H
#define PONG_H

#include "../Packet.h"

class Pong : public Packet
{
public:
	int serial;
	int time;

	// Constructor
	Pong();
	Pong(byte*, int);
	Pong(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif