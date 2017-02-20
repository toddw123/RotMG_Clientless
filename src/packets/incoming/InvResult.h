#pragma once

#ifndef INVRESULT_H
#define INVRESULT_H

#include "../Packet.h"

class InvResult : public Packet
{
public:
	int result;

	// Constructor
	InvResult();
	InvResult(byte*, int);
	InvResult(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif