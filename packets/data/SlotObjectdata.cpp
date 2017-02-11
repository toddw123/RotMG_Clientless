#include "SlotObjectData.h"
#include "../Packet.h"


SlotObjectData::SlotObjectData(Packet *p)
{
	// Just pass the packet to the Read function
	Read(p);
}
SlotObjectData::SlotObjectData(const SlotObjectData &sod)
{
	this->objectId = sod.objectId;
	this->slotId = sod.slotId;
	this->objectType = sod.objectType;
}

void SlotObjectData::Read(Packet *p)
{
	objectId = p->readBytes<int>();
	slotId = p->readBytes<byte>();
	objectType = p->readBytes<int>();
}

void SlotObjectData::Write(Packet *p)
{
	p->writeBytes<int>(objectId);
	p->writeBytes<byte>(slotId);
	p->writeBytes<int>(objectType);
}