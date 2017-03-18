#pragma once

#ifndef PROJECTILEPROPERTIES_H
#define PROJECTILEPROPERTIES_H

#include <string>
#include <vector>
#include "../pugixml/pugixml.hpp"

typedef unsigned int uint;

class ProjectileProperties
{
public:
	int bulletType;
	std::string objectId;
	int lifetime;
	int speed;
	int size;
	int minDamage;
	int maxDamage;
	std::vector<uint> effects;
	bool multiHit;
	bool passesCover;
	bool armorPiercing;
	bool particleTrail;
	bool wavy;
	bool parametric;
	bool boomerang;
	float amplitude;
	float frequency;
	float magnitude;

	ProjectileProperties();
	ProjectileProperties(const ProjectileProperties&);
	ProjectileProperties(pugi::xml_node);

	void parseFromXML(pugi::xml_node);
};

#endif

/*
      public var particleTrailIntensity_:int = -1;
      public var particleTrailLifetimeMS:int = -1;
      public var particleTrailColor_:int = 16711935;
	  public var isPetEffect_:Dictionary;
*/