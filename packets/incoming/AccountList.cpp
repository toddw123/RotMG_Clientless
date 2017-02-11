#include "AccountList.h"
#include "..\PacketIOHelper.h"
#include "..\PacketType.h"


// Constructors
AccountList::AccountList()
{
	this->id = PacketType::ACCOUNTLIST;
}
AccountList::AccountList(byte *b, int i) : Packet(b, i)
{
	this->id = PacketType::ACCOUNTLIST;
	Parse();
}
AccountList::AccountList(const Packet &p) : Packet(p)
{
	this->id = PacketType::ACCOUNTLIST;
	Parse();
}

void AccountList::Send()
{
	// Clear the packet data just to be safe
	this->clearData();
	// Write
	this->writeBytes<int>(accountListId);
	this->writeBytes<short>(accountIds.size());
	if (accountIds.size() > 0)
	{
		for (short i = 0; i < accountIds.size(); i++)
		{
			this->writeString<short>(accountIds.at(i));
		}
	}
	this->writeBytes<int>(lockAction);

	PacketIOHelper::SendPacket(this);
}

void AccountList::Parse()
{
	// Make sure the index is set to 0
	this->setIndex(0);
	// Read
	accountListId = this->readBytes<int>();
	
	short count = this->readBytes<short>();
	accountIds.clear();
	if (count > 0)
	{
		for (short i = 0; i < count; i++)
		{
			accountIds.push_back(this->readString<short>());
		}
	}

	lockAction = this->readBytes<int>();
	// done!
}

void AccountList::Fill(byte *bytes, int len)
{
	// Clear the packet data just to be safe
	this->clearData();
	// Take the raw data and fill in our packet.data vector
	this->setData(bytes, len);
	// Parse
	Parse();
}