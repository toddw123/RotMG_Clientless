#pragma once

#ifndef TILEMAP_H
#define TILEMAP_H

#include "../micropather/micropather.h"
#include "ObjectLibrary.h"



class TileMap : public micropather::Graph
{
private:
	std::unordered_map<int, int> mapTile;
	std::unordered_map<int, int> objMap;
	int mapWidth, mapHeight;

	micropather::MicroPather* pather;

public:
	TileMap() : pather(0), mapWidth(0), mapHeight(0)
	{
		pather = new micropather::MicroPather(this, 10);
	}

	virtual ~TileMap()
	{
		mapTile.clear();
		objMap.clear();
		delete pather;
	}

	void createMap(int w, int h)
	{
		mapTile.clear();
		objMap.clear();

		mapWidth = w;
		mapHeight = h;
		for (int i = 0; i < mapWidth * mapHeight + 1; i++)
		{
			mapTile[i] = -1;
			objMap[i] = 0;
		}
	}
	void updateTile(int x, int y, int t)
	{
		mapTile[y * mapWidth + x] = t;
	}
	void addObject(int x, int y, int o)
	{
		objMap[y * mapWidth + x] = o;
	}

	unsigned Checksum() { return pather->Checksum(); }


	bool canWalk(int nx, int ny)
	{
		if (nx < 0 || nx > mapWidth || ny < 0 || ny > mapHeight)
			return false;

		int index = ny * mapWidth + nx;
		int t = mapTile[index];

		if (mapTile[index] == -1)
			return true;
		if (objMap[index] != 0)
			return false;

		Tile* tile = ObjectLibrary::getTilePtr(t);
		if (tile == NULL)
		{
			return false;
		}
		if (tile->noWalk)
		{
			return false;
		}
		/*if (tile->sink)
		{
		return false;
		}*/

		return true;
	}

	/*double distance(int ax, int ay, int bx, int by)
	{
	double x = ax - bx;
	double y = ay - by;
	return sqrt(x * x + y * y);
	}
	bool onLine(int ax, int ay, int bx, int by, int cx, int cy)
	{
	return distance(ax, ay, cx, cy) + distance(cx, cy, bx, by) == distance(ax, ay, bx, by);
	}*/
	bool inLine(int ax, int ay, int bx, int by, int cx, int cy)
	{
		if (ax == cx) return bx == cx;
		if (ay == cy) return by == cy;
		return ((cx - ax) * (bx - ax)) == ((cy - ay) * (by - ay));
	}

	bool createPath(int startx, int starty, int endx, int endy, std::vector<void*>* path)
	{
		if (!canWalk(startx, starty))
			return false;

		void* start = XYToNode(startx, starty);
		void* end = XYToNode(endx, endy);
		std::vector<void*> tmppath;
		float tmpcost;
		int result = pather->Solve(start, end, &tmppath, &tmpcost);

		if (result == micropather::MicroPather::SOLVED)
		{
			// First, remove any nodes that are outside visible bounds, we will walk all the way to the edge of visiblity
			for (int i = 0; i < tmppath.size(); i++)
			{
				int _x, _y;
				NodeToXY(tmppath[i], &_x, &_y);
				if (mapTile[_y * mapWidth + _x] == -1 && i > 1)
				{
					tmppath.resize((i - 1));
					break;
				}
			}
			// Attempt to remove any nodes that are within a straight line of eachother
			for (int i = 0; i < tmppath.size() - 1; )
			{
				path->push_back(tmppath[i]);
				int startx, starty;
				NodeToXY(tmppath[i], &startx, &starty);
				int nx, ny;
				NodeToXY(tmppath[(i + 1)], &nx, &ny);
				if (i + 2 < tmppath.size() - 1)
				{
					for (int ii = i + 2; ii < tmppath.size(); ii++)
					{
						int ex, ey;
						NodeToXY(tmppath[ii], &ex, &ey);
						if (!inLine(startx, starty, nx, ny, ex, ey))
						{
							i = ii - 1;
							break;
						}
						if (ii == tmppath.size() - 1)
						{
							i = ii;
						}
					}
				}
				else
				{
					break;
				}
			}
			path->push_back(tmppath.back());
			return true;
		}
		return false;
	}

	void NodeToXY(void* node, int* x, int* y)
	{
		long index = (long)node;
		*y = index / mapWidth;
		*x = index - *y * mapWidth;
	}

	void* XYToNode(long x, long y)
	{
		return (void*)(y * mapWidth + x);
	}


	virtual float LeastCostEstimate(void* nodeStart, void* nodeEnd)
	{
		//int xStart, yStart, xEnd, yEnd;
		//NodeToXY(nodeStart, &xStart, &yStart);
		//NodeToXY(nodeEnd, &xEnd, &yEnd);
		Tile* tile = ObjectLibrary::getTilePtr(mapTile[(long)nodeEnd]);
		if (tile == NULL) return 2;

		return (float)(2.0 - tile->speed);
		//int dx = xStart - xEnd;
		//int dy = yStart - yEnd;
		//return (dx * dx + dy * dy);
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

		if (canWalk(x, y + 1))
		{
			if (canWalk(x + 1, y) && canWalk(x + 1, y + 1))
				QUICK_NODE_ADD(x + 1, y + 1);
			if (canWalk(x - 1, y) && canWalk(x - 1, y + 1))
				QUICK_NODE_ADD(x - 1, y + 1);
		}
		if (canWalk(x, y - 1))
		{
			if (canWalk(x + 1, y) && canWalk(x + 1, y - 1))
				QUICK_NODE_ADD(x + 1, y - 1);
			if (canWalk(x - 1, y) && canWalk(x - 1, y - 1))
				QUICK_NODE_ADD(x - 1, y - 1);
		}
	}

	virtual void PrintStateInfo(void* node)
	{
		int x, y;
		NodeToXY(node, &x, &y);
		printf("(%d,%d)", x, y);
	}
};

#endif