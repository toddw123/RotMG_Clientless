#pragma once

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "..\Packet.h"

class Notification : public Packet
{
public:
	int objectId;
	std::string message;
	int color;

	// Constructor
	Notification();
	Notification(byte*, int);
	Notification(const Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif