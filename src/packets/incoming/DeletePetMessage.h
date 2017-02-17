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
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif