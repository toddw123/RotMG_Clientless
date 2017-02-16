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
	void Send();
	// Input
	void Parse(); // nothing to parse
	void Fill(byte*, int); // no data to fill
};

#endif