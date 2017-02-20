#pragma once

#ifndef EVOLVEDPETMESSAGE_H
#define EVOLVEDPETMESSAGE_H

#include "../Packet.h"

class EvolvedPetMessage : public Packet
{
public:
	int petId;
	int initialSkin;
	int finalSkin;

	// Constructor
	EvolvedPetMessage();
	EvolvedPetMessage(byte*, int);
	EvolvedPetMessage(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif