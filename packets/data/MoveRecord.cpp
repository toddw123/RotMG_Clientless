#include "MoveRecord.h"
#include "../Packet.h"


MoveRecord::MoveRecord(Packet *p)
{
	// Just pass the packet to the Read function
	Read(p);
}
MoveRecord::MoveRecord(const MoveRecord &pos)
{
	this->time = pos.time;
	this->x = pos.x;
	this->y = pos.y;
}
MoveRecord::MoveRecord(int t, float x, float y)
{
	this->time = t;
	this->x = x;
	this->y = y;
}

void MoveRecord::Read(Packet *p)
{
	time = p->readBytes<int>();
	x = p->readFloat();
	y = p->readFloat();
}

void MoveRecord::Write(Packet *p)
{
	p->writeBytes<int>(time);
	p->writeBytes<float>(x);
	p->writeBytes<float>(y);
}