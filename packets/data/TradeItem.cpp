#include "TradeItem.h"
#include "../Packet.h"

TradeItem::TradeItem(Packet *p)
{
	// Just pass the packet to the Read function
	Read(p);
}
TradeItem::TradeItem(const TradeItem &ti)
{
	this->item = ti.item;
	this->slotType = ti.slotType;
	this->tradeable = ti.tradeable;
	this->included = ti.included;
}

void TradeItem::Read(Packet *p)
{
	item = p->readBytes<int>();
	slotType = p->readBytes<int>();
	tradeable = p->readBytes<bool>();
	included = p->readBytes<bool>();
}
void TradeItem::Write(Packet *p)
{
	p->writeBytes<int>(item);
	p->writeBytes<int>(slotType);
	p->writeBytes<bool>(tradeable);
	p->writeBytes<bool>(included);
}