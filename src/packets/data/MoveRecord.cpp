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
MoveRecord::MoveRecord(int t, double x, double y)
{
	this->time = t;
	this->x = x;
	this->y = y;
}

void MoveRecord::Read(Packet *p)
{
	time = p->readBytes<int>();
	x = static_cast<double>(p->readBytes<float>());
	y = static_cast<double>(p->readBytes<float>());
}

void MoveRecord::Write(Packet *p)
{
	p->writeBytes<int>(time);
	p->writeBytes<float>(static_cast<float>(x));
	p->writeBytes<float>(static_cast<float>(y));
}