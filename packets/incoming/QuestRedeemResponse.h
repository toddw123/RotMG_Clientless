#pragma once

#ifndef QUESTREDEEMRESPONSE_H
#define QUESTREDEEMRESPONSE_H

#include "../Packet.h"

class QuestRedeemResponse : public Packet
{
public:
	bool ok;
	std::string message;

	// Constructor
	QuestRedeemResponse();
	QuestRedeemResponse(byte*, int);
	QuestRedeemResponse(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif