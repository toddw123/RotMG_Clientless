#pragma once

#ifndef QUESTFETCHRESPONSE_H
#define QUESTFETCHRESPONSE_H

#include "../Packet.h"

class QuestFetchResponse : public Packet
{
public:
	int tier;
	std::string goal;
	std::string description;
	std::string image;

	// Constructor
	QuestFetchResponse();
	QuestFetchResponse(byte*, int);
	QuestFetchResponse(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif