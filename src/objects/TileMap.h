#pragma once

#ifndef TILEMAP_H
#define TILEMAP_H

#include "../micropather/micropather.h"
#include "ObjectLibrary.h"

class TileMap : public micropather::Graph
{
private:
	int* mapTile;
	int mapWidth, mapHeight;

	micropather::MicroPather* pather;

public:
	TileMap() : pather(0), mapWidth(0), mapHeight(0)
	{
		pather = new micropather::MicroPather(this, 256);	// Use a very small memory block to stress the pather
	}

	virtual ~TileMap() {
		delete pather;
	}

	void createMap(int w, int h)
	{
		mapWidth = w;
		mapHeight = h;
		mapTile = new int[mapWidth * mapHeight + 1];
		for (int i = 0; i < mapWidth * mapHeight + 1; i++)
		{
			mapTile[i] = 0;
		}
	}
	void updateTile(int x, int y, int t)
	{
		mapTile[y * mapWidth + x] = t;
	}

	unsigned Checksum() { return pather->Checksum(); }


	bool canWalk(int nx, int ny)
	{
		if (nx < 0 || nx > mapWidth || ny < 0 || ny > mapHeight)
			return false;

		int index = ny * mapWidth + nx;
		int t = mapTile[index];
		if (mapTile[index] == 0)
		{
			return false;
		}

		Tile* tile = ObjectLibrary::getTilePtr(t);
		if (tile == NULL)
		{
			return false;
		}
		if (tile->noWalk)
		{
			return false;
		}
		if (tile->sink)
		{
			return false;
		}
		if (tile->speed < 1.0)
		{
			return false;
		}

		return true;
	}

	bool createPath(int startx, int starty, int endx, int endy, std::vector<void*>* path)
	{
		if (!canWalk(startx, starty))
			return false;

		void* start = XYToNode(startx, starty);
		void* end = XYToNode(endx, endy);
		float tmpcost;
		int result = pather->Solve(start, end, path, &tmpcost);

		if (result == micropather::MicroPather::SOLVED)
		{
			/*for (std::vector<void*>::iterator itr = path.begin(); itr != path.end(); ++itr)
			{
				//PrintStateInfo(*itr);
				int tmpx, tmpy;
				NodeToXY(*itr, &tmpx, &tmpy);
				printf("X,Y: %d,%d\n", tmpx, tmpy);
			}*/
			return true;
		}
		return false;
	}

	void NodeToXY(void* node, int* x, int* y)
	{
		int index = (int)node;
		*y = index / mapWidth;
		*x = index - *y * mapWidth;
	}

	void* XYToNode(int x, int y)
	{
		return (void*)(y * mapWidth + x);
	}


	virtual float LeastCostEstimate(void* nodeStart, void* nodeEnd)
	{
		int xStart, yStart, xEnd, yEnd;
		NodeToXY(nodeStart, &xStart, &yStart);
		NodeToXY(nodeEnd, &xEnd, &yEnd);
		int dx = xStart - xEnd;
		int dy = yStart - yEnd;
		return (dx * dx + dy * dy);
	}

#define QUICK_NODE_ADD(nx, ny)\
    if (canWalk(nx, ny))\
    {\
    micropather::StateCost s;\
    s.state = XYToNode(nx, ny);\
    s.cost = LeastCostEstimate(node, s.state);\
    n->push_back(s);\
    }

	virtual void AdjacentCost(void* node, std::vector< micropather::StateCost > *n)
	{
		int x, y;
		NodeToXY(node, &x, &y);

		QUICK_NODE_ADD(x, y + 1);
		QUICK_NODE_ADD(x, y - 1);
		QUICK_NODE_ADD(x - 1, y);
		QUICK_NODE_ADD(x + 1, y);
	}

	virtual void PrintStateInfo(void* node)
	{
		int x, y;
		NodeToXY(node, &x, &y);
		printf("(%d,%d)", x, y);
	}
};

#endif