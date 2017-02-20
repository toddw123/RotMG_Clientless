#pragma once

#ifndef PASSWORDPROMPT_H
#define PASSWORDPROMPT_H

#include "../Packet.h"

class PasswordPrompt : public Packet
{
public:
	int cleanPasswordStatus;

	// Constructor
	PasswordPrompt();
	PasswordPrompt(byte*, int);
	PasswordPrompt(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif