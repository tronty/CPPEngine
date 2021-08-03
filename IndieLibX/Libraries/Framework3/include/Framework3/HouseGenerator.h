#ifndef __HouseGenerator___
#define __HouseGenerator___

// http://www.polygonpi.com/files/housegenerator.h
// http://www.polygonpi.com/files/housegenerator.cpp

/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

//	INCLUDES	--------------------------------------------------------------
#include <queue>


class HouseGenerator
{
public:
HouseGenerator(const int width, const int height)
: m_width(width)
, m_height(height)
, m_data(NULL)
, m_chunks()
, m_halls()
, m_unconnectedRooms()
, m_totalHallArea(0)
{
	MaxHallRate = 0.15f;
	HallDoorProb = 0.6f;
	RoomDoorProb = 0.4f;
	MaxRoomSize = 15;
	m_data = (TileType::Enum*)malloc(m_width * m_height * sizeof(int));
}


~HouseGenerator()
{
	if (m_data)
	{
		free(m_data);
	}
}

void render()
{
}

void generate()
{
	// Fill it with wall, carve it out.
	for (int i = 0; i < m_width * m_height; i++)
	{
		m_data[i] = TileType::Wall;
	}

	// Push the first interior area.

	// 10 is the smallest hall split width: ---|--|---
	bool firstDir = m_width < 10 ? false : m_height < 10 ? true : RandBool();
	m_chunks.push(AreaAndDir(Area(1, m_width - 2, 1, m_height - 2), firstDir));

	chunksToRooms();
	connectHalls();
	connectRooms();
}
	struct TileType
	{
		enum Enum
		{
			Wall,
			Hall,
			Room,
			Door,
			MAX
		};
	};
	
private:

	class Area
	{
	public:
		Area(int l_in, int r_in, int b_in, int t_in)
		: l(l_in)
		, r(r_in)
		, b(b_in)
		, t(t_in)
		{}

		Area()
		: l(0), r(0), b(0), t(0)
		{}

		int width() const { return r - l + 1; }
		int height() const { return t - b + 1; }
		int area() const { return width() * height(); }
		bool containsPoint(const int x, const int y) const { return x >= l && x <= r && y >=b && y <= t; }

		int l, r, b, t;
	};

TileType::Enum getTileType(const int x, const int y)
{
	LOG_FNLN;
	// Check for bounds here for simpler calling code.
	if (x >= 0 && x < m_width && y >= 0 && y < m_height)
	{
		return m_data[y*m_width + x];
	}
	else
	{
		// Out of bounds, invalid.
		return TileType::MAX;
	}
}

void carveArea(const Area& area, const TileType::Enum type)
{
	LOG_FNLN;
	for (int j = area.b; j <= area.t; j++)
	{
		for (int i = area.l; i <= area.r; i++)
		{
			setTileType(i, j, type);
		}
	}
}


void setTileType(const int x, const int y, const TileType::Enum type)
{
	LOG_FNLN;
	m_data[y*m_width + x] = type;
}


void chunksToRooms()
{
	LOG_FNLN;
	while (!m_chunks.empty())
	{
		AreaAndDir chunk = m_chunks.front();
		m_chunks.pop();

		// Attempt to split as a hall, alternating the cut direction. If not
		// possible, don't create any more halls.
		if (!splitChunk(chunk.m_area, chunk.m_directionHorizontal))
		{
			// Otherwise leave them as rooms.
			m_unconnectedRooms.push_back(chunk.m_area);

			carveArea(chunk.m_area, TileType::Room);
		}
	}
}


bool splitChunk(const Area& chunk, const bool splitHorizontal)
{
	LOG_FNLN;
	// Split in three while there isn't enough hall.
	int hallThreshold = (int)(m_width * m_height * MaxHallRate);

	Area chunkA, chunkB, hall;
	if (m_totalHallArea < hallThreshold && splitWithHall(chunk, splitHorizontal, chunkA, hall, chunkB))
	{
		// Successfully split into more hall.
		m_chunks.push(AreaAndDir(chunkA, !splitHorizontal));
		m_chunks.push(AreaAndDir(chunkB, !splitHorizontal));
		m_halls.push(hall);

		m_totalHallArea += hall.area();

		carveArea(hall, TileType::Hall);
		return true;
	}
	else if (chunk.width() > MaxRoomSize || chunk.height() > MaxRoomSize)
	{
		// Split into two rooms.
		Area chunkA, chunkB;
		splitToRooms(chunk, chunkA, chunkB);

		// Don't reverse the split direction as this could create more halls, possibly
		// resulting in a disconnected final layout.
		m_chunks.push(AreaAndDir(chunkA, splitHorizontal));
		m_chunks.push(AreaAndDir(chunkB, splitHorizontal));
		return true;
	}
	else
	{
		return false;
	}
}


bool splitWithHall(const Area& chunk, const bool splitHorizontal, Area& chunkA, Area& hall, Area& chunkB)
{
	LOG_FNLN;
	// Check if we can perform the split.
	int width = chunk.width();
	int height = chunk.height();

	if ((splitHorizontal && width < 10) || (!splitHorizontal && height < 10))
	{
		return false;
	}

	int splitLength = (splitHorizontal ? width : height) - 10;
	int splitPos = RandMinMax(0, splitLength + 1);

	// Space out the two chunks with a 2-width hallway in between.
	if (splitHorizontal)
	{
		chunkA = hall = chunkB = chunk;

		chunkA.r = chunk.l + 2 + splitPos;

		hall.l = chunkA.r + 2;
		hall.r = hall.l + 1;

		chunkB.l = hall.r + 2;
	}
	else
	{
		chunkA = hall = chunkB = chunk;

		chunkA.t = chunk.b + 2 + splitPos;

		hall.b = chunkA.t + 2;
		hall.t = hall.b + 1;

		chunkB.b = hall.t + 2;
	}

	return true;
}


void splitToRooms(const Area& chunk, Area& chunkA, Area& chunkB)
{
	LOG_FNLN;
	// Split horizontally or vertically.
	int width = chunk.width();
	int height = chunk.height();

	// 7 is the smallest width, rooms are at least three wide: ---|---
	bool horizSplit = height < 7 || (width >= 7 && RandBool());

	int splitLength = (horizSplit ? width : height) - 7;
	int splitPos = RandMinMax(0, splitLength + 1);

	if (horizSplit)
	{
		chunkA = chunkB = chunk;

		chunkA.r = chunk.l + 2 + splitPos;
		chunkB.l = chunkA.r + 2;
	}
	else
	{
		chunkA = chunkB = chunk;

		chunkA.t = chunk.b + 2 + splitPos;
		chunkB.b = chunkA.t + 2;
	}
}


void connectHalls()
{
	LOG_FNLN;
	while (!m_halls.empty())
	{
		Area hall = m_halls.front();
		m_halls.pop();

		// Check if it ends into another hall.
		bool horiz = hall.width() > hall.height() || ((hall.width() == hall.height()) && RandBool());

		if (horiz)
		{
			carveIfConnected(hall.l - 1, hall.b, 1 + hall.t - hall.b, false);
			carveIfConnected(hall.r + 1, hall.b, 1 + hall.t - hall.b, false);
		}
		else
		{
			carveIfConnected(hall.l, hall.b - 1, 1 + hall.r - hall.l, true);
			carveIfConnected(hall.l, hall.t + 1, 1 + hall.r - hall.l, true);
		}
	}
}


void carveIfConnected(const int x, const int y, const int size, const bool horizontal)
{
	LOG_FNLN;
	bool valid = true;

	for (int i = 0; i < size; i++)
	{
		// Check both sides of the wall.
		int checkX1 = horizontal ? x + i : x + 1;
		int checkY1 = horizontal ? y + 1 : y + i;
		int checkX2 = horizontal ? x + i : x - 1;
		int checkY2 = horizontal ? y - 1 : y + i;

		if (getTileType(checkX1, checkY1) != TileType::Hall || getTileType(checkX2, checkY2) != TileType::Hall)
		{
			valid = false;
			break;
		}
	}

	if (valid)
	{
		Area cut(x, horizontal ? x + size - 1 : x, y, horizontal ? y : y + size - 1);
		carveArea(cut, TileType::Hall);
	}
}


void connectRooms()
{
	LOG_FNLN;
	while (!m_unconnectedRooms.empty())
	{
		Area room = m_unconnectedRooms.front();
		m_unconnectedRooms.pop_front();

		// Try to connect to one or more halls to become connected.
		if (!connectRoomToHall(room))
		{
			// Try to connect to one or more connected rooms to become connected.
			if (!connectRoomToRoom(room))
			{
				// Put it back and try again later.
				m_unconnectedRooms.push_back(room);
			}
		}
	}
}


bool connectRoomToHall(const Area& room)
{
	LOG_FNLN;
	// See if it borders a hall.
	bool ret = false;
	int side = RandMinMax(0, 4);

	for (int i = 0; i < 4; i++)
	{
		int wall = (side + i) % 4;

		// If it's already connected to a hallway, allow it to connect to a
		// connected room (more interesting for 1xN and 2xN blocks of rooms).
		if (connectWall(room, i, TileType::Hall, false) ||
			(ret && connectWall(room, i, TileType::Room, true)))
		{
			ret = true;

			// Allow multiple doors sometimes.
			if (Randf() > HallDoorProb)
			{
				break;
			}
		}
	}

	return ret;
}


bool connectRoomToRoom(const Area& room)
{
	LOG_FNLN;
	// See if it borders a connected room.
	bool ret = false;
	int side = RandMinMax(0, 4);

	for (int i = 0; i < 4; i++)
	{
		int wall = (side + i) % 4;

		if (connectWall(room, i, TileType::Room, true))
		{
			ret = true;

			// Allow multiple doors sometimes.
			if (Randf() > RoomDoorProb)
			{
				break;
			}
		}
	}

	return ret;
}


bool connectWall(const Area& room, int wall, TileType::Enum connectType, const bool connectedOnly)
{
	LOG_FNLN;
	int xStart = 0;
	int yStart = 0;
	int xDir = 0;
	int yDir = 0;
	int xLook = 0;
	int yLook = 0;

	switch (wall)
	{
		case 0:
			xStart = room.l;
			yStart = room.b;
			yDir = 1;
			xLook = -1;
			break;
		case 1:
			xStart = room.l;
			yStart = room.b;
			xDir = 1;
			yLook = -1;
			break;
		case 2:
			xStart = room.r;
			yStart = room.b;
			yDir = 1;
			xLook = 1;
			break;
		case 3:
			xStart = room.l;
			yStart = room.t;
			xDir = 1;
			yLook = 1;
			break;
	}

	// Doors must be two wide.
	int space = (xDir ? room.width() : room.height()) - 1;
	int doorIdx = RandMinMax(0, space);

	// Check each possible space.
	for (int i = 0; i < space; i++)
	{
		int doorOffset = (i + doorIdx) % space;
		int xPos1 = xStart + xLook + doorOffset*xDir;
		int yPos1 = yStart + yLook + doorOffset*yDir;
		int xPos2 = xStart + xLook + (doorOffset+1)*xDir;
		int yPos2 = yStart + yLook + (doorOffset+1)*yDir;

		int xLook1 = xPos1 + xLook;
		int yLook1 = yPos1 + yLook;
		int xLook2 = xPos2 + xLook;
		int yLook2 = yPos2 + yLook;

		// See if it opens onto the right space.
		if (getTileType(xLook1, yLook1) == connectType && getTileType(xLook2, yLook2) == connectType)
		{
			// Check it's not in the disconnected list if required.	Both will be
			// in the same room so only check one.
			if (!connectedOnly || !tileIsInDisconnectedRoom(xLook1, yLook1))
			{
				// Valid door.
				setTileType(xPos1, yPos1, TileType::Door);
				setTileType(xPos2, yPos2, TileType::Door);

				return true;
			}
		}
	}

	return false;
}


bool tileIsInDisconnectedRoom(const int x, const int y)
{
	LOG_FNLN;
	std::deque<Area>::iterator iter = m_unconnectedRooms.begin();
	while(iter != m_unconnectedRooms.end())
	{
		if (iter->containsPoint(x, y))
		{
			return true;
		}
		iter++;
	}

	return false;
}

int RandMinMax(int min, int max) //Pass in range
{
	int random = min;
	if(max)
		random = stx_rand() % max + min;
	return random;
}

bool RandBool()
{
	int r=RandMinMax(0, 1);
	return (r?true:false);
}

float Randf()
{
	return 1.0f*RandMinMax(0, 1);
}

	float MaxHallRate;
	float HallDoorProb;
	float RoomDoorProb;
	int MaxRoomSize;

	int		m_width;
	int		m_height;
	TileType::Enum*	m_data;

	struct AreaAndDir
	{
		AreaAndDir(const Area area, const bool directionHorizontal)
		: m_area(area)
		, m_directionHorizontal(directionHorizontal)
		{}

		Area				m_area;
		bool				m_directionHorizontal;
	};

	std::queue<AreaAndDir>	m_chunks;
	std::queue<Area>		m_halls;
	std::deque<Area>		m_unconnectedRooms;

	int						m_totalHallArea;
};
#endif

