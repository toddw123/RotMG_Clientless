#pragma once

#ifndef ACCEPTARENADEATH_H
#define ACCEPTARENADEATH_H

#include "../Packet.h"

class AcceptArenaDeath : public Packet
{
public:
	// Constructor
	AcceptArenaDeath();
	AcceptArenaDeath(byte*, int);
	AcceptArenaDeath(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif