#pragma once

#ifndef CREATE_H
#define CREATE_H

#include "../Packet.h"

class Create : public Packet
{
public:
	short classType;
	short skinType;

	// Constructor
	Create();
	Create(byte*, int);
	Create(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif