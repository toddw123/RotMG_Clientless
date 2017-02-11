#include "ObjectStatusData.h"
#include "../Packet.h"


ObjectStatusData::ObjectStatusData(Packet *p)
{
	// Just pass the packet to the Read function
	Read(p);
}
ObjectStatusData::ObjectStatusData(const ObjectStatusData &osd)
{
	this->objectId = osd.objectId;
	this->pos = osd.pos;

	// Make sure stats is clear
	this->stats.clear();
	std::copy(osd.stats.begin(), osd.stats.end(), std::back_inserter(this->stats));
}

void ObjectStatusData::Read(Packet *p)
{
	objectId = p->readBytes<int>();
	// Read WorldPosData (i could probably just use a POINT but fuck it)
	pos.Read(p);
	// Read StatData
	short count = p->readBytes<short>();
	stats.clear();
	if (count > 0)
	{
		//stats.resize(count);
		for (short i = 0; i < count; i++)
		{
			stats.push_back(StatData(p));
		}
	}
}

void ObjectStatusData::Write(Packet *p)
{
	// Write objectId
	p->writeBytes<int>(objectId);
	// Pass packet pointer to WorldPosData for writing
	pos.Write(p);
	// Write the number of StatData
	p->writeBytes<short>((short)stats.size());
	if (stats.size() > 0)
	{
		for (short i = 0; i < (short)stats.size(); i++)
		{
			// Pass the packet pointer to each StatData for writing
			stats.at(i).Write(p);
		}
	}
}