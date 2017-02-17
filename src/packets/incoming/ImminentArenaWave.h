#pragma once

#ifndef IMMINENTARENAWAVE_H
#define IMMINENTARENAWAVE_H

#include "../Packet.h"

class ImminentArenaWave : public Packet
{
public:
	int currentRuntime;

	// Constructor
	ImminentArenaWave();
	ImminentArenaWave(byte*, int);
	ImminentArenaWave(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif