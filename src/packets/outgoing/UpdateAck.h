#pragma once

#ifndef UPDATEACK_H
#define UPDATEACK_H

#include "../Packet.h"

class UpdateAck : public Packet
{
public:
	// Constructor
	UpdateAck();
	UpdateAck(byte*, int);
	UpdateAck(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif