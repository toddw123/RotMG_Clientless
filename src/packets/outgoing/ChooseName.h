#pragma once

#ifndef CHOOSENAME_H
#define CHOOSENAME_H

#include "../Packet.h"

class ChooseName : public Packet
{
public:
	std::string name;

	// Constructor
	ChooseName();
	ChooseName(byte*, int);
	ChooseName(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif