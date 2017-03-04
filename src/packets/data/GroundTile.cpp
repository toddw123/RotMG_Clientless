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
	x    = p->readBytes<short>();
	y    = p->readBytes<short>();
	type = p->readBytes<ushort>();
}
void GroundTile::Write(Packet *p)
{
	p->writeBytes<short>(x);
	p->writeBytes<short>(y);
	p->writeBytes<ushort>(type);
}