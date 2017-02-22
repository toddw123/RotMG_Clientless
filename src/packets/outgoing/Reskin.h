#pragma once

#ifndef RESKIN_H
#define RESKIN_H

#include "../Packet.h"

class Reskin : public Packet
{
public:
	int skinId;

	// Constructor
	Reskin();
	Reskin(byte*, int);
	Reskin(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif