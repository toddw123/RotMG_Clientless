#pragma once

#ifndef INVRESULT_H
#define INVRESULT_H

#include "..\Packet.h"

class InvResult : public Packet
{
public:
	int result;

	// Constructor
	InvResult();
	InvResult(byte*, int);
	InvResult(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif