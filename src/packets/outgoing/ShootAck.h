#pragma once

#ifndef SHOOTACK_H
#define SHOOTACK_H

#include "../Packet.h"

class ShootAck : public Packet
{
public:
	int time;

	// Constructor
	ShootAck();
	ShootAck(byte*, int);
	ShootAck(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif