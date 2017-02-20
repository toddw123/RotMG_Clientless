#pragma once

#ifndef PING_H
#define PING_H

#include "../Packet.h"

class Ping : public Packet
{
public:
	int serial;

	// Constructor
	Ping();
	Ping(byte*, int);
	Ping(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif