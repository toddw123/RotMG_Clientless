#pragma once

#ifndef NAMERESULT_H
#define NAMERESULT_H

#include "../Packet.h"

class NameResult : public Packet
{
public:
	bool success;
	std::string errorText;

	// Constructor
	NameResult();
	NameResult(byte*, int);
	NameResult(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif