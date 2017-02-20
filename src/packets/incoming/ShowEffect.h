#pragma once

#ifndef SHOWEFFECT_H
#define SHOWEFFECT_H

#include "../Packet.h"
#include "../data/WorldPosData.h"

enum EffectTypes : int
{
	UNKNOWN_EFFECT_TYPE = 0,
	HEAL_EFFECT_TYPE = 1,
	TELEPORT_EFFECT_TYPE = 2,
	STREAM_EFFECT_TYPE = 3,
	THROW_EFFECT_TYPE = 4,
	NOVA_EFFECT_TYPE = 5,
	POISON_EFFECT_TYPE = 6,
	LINE_EFFECT_TYPE = 7,
	BURST_EFFECT_TYPE = 8,
	FLOW_EFFECT_TYPE = 9,
	RING_EFFECT_TYPE = 10,
	LIGHTNING_EFFECT_TYPE = 11,
	COLLAPSE_EFFECT_TYPE = 12,
	CONEBLAST_EFFECT_TYPE = 13,
	JITTER_EFFECT_TYPE = 14,
	FLASH_EFFECT_TYPE = 15,
	THROW_PROJECTILE_EFFECT_TYPE = 16,
	SHOCKER_EFFECT_TYPE = 17,
	SHOCKEE_EFFECT_TYPE = 18,
	RISING_FURY_EFFECT_TYPE = 19
};

class ShowEffect : public Packet
{
public:
	byte effectType;
	int targetObjectId;
	WorldPosData pos1;
	WorldPosData pos2;
	int color;

	// Constructor
	ShowEffect();
	ShowEffect(byte*, int);
	ShowEffect(const Packet&);

	// Output
	Packet *write();
	// Input
	void read();
};

#endif