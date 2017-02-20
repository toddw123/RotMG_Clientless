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
	Packet *write();
	// Input
	void read();
};

#endif