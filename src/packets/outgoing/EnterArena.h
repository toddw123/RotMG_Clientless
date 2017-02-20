#pragma once

#ifndef ENTERARENA_H
#define ENTERARENA_H

#include "../Packet.h"

class EnterArena : public Packet
{
public:
	int currency;

	// Constructor
	EnterArena();
	EnterArena(byte*, int);
	EnterArena(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif