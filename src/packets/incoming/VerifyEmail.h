#pragma once

#ifndef VERIFYEMAIL_H
#define VERIFYEMAIL_H

#include "../Packet.h"

class VerifyEmail : public Packet
{
public:
	// Constructor
	VerifyEmail();
	VerifyEmail(byte*, int);
	VerifyEmail(const Packet&);

	// Output
	Packet *write();
	// Input
	void read(); // nothing to parse
};

#endif