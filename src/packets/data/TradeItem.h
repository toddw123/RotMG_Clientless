#pragma once

#ifndef TRADEITEM_H
#define TRADEITEM_H

class Packet;

class TradeItem
{
public:
	int item;
	int slotType;
	bool tradeable;
	bool included;

	TradeItem() {};
	TradeItem(Packet*);
	TradeItem(const TradeItem&);

	void Read(Packet*);
	void Write(Packet*);
};


#endif
