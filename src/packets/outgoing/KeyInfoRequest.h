#pragma once

#ifndef KEYINFOREQUEST_H
#define KEYINFOREQUEST_H

#include "../Packet.h"

class KeyInfoRequest : public Packet
{
public:
	std::vector<byte> request;
	// Constructor
	KeyInfoRequest();
	KeyInfoRequest(byte*, int);
	KeyInfoRequest(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif