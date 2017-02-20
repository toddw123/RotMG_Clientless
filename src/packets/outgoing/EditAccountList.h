#pragma once

#ifndef EDITACCOUNTLIST_H
#define EDITACCOUNTLIST_H

#include "../Packet.h"

class EditAccountList : public Packet
{
public:
	int accountListId;
	bool add;
	int objectId;

	// Constructor
	EditAccountList();
	EditAccountList(byte*, int);
	EditAccountList(Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif