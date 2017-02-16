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
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif