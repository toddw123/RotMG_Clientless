#include "GroundTile.h"
#include "../Packet.h"

GroundTile::GroundTile()
{
	x    = 0;
	y    = 0;
	type = 0;
}
GroundTile::GroundTile(Packet *p)
{
	// Just pass the packet to the Read function
	Read(p);
}
GroundTile::GroundTile(const GroundTile &gt)
{
	this->x = gt.x;
	this->y = gt.y;
	this->type = gt.type;
}

void GroundTile::Read(Packet *p)
{
	x    = p->readBytes<int>();
	y    = p->readBytes<int>();
	type = p->readBytes<uint>();
}
void GroundTile::Write(Packet *p)
{
	p->writeBytes<int>(x);
	p->writeBytes<int>(y);
	p->writeBytes<uint>(type);
}