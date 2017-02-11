#pragma once

#ifndef ESCAPE_H
#define ESCAPE_H

#include "..\Packet.h"

class Escape : public Packet
{
public:

	// Constructor
	Escape();
	Escape(byte*, int);
	Escape(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif