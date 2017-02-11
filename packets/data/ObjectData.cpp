#include "ObjectData.h"
#include "../Packet.h"


ObjectData::ObjectData(Packet *p)
{
	// Just pass the packet to the Read function
	Read(p);
}
ObjectData::ObjectData(const ObjectData &od)
{
	this->objectType = od.objectType;
	this->status = od.status;
}

void ObjectData::Read(Packet *p)
{
	objectType = p->readBytes<ushort>();
	status.Read(p);
}

void ObjectData::Write(Packet *p)
{
	p->writeBytes<ushort>(objectType);
	status.Write(p);
}