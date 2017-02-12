#pragma once

#ifndef ESCAPE_H
#define ESCAPE_H

#include "..\Packet.h"

class PlayerEscape : public Packet
{
public:

	// Constructor
	PlayerEscape();
	PlayerEscape(byte*, int);
	PlayerEscape(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif