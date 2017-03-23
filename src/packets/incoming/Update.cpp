#include "Update.h"


// Constructor
Update::Update()
{
	this->type_ = PacketType::UPDATE;
}
Update::Update(byte *b, int i) : Packet(b, i)
{
	this->type_ = PacketType::UPDATE;
	read();
}
Update::Update(const Packet &p) : Packet(p)
{
	this->type_ = PacketType::UPDATE;
	read();
}

Packet *Update::write()
{
	// Clear the packet data just to be safe
	this->clearData();
	
	short i = 0; // used in for loops

	// Write the total GroundTiles
	this->writeBytes<short>((short)tiles.size());
	if ((short)tiles.size() > 0)
	{
		for (i = 0; i < (short)tiles.size(); i++)
		{
			tiles.at(i).Write(this);
		}
	}

	// Write the newObjs
	this->writeBytes<short>((short)newObjs.size());
	if ((short)newObjs.size())
	{
		for (i = 0; i < (short)newObjs.size(); i++)
		{
			newObjs.at(i).Write(this);
		}
	}

	// Write the drops
	this->writeBytes<short>((short)drops.size());
	if ((short)drops.size())
	{
		for (i = 0; i < (short)drops.size(); i++)
		{
			this->writeBytes<int>(drops.at(i));
		}
	}
	

	//PacketIOHelper::SendPacket(this);
	return this;
}

void Update::read()
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