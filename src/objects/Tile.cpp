#include "Tile.h"

// Constructor
Tile::Tile()
{

}
Tile::Tile(const Tile &obj)
{
	// TODO: copy Tile properties	
}
Tile::Tile(pugi::xml_node objNode)
{
	parseFromXML(objNode);
}

void Tile::parseFromXML(pugi::xml_node node)
{
	this->type = strtol(node.attribute("type").value(), NULL, 0);
	this->id = node.attribute("id").value();

	this->speed = node.child("Speed") ? strtof(node.child_value("Speed"), NULL) : 1.0f;

	this->noWalk = node.child("NoWalk") ? true : false;
	this->sink = node.child("Sink") ? true : false;
	this->push = node.child("Push") ? true : false;

	this->xOffset = node.child("XOffset") ? strtof(node.child_value("XOffset"), NULL) : 0.0f;
	this->yOffset = node.child("YOffset") ? strtof(node.child_value("YOffset"), NULL) : 0.0f;

	if (node.child("Animate"))
	{
		std::string ani = node.child_value("Animate");
		this->animate = (ani == "Flow") ? AnimateType::FLOW : AnimateType::NONE;
	}
	else
		this->animate = AnimateType::NONE;

	this->dx = node.child("Animate") ? strtof(node.child("Animate").attribute("dx").value(), NULL) : 0.0f;
	this->dy = node.child("Animate") ? strtof(node.child("Animate").attribute("dy").value(), NULL) : 0.0f;

	this->minDamage = node.child("MinDamage") ? atoi(node.child_value("Speed")) : 0;
	this->maxDamage = node.child("MaxDamage") ? atoi(node.child_value("Speed")) : 0;

	this->slideAmount = node.child("SlideAmount") ? strtof(node.child_value("SlideAmount"), NULL) : 0.0f;
}