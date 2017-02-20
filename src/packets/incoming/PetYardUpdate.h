#pragma once

#ifndef PETYARDUPDATE_H
#define PETYARDUPDATE_H

#include "../Packet.h"

class PetYardUpdate : public Packet
{
public:
	int type;

	// Constructor
	PetYardUpdate();
	PetYardUpdate(byte*, int);
	PetYardUpdate(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif