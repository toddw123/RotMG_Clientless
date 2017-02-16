#pragma once

#ifndef OBJECTDATA_H
#define OBJECTDATA_H

#include "ObjectStatusData.h"

typedef unsigned short ushort;

class Packet;

class ObjectData
{
public:
	ushort objectType;
	ObjectStatusData status;

	ObjectData() {};
	ObjectData(Packet*);
	ObjectData(const ObjectData&);

	void Read(Packet*);
	void Write(Packet*);
};


#endif
