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
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif