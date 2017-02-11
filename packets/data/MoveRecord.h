#pragma once

#ifndef MOVERECORD_H
#define MOVERECORD_H

class Packet;

class MoveRecord
{
public:
	int time;
	float x;
	float y;

	MoveRecord() {};
	MoveRecord(Packet*);
	MoveRecord(const MoveRecord&);
	MoveRecord(int, float, float);

	void Read(Packet*);
	void Write(Packet*);
};


#endif
