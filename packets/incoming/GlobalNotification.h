#pragma once

#ifndef GLOBALNOTIFICATION_H
#define GLOBALNOTIFICATION_H

#include "../Packet.h"

class GlobalNotification : public Packet
{
public:
	int type;
	std::string text;

	// Constructor
	GlobalNotification();
	GlobalNotification(byte*, int);
	GlobalNotification(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif