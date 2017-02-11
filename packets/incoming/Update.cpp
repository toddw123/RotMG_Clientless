#include "Update.h"
#include "../PacketIOHelper.h"
#include "../PacketType.h"

// Constructor
Update::Update()
{
	this->id = PacketType::UPDATE;
}
Update::Update(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::UPDATE;
	Parse();
}
Update::Update(const Packet &p) : Packet(p)
{
	this->id = PacketType::UPDATE;
	Parse();
}

void Update::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	
	short i = 0; // used in for loops

	// Write the total GroundTiles
	this->writeBytes<short>(tiles.size());
	if (tiles.size() > 0)
	{
		for (i = 0; i < tiles.size(); i++)
		{
			tiles.at(i).Write(this);
		}
	}

	// Write the newObjs
	this->writeBytes<short>(newObjs.size());
	if (newObjs.size())
	{
		for (i = 0; i < newObjs.size(); i++)
		{
			newObjs.at(i).Write(this);
		}
	}

	// Write the drops
	this->writeBytes<short>(drops.size());
	if (drops.size())
	{
		for (i = 0; i < drops.size(); i++)
		{
			this->writeBytes<int>(drops.at(i));
		}
	}
	

	PacketIOHelper::SendPacket(this);
}

void Update::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);

	short i = 0; // used in for loops
	// Empty data just to be safe
	tiles.clear();
	newObjs.clear();
	drops.clear();

	// Read in GroundTile data
	short count = this->readBytes<short>();
	if (count > 0)
	{
		// Resize the vector just to be safe
		//tiles.resize(count);
		for (i = 0; i < count; i++)
		{
			// Pass the packet data to the ground tile so it can read the data it needs
			tiles.push_back(GroundTile(this));
		}
	}

	// Read in ObjectData data
	count = this->readBytes<short>();
	if (count > 0)
	{
		//newObjs.resize(count);
		for (i = 0; i < count; i++)
		{
			newObjs.push_back(ObjectData(this));
		}
	}

	// Read in the different objectId's dropped
	count = this->readBytes<short>();
	if (count > 0)
	{
		//drops.resize(count);
		for (i = 0; i < count; i++)
		{
			drops.push_back(this->readBytes<int>());
		}
	}
	// done!
}

void Update::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}