#pragma once

#ifndef TELEPORT_H
#define TELEPORT_H

#include "../Packet.h"

class Teleport : public Packet
{
public:
	int objectId;

	// Constructor
	Teleport();
	Teleport(byte*, int);
	Teleport(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif