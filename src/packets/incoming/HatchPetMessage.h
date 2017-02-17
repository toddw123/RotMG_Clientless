#pragma once

#ifndef HATCHPETMESSAGE_H
#define HATCHPETMESSAGE_H

#include "../Packet.h"

class HatchPetMessage : public Packet
{
public:
	std::string petName;
	int petSkin;

	// Constructor
	HatchPetMessage();
	HatchPetMessage(byte*, int);
	HatchPetMessage(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif