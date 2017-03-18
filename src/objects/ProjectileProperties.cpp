#include "ProjectileProperties.h"

// Constructor
ProjectileProperties::ProjectileProperties()
{

}
ProjectileProperties::ProjectileProperties(const ProjectileProperties &obj)
{
	this->effects.clear();

	this->bulletType = obj.bulletType;
	this->objectId = obj.objectId;

	this->lifetime = obj.lifetime;
	this->speed = obj.speed;
	this->size = obj.size;
	
	this->minDamage = obj.minDamage;
	this->maxDamage = obj.maxDamage;


	this->multiHit = obj.multiHit;
	this->passesCover = obj.passesCover;
	this->armorPiercing = obj.armorPiercing;
	this->particleTrail = obj.particleTrail;

	this->wavy = obj.wavy;
	this->parametric = obj.parametric;
	this->boomerang = obj.boomerang;

	this->amplitude = obj.amplitude;
	this->frequency = obj.frequency;
	this->magnitude = obj.magnitude;
}
ProjectileProperties::ProjectileProperties(pugi::xml_node objNode)
{
	parseFromXML(objNode);
}

void ProjectileProperties::parseFromXML(pugi::xml_node node)
{
	this->effects.clear();

	this->bulletType = atoi(node.attribute("id").value());
	this->objectId = node.child_value("ObjectId");

	this->lifetime = atoi(node.child_value("LifetimeMS"));
	this->speed = atoi(node.child_value("Speed"));
	this->size = node.child("Size") ? atoi(node.child_value("Size")) : -1;
	if (node.child("Damage"))
	{
		this->minDamage = this->maxDamage = atoi(node.child_value("Damage"));
	}
	else
	{
		this->minDamage = atoi(node.child_value("MinDamage"));
		this->maxDamage = atoi(node.child_value("MaxDamage"));
	}

	this->multiHit = node.child("MultiHit") ? true : false;
	this->passesCover = node.child("PassesCover") ? true : false;
	this->armorPiercing = node.child("ArmorPiercing") ? true : false;
	this->particleTrail = node.child("ParticleTrail") ? true : false;

	this->wavy = node.child("Wavy") ? true : false;
	this->parametric = node.child("Parametric") ? true : false;
	this->boomerang = node.child("Boomerang") ? true : false;

	this->amplitude = node.child("Amplitude") ? strtof(node.child_value("Amplitude"), NULL) : 0.0f;
	this->frequency = node.child("Frequency") ? strtof(node.child_value("Frequency"), NULL) : 1.0f;
	this->magnitude = node.child("Magnitude") ? strtof(node.child_value("Magnitude"), NULL) : 3.0f;


	// TODO: CONDITION EFFECTS
}