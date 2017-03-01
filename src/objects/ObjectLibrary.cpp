#include "ObjectLibrary.h"

std::unordered_map<int, Object> ObjectLibrary::objects;
bool ObjectLibrary::loaded = false;

void ObjectLibrary::loadLibrary()
{
	// Dont want to load it more then once by accident
	if (!loaded)
	{
		// Parse all the objects in objects.xml
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file("resources/objects.xml");
		// Make sure it parsed the file correctly
		if (!result)
		{
			printf("Error parsing objects.xml!\nError description: %s\nError offset: %i\n", result.description(), result.offset);
			return;
		}

		pugi::xml_node mainNode = doc.child("Objects");
		for (pugi::xml_node objNode = mainNode.child("Object"); objNode; objNode = objNode.next_sibling("Object"))
		{
			// Might as well make sure it has a type
			if (objNode.attribute("type"))
			{
				int type = strtol(objNode.attribute("type").value(), NULL, 0);
				objects[type] = Object(objNode);
				//objects.insert(std::make_pair(type, Object(objNode))); -- no idea why this doesnt work, oh well
			}
		}
		loaded = true;
	}
}

Object* ObjectLibrary::getObject(int type)
{
	if (objects.find(type) != objects.end())
	{
		return &objects[type];
	}
	return nullptr;
}

Object* ObjectLibrary::getObjectByName(std::string str)
{
	for (std::unordered_map<int, Object>::iterator it = objects.begin(); it != objects.end(); ++it)
	{
		if (it->second.id == str)
			return &objects[it->first];
	}
	return nullptr;
}