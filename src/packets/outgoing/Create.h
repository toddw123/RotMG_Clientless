#pragma once

#ifndef Create_H
#define Create_H

#include "../Packet.h"

class Create : public Packet
{
public:
	short classType;
	short skinType;

	// Constructor
	Create();
	Create(byte*, int);
	Create(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif