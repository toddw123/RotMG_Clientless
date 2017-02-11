#include "MapInfo.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"

// Constructors
MapInfo::MapInfo()
{
	this->id = PacketType::MAPINFO;
}
MapInfo::MapInfo(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::MAPINFO;
	Parse();
}
MapInfo::MapInfo(const Packet &p) : Packet(p)
{
	this->id = PacketType::MAPINFO;
	Parse();
}

void MapInfo::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write the data from MapInfo to the packet
	this->writeBytes<int>(width);
	this->writeBytes<int>(height);
	this->writeString<short>(name);
	this->writeString<short>(displayName);
	this->writeBytes<uint>(fp);
	this->writeBytes<int>(background);
	this->writeBytes<int>(difficulty);
	this->writeBytes<bool>(allowPlayerTeleport);
	this->writeBytes<bool>(showDisplays);
	// Write xml vector<string> data
	this->writeBytes<short>(clientXML.size());
	if (clientXML.size() > 0)
	{
		for (short i = 0; i < clientXML.size(); i++)
		{
			this->writeString<int>(clientXML.at(i));
		}
	}
	this->writeBytes<short>(extraXML.size());
	if (extraXML.size() > 0)
	{
		for (short i = 0; i < extraXML.size(); i++)
		{
			this->writeString<int>(extraXML.at(i));
		}
	}

	PacketIOHelper::SendPacket(this);
}

void MapInfo::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read in the data
	width               = this->readBytes<int>();
	height              = this->readBytes<int>();
	name                = this->readString<short>();
	displayName         = this->readString<short>();
	fp                  = this->readBytes<uint>();
	background          = this->readBytes<int>();
	difficulty          = this->readBytes<int>();
	allowPlayerTeleport = this->readBytes<bool>();
	showDisplays        = this->readBytes<bool>();
	// Read in the vector<string> data
	short count = this->readBytes<short>();
	clientXML.clear();
	if (count > 0)
	{
		// Make sure clientXML is big enough
		//clientXML.resize(count); -- seems to fuck things up
		for (short i = 0; i < count; i++)
		{
			// Push back the new string, these strings use int for length
			clientXML.push_back(this->readString<int>());
		}
	}
	// Repeat this same process for extraXML
	count = this->readBytes<short>();
	extraXML.clear();
	if (count > 0)
	{
		//extraXML.resize(count); -- seems to fuck things up
		for (short i = 0; i < count; i++)
		{
			extraXML.push_back(this->readString<int>());
		}
	}
	// done!
}

void MapInfo::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}