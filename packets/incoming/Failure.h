#pragma once

#ifndef FAILURE_H
#define FAILURE_H

#include "..\Packet.h"

enum Errors : int
{
	INCORRECT_VERSION = 4,
	BAD_KEY = 5,
	INVALID_TELEPORT_TARGET = 6,
	EMAIL_VERIFICATION_NEEDED = 7
};

class Failure : public Packet
{
public:
	int errorId;
	std::string errorDescription;

	// Constructor
	Failure();
	Failure(byte*, int);
	Failure(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif