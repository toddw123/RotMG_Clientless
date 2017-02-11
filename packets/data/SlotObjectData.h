#pragma once

#ifndef SLOTOBJECTDATA_H
#define SLOTOBJECTDATA_H

typedef unsigned char byte;

class Packet;

class SlotObjectData
{
public:
	int objectId;
	byte slotId;
	int objectType;

	SlotObjectData() {};
	SlotObjectData(Packet*);
	SlotObjectData(const SlotObjectData&);

	void Read(Packet*);
	void Write(Packet*);
};


#endif
