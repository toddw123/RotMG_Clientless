#include "StatData.h"
#include "../Packet.h"

StatData::StatData(Packet *p)
{
	// Just pass the packet to the Read function
	Read(p);
}
StatData::StatData(const StatData &sd)
{
	this->statType = sd.statType;
	this->statValue = sd.statValue;
	this->strStatValue = sd.strStatValue;
}

void StatData::Read(Packet *p)
{
	statType = p->readBytes<byte>();
	if (isStringStat())
	{
		strStatValue = p->readString<short>();
	}
	else
	{
		statValue = p->readBytes<int>();
	}
}

void StatData::Write(Packet *p)
{
	p->writeBytes<byte>(statType);
	if (isStringStat())
	{
		p->writeString<short>(strStatValue);
	}
	else
	{
		p->writeBytes<int>(statValue);
	}
}

bool StatData::isStringStat()
{
	switch (statType)
	{
		case NAME_STAT:
		case GUILD_NAME_STAT:
		case PET_NAME_STAT:
		case ACCOUNT_ID_STAT:
		case OWNER_ACCOUNT_ID_STAT:
			return true;
		default:
			return false;
	}
}