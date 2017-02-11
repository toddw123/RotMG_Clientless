#pragma once

#ifndef TEXT_H
#define TEXT_H

#include "..\Packet.h"

class Text : public Packet
{
public:
	std::string name;
	int objectId;
	int numStars;
	byte bubbleTime;
	std::string recipient;
	std::string text;
	std::string cleanText;

	// Constructor
	Text();
	Text(byte*, int);
	Text(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif