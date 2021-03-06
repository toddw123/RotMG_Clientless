#include "Tile.h"

// Constructor
Tile::Tile()
{
	this->speed = 1.0;
	this->xOffset = 0.0;
	this->yOffset = 0.0;
	this->dx = 0.0;
	this->dy = 0.0;
	this->minDamage = 0;
	this->maxDamage = 0;
	this->slideAmount = 0.0;
}

Tile::Tile(const Tile &obj)
{
	// TODO: copy Tile properties	
	this->animate = obj.animate;
	this->dx = obj.dx;
	this->dy = obj.dy;
	this->id = obj.id;
	this->maxDamage = obj.maxDamage;
	this->minDamage = obj.minDamage;
	this->noWalk = obj.noWalk;
	this->push = obj.push;
	this->sink = obj.sink;
	this->slideAmount = obj.slideAmount;
	this->speed = obj.speed;
	this->type = obj.type;
	this->xOffset = obj.xOffset;
	this->yOffset = obj.yOffset;
}
Tile::Tile(pugi::xml_node objNode)
{
	parseFromXML(objNode);
}

void Tile::parseFromXML(pugi::xml_node node)
{
	this->type = strtol(node.attribute("type").value(), NULL, 0);
	this->id = node.attribute("id").value();

	this->speed = node.child("Speed") ? strtod(node.child_value("Speed"), NULL) : 1.0;

	this->noWalk = node.child("NoWalk") ? true : false;
	this->sink = node.child("Sink") ? true : false;
	this->push = node.child("Push") ? true : false;

	this->xOffset = node.child("XOffset") ? strtod(node.child_value("XOffset"), NULL) : 0.0;
	this->yOffset = node.child("YOffset") ? strtod(node.child_value("YOffset"), NULL) : 0.0;

	if (node.child("Animate"))
	{
		std::string ani = node.child_value("Animate");
		this->animate = (ani == "Flow") ? AnimateType::FLOW : AnimateType::NONE;
	}
	else
		this->animate = AnimateType::NONE;

	this->dx = node.child("Animate") ? strtod(node.child("Animate").attribute("dx").value(), NULL) : 0.0;
	this->dy = node.child("Animate") ? strtod(node.child("Animate").attribute("dy").value(), NULL) : 0.0;

	this->minDamage = node.child("MinDamage") ? atoi(node.child_value("Speed")) : 0;
	this->maxDamage = node.child("MaxDamage") ? atoi(node.child_value("Speed")) : 0;

	this->slideAmount = node.child("SlideAmount") ? strtod(node.child_value("SlideAmount"), NULL) : 0.0;
}