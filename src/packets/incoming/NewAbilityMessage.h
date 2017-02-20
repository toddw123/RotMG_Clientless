#pragma once

#ifndef NEWABILITYMESSAGE_H
#define NEWABILITYMESSAGE_H

#include "../Packet.h"

class NewAbilityMessage : public Packet
{
public:
	int type;

	// Constructor
	NewAbilityMessage();
	NewAbilityMessage(byte*, int);
	NewAbilityMessage(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif