#pragma once

#ifndef ACTIVEPETUPDATE_H
#define ACTIVEPETUPDATE_H

#include "../Packet.h"

class ActivePetUpdate : public Packet
{
public:
	int instanceId;

	// Constructor
	ActivePetUpdate();
	ActivePetUpdate(byte*, int);
	ActivePetUpdate(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif