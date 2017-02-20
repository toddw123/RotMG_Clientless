#pragma once

#ifndef DELETEPETMESSAGE_H
#define DELETEPETMESSAGE_H

#include "../Packet.h"

class DeletePetMessage : public Packet
{
public:
	int petId;

	// Constructor
	DeletePetMessage();
	DeletePetMessage(byte*, int);
	DeletePetMessage(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif