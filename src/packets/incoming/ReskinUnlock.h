#pragma once

#ifndef RESKINUNLOCK_H
#define RESKINUNLOCK_H

#include "../Packet.h"

class ReskinUnlock : public Packet
{
public:
	int skinId;

	// Constructor
	ReskinUnlock();
	ReskinUnlock(byte*, int);
	ReskinUnlock(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif