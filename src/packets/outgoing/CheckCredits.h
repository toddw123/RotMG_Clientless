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
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif