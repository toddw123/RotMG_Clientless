#pragma once

#ifndef MOVERECORD_H
#define MOVERECORD_H

class Packet;

class MoveRecord
{
public:
	int time;
	double x;
	double y;

	MoveRecord() {};
	MoveRecord(Packet*);
	MoveRecord(const MoveRecord&);
	MoveRecord(int, double, double);

	void Read(Packet*);
	void Write(Packet*);
};


#endif
