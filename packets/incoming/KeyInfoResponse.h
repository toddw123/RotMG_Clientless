#pragma once

#ifndef KEYINFORESPONSE_H
#define KEYINFORESPONSE_H

#include "../Packet.h"

class KeyInfoResponse : public Packet
{
public:
	std::string name;
	std::string description;
	std::string creator;

	// Constructor
	KeyInfoResponse();
	KeyInfoResponse(byte*, int);
	KeyInfoResponse(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif