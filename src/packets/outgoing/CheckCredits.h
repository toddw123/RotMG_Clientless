#pragma once

#ifndef CHECKCREDITS_H
#define CHECKCREDITS_H

#include "..\Packet.h"

class CheckCredits : public Packet
{
public:

	// Constructor
	CheckCredits();
	CheckCredits(byte*, int);
	CheckCredits(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif