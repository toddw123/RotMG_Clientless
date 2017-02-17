#pragma once

#ifndef CANCELTRADE_H
#define CANCELTRADE_H

#include "..\Packet.h"

class CancelTrade : public Packet
{
public:

	// Constructor
	CancelTrade();
	CancelTrade(byte*, int);
	CancelTrade(Packet&);

	// Output
	void Send();
	// Input
	void Parse();
	void Fill(byte*, int);
};

#endif