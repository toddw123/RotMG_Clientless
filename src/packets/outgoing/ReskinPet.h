#pragma once

#ifndef RESKINPET_H
#define RESKINPET_H

#include "../Packet.h"
#include "../data/SlotObjectData.h"

class ReskinPet : public Packet
{
public:
	int petInstanceId;
	int pickedNewPetType;
	SlotObjectData item;

	// Constructor
	ReskinPet();
	ReskinPet(byte*, int);
	ReskinPet(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif