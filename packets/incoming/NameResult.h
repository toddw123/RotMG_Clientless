#pragma once

#ifndef NAMERESULT_H
#define NAMERESULT_H

#include "..\Packet.h"

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
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif