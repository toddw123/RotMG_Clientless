#pragma once

#ifndef PICPACKET_H
#define PICPACKET_H

#include "../Packet.h"

class PicPacket : public Packet
{
public:
/* This is from the client's Pic class for reference
	public var bitmapData_:BitmapData = null;
...
	override public function parseFromInput(param1:IDataInput) : void
	{
		var _loc2_:int = param1.readInt();
		var _loc3_:int = param1.readInt();
		var _loc4_:ByteArray = new ByteArray();
		param1.readBytes(_loc4_,0,_loc2_ * _loc3_ * 4);
		this.bitmapData_ = new BitmapDataSpy(_loc2_,_loc3_);
		this.bitmapData_.setPixels(this.bitmapData_.rect,_loc4_);
	}
*/
	int width;
	int height;
	std::vector<byte> bitmapData;

	// Constructor
	PicPacket();
	PicPacket(byte*, int);
	PicPacket(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif