#ifndef GENERATED_MAP_HPP_
#define GENERATED_MAP_HPP_

// http://trlewis.net/generating-a-dungeon-part-1/
// http://trlewis.net/generating-a-dungeon-part-2/
// http://trlewis.net/generating-a-dungeon-part-3/

/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <iostream> //cout, endl
#include <string>
#include <queue>
#include <time.h> //time()
#include <algorithm>
#include <vector>

#include <Framework3/IRenderer.h>

class Room
{
	//y axis goes top to bottom, higher values are further down
	//x axis goes left to right, pretty standard
public:
	/*! The first cell of the room that is walked into from the prev room */
	Vector2i startingCell; 
	/*! The exit cell from the previous room */
	Vector2i previousCell; 

	/*! Top left corner of the room, defines boundary/size.*/
	Vector2i topLeft;
	/*! Bottom right corner of the room, defines boundary/size.*/
	Vector2i bottomRight;

	bool isStarting; //!< if this room is the "start" room of the map
	bool isEnding; //!< if this room is the "end" room of the map
	bool branchesCreatd; //!< if the rooms this room goes to have been created

	// Delete these after map generation is finished... memory leaks yo.
	Room* left; //!< The room to the left of this one.
	Room* right; //!< The room to the right of this one.
	Room* bottom; //!< The room below this one.
	Room* top; //!< The room above this one.



Room(Vector2i start)
{
	init();
	startingCell = start;
}


Room(Vector2i topLeft, Vector2i bottomRight)
{
	init();
	this->topLeft = topLeft;
	this->bottomRight = bottomRight;
}

Room(Vector2i topLeft, Vector2i bottomRight, Vector2i start)
{
	init();
	this->topLeft = topLeft;
	this->bottomRight = bottomRight;
	startingCell = start;
}

bool isDeadEnd()
{
	// is the same as (generally... depending on what NULL is defined as)
	// left == NULL
	return !left && !right && !bottom && !top;
}

bool containsCell(Vector2i &cell)
{
	if (topLeft.x >= cell.x && bottomRight.x <= cell.y)
	{
		if (topLeft.y <= cell.y && bottomRight.x >= cell.x)
			return true;
	}
	return false;
}

int getHeight()
{
	return bottomRight.y - topLeft.y;
}

int getWidth()
{
	return bottomRight.x - topLeft.x;
}

int getArea()
{
	return (getHeight() + 1) * (getWidth() + 1);
}

private:
	void init()
{
	isStarting = false;
	isEnding = false;
	branchesCreatd = false;
	left = 0;
	right = 0;
	top = 0;
	bottom = 0;
}
};

/*! This class will generate a dungeon made of rooms and their connections. 
 * It does not do anything involving graphics, this is purely data about 
 * the rooms and their relationships.*/
class GeneratedMap
{
public:
	static const int MAX_ROOM_WIDTH = 3;
	static const int MAX_ROOM_HEIGHT = 3;
	static const int DEFAULT_MAP_SIZE = 20;
	//static const int DEFAULT_MAP_SIZE = 50;//4
	//static const int DEFAULT_MAP_SIZE = 500;//9

	GeneratedMap(const int width = 25, const int height = 16)
{
	myWidth = width;
	myHeight = height;
}

void generateMap()
{
	//create empty map
	myMapCells = std::vector<std::vector<char>>();
	for (int y = 0; y < myHeight; y++)
	{
		std::vector<char> row = std::vector<char>();
		for (int x = 0; x < myWidth; x++)
			row.push_back('.');
		
		myMapCells.push_back(row);
	}

	printMap("Empty map:");

	stx_srand(time(0));

	//add in purposely blank cells, so there's some emptiness
	createEmptyCells();

	printMap("with empty cells:");

	//now to finally create rooms ._.
	fillRooms();

	printMap("with rooms!");
} 

void printMap(std::string message)
{
	LOG_PRINT("%s\n", message.c_str());

	int element = 0;
	for (int y = 0; y < myHeight; y++)
	{
		for (int x = 0; x < myWidth; x++)
		{
			element = y * myWidth + x;
			LOG_PRINT("%c", myMapCells[y][x]);
		}
		LOG_PRINT("\n");
	}
	LOG_PRINT("\n");
}
	Room* startRoom; //!< The "entrance" room to the dungeon.
	Room* endRoom; //!< The "exit" room of the dungeon.
	std::vector<Room*> rooms; //!< All rooms, including start/end.
	
private:
	int myHeight;
	int myWidth;

	/*! This is the 2D grid of chars that represents the cells 
	 * in the dungeon.*/
	std::vector<std::vector<char>> myMapCells;	
	std::vector<Room*> roomStack;

bool allUnassignedReachable()
{
	std::queue<Vector2i> queue;
	Vector2i start(0, 0);
	while (myMapCells[start.y][start.x] != '.')
	{
		start.x = stx_rand() % myWidth;
		start.y = stx_rand() % myHeight;
	}
	queue.push(start);

	while (queue.size() > 0)
	{
		Vector2i cell = queue.front();
		queue.pop();
		
		//left
		if (cell.x > 0 && myMapCells[cell.y][cell.x - 1] == '.')
		{
			Vector2i newCell(cell.x - 1, cell.y);
			myMapCells[newCell.y][newCell.x] = 'T';
			queue.push(newCell);
		}

		//right
		if (cell.x < myWidth - 1 && myMapCells[cell.y][cell.x + 1] == '.')
		{
			Vector2i newCell(cell.x + 1, cell.y);
			myMapCells[newCell.y][newCell.x] = 'T';
			queue.push(newCell);
		}

		//top
		if (cell.y > 0 && myMapCells[cell.y - 1][cell.x] == '.')
		{
			Vector2i newCell(cell.x, cell.y - 1);
			myMapCells[newCell.y][newCell.x] = 'T';
			queue.push(newCell);
		}

		//bottom
		if (cell.y < myHeight - 1 && myMapCells[cell.y + 1][cell.x] == '.')
		{
			Vector2i newCell(cell.x, cell.y + 1);
			myMapCells[newCell.y][newCell.x] = 'T';
			queue.push(newCell);
		}
	}

	//make sure no unassigned cells remain
	bool result = true;
	for (int y = 0; y < myHeight; y++)
	{
		for (int x = 0; x < myWidth; x++)
		{
			if (myMapCells[y][x] == '.')
				result = false;
			else if (myMapCells[y][x] == 'T')
				myMapCells[y][x] = '.';
		}
	}
	return result;
}

bool canPlaceRoom(Vector2i &topLeft, Vector2i &bottomRight)
{
	if (!isRoomInBounds(topLeft, bottomRight))
		return false;

	//assuming that the topLeft is indeed to the left and above bottomRight
	for (int y = topLeft.y; y <= bottomRight.y; y++)
	{
		for (int x = topLeft.x; x <= bottomRight.x; x++)
		{
			if (myMapCells[y][x] != '.')
				return false;
		}
	}
	return true;
}

void cleanupTest()
{
	for (int y = 0; y < myHeight; y++)
	{
		for (int x = 0; x < myWidth; x++)
		{
			char cell = myMapCells[y][x];
			if (cell == 'T')
				myMapCells[y][x] = '.';
		}
	}

	replacePeriods(startRoom->topLeft, startRoom->bottomRight, 'R');
	replacePeriods(endRoom->topLeft, endRoom->bottomRight, 'R');
}

void createEmptyCells()
{
	std::vector<Room> emptyRooms;
	int numEmpties = (myWidth * myHeight) * 0.20; //20% of cells are empty
	while (true)
	{
		//empty vector of rooms
		emptyRooms.clear();
		int emptyCellCount = 0;
		while (emptyCellCount < numEmpties)
		{
			//randomly pick a size
			int width = 0;
			int num = stx_rand() % 100;
			if (num < 40)
				width = 0;
			else if (num >= 40 && num < 80)
				width = 1;
			else
				width = 2;

			int height = 0;
			num = stx_rand() % 100;
			if (num < 40)
				height = 0;
			else if (num >= 40 && num < 80)
				height = 1;
			else
				height = 2;

			//randomly pick location
			int xpos = stx_rand() % (myWidth - (width));
			int ypos = stx_rand() % (myHeight - (height));

			//randomly pick a size
			Vector2i topLeft(xpos, ypos);
			Vector2i bottomRight(xpos + width, ypos + height);

			if (canPlaceRoom(topLeft, bottomRight))
			{
				//if it fits then add it to the list
				Room emptyRoom(topLeft, bottomRight);
				emptyRooms.push_back(emptyRoom);

				//fill cells in map
				replacePeriods(topLeft, bottomRight, 'X');

				//recalculate number of empty cells
				emptyCellCount = 0;
				for (int i = 0; i < emptyRooms.size(); i++)
					emptyCellCount += emptyRooms[i].getArea();
			}
		}

		if (allUnassignedReachable())
			break;
		else
			removeEmpties();
	}
}

Room* createRoom(Vector2i entrance, Vector2i from)
{
	//the entrance is the first cell of the room that the player enters from
	//the previous room, this cell MUST be in the room. The from cell is the
	//last cell of the previous room, adjacent to "entrance". It'll tell us
	//which directions we're allowed to slide the room we're creating in.

	if (myMapCells[entrance.y][entrance.x] != '.')
		return 0;

	int width = 0;
	int num = stx_rand() % 100;
	if (num < 30)
		width = 0;
	else if (num >= 30 && num < 60)
		width = 1;
	else
		width = 2;

	int height = 0;
	num = stx_rand() % 100;
	if (num < 30)
		height = 0;
	else if (num >= 30 && num < 60)
		height = 1;
	else
		height = 2;

	Vector2i topLeft, bottomRight;
	if (entrance.y != from.y)
	{
		// up/down transition

		//set left and right edges of room
		int xmin = (entrance.x - width < 0) ? 0 : entrance.x - width;
		int shiftHoriz = (width == 0 || entrance.x - xmin == 0) ? 0 : stx_rand() % (entrance.x - xmin);// width;
		topLeft.x = xmin + shiftHoriz;
		bottomRight.x = topLeft.x + width;
		if (bottomRight.x >= myWidth)
			bottomRight.x = myWidth - 1;

		if (entrance.y < from.y)
		{
			//set top and bottom bounds
			bottomRight.y = entrance.y;
			int top = entrance.y - height;
			if (top < 0)
				top = 0;
			topLeft.y = top;
		}
		else
		{
			topLeft.y = entrance.y;
			int bottom = entrance.y + height;
			if (bottom >= myHeight)
				bottom = myHeight - 1;
			bottomRight.y = bottom;
		}
	}
	else
	{
		// left/right transition
		int ymin = (entrance.y - height < 0) ? 0 : entrance.y - height;
		int shiftVert = (height == 0 || entrance.y - ymin == 0) ? 0 : stx_rand() % (entrance.y - ymin);//height;
		topLeft.y = ymin + shiftVert;
		bottomRight.y = topLeft.y + height;
		if (bottomRight.y >= myHeight)
			bottomRight.y = myHeight - 1;

		if (entrance.x < from.x)
		{
			bottomRight.x = entrance.x;
			int left = entrance.x - width;
			if (left < 0)
				left = 0;
			topLeft.x = left;
		}
		else
		{
			topLeft.x = entrance.x;
			int right = topLeft.x + width;
			if (right >= myWidth)
				right = myWidth - 1;
			bottomRight.x = right;
		}
	}

	if (canPlaceRoom(topLeft, bottomRight))
	{
		Room* r = new Room(topLeft, bottomRight, entrance);
		r->previousCell = from;
		return r;
	}
	return 0;
}


void fillRooms()
{
	Vector2i startEnter, startFrom;
	Room* seedRoom;
	do
	{
		startEnter.x = stx_rand() % myWidth;
		startEnter.y = stx_rand() % myHeight;

		int dir = stx_rand() % 4;
		switch (dir)
		{
		case 0: //from left
			startFrom.x = startEnter.x - 1;
			startFrom.y = startEnter.y;
			break;
		case 1: //from right
			startFrom.x = startEnter.x + 1;
			startFrom.y = startEnter.y;
			break;
		case 2: //from top
			startFrom.x = startEnter.x;
			startFrom.y = startEnter.y - 1;
			break;
		case 3: //from bottom
			startFrom.x = startEnter.x;
			startFrom.y = startEnter.y + 1;
			break;
		}

		if (startFrom.x < 0 || startFrom.x >= myWidth || startFrom.y < 0 || startFrom.y >= myHeight)
			continue;

		seedRoom = createRoom(startEnter, startFrom);
		if (seedRoom != 0)
			break;
	} while (true);

	rooms.push_back(seedRoom);
	placeRoom(seedRoom);
	roomStack.push_back(seedRoom);

	while (roomStack.size() > 0)
	{
		//get the next room and remove it from the list/stack
		Room* room = *roomStack.begin();
		roomStack.erase(roomStack.begin());

		//try to generate another room in each direction
		if (room->topLeft.x > 0 && room->left == 0)
		{
			//to left
			std::vector<Vector2i> possibleStarts;
			for (int y = room->topLeft.y; y <= room->bottomRight.y; y++)
				possibleStarts.push_back(Vector2i(room->topLeft.x - 1, y));
			std::random_shuffle(possibleStarts.begin(), possibleStarts.end());

			//try to find an unassigned adjacent cell to the left
			Vector2i entrance;
			bool validStart = false;
			do
			{
				entrance = *possibleStarts.begin();
				possibleStarts.erase(possibleStarts.begin());
				if (myMapCells[entrance.y][entrance.x] == '.')
				{
					validStart = true;
					break;
				}
			} while (possibleStarts.size() > 0);

			if (validStart)
			{
				Vector2i from(entrance.x + 1, entrance.y);
				Room* newroom = 0;
				do
				{
					newroom = createRoom(entrance, from);
				} while (newroom == 0);

				if (newroom != 0)
				{
					placeRoom(newroom);
					room->left = newroom;
					roomStack.push_back(newroom);
					rooms.push_back(newroom);
				}
			}
		}

		if (room->bottomRight.x < myWidth - 1 && room->right == 0)
		{
			//to right
			std::vector<Vector2i> possibleStarts;
			for (int y = room->topLeft.y; y <= room->bottomRight.y; y++)
				possibleStarts.push_back(Vector2i(room->bottomRight.x + 1, y));
			std::random_shuffle(possibleStarts.begin(), possibleStarts.end());

			//try to find an unassigned adjacent cell to the right
			Vector2i entrance;
			bool validStart = false;
			do
			{
				entrance = *possibleStarts.begin();
				possibleStarts.erase(possibleStarts.begin());
				if (myMapCells[entrance.y][entrance.x] == '.')
				{
					validStart = true;
					break;
				}
			} while (possibleStarts.size() > 0);

			if (validStart)
			{
				Vector2i from(entrance.x - 1, entrance.y);
				Room* newroom = 0;
				do
				{
					newroom = createRoom(entrance, from);
				} while (newroom == 0);

				if (newroom != 0)
				{
					placeRoom(newroom);
					room->right = newroom;
					roomStack.push_back(newroom);
					rooms.push_back(newroom);
				}

			}
		}

		if (room->topLeft.y > 0 && room->top == 0)
		{
			//top
			std::vector<Vector2i> possibleStarts;
			for (int x = room->topLeft.x; x <= room->bottomRight.x; x++)
				possibleStarts.push_back(Vector2i(x, room->topLeft.y - 1));
			std::random_shuffle(possibleStarts.begin(), possibleStarts.end());

			//try to find an unassigned adjacent cell to the top
			Vector2i entrance;
			bool validStart = false;
			do
			{
				entrance = *possibleStarts.begin();
				possibleStarts.erase(possibleStarts.begin());
				if (myMapCells[entrance.y][entrance.x] == '.')
				{
					validStart = true;
					break;
				}
			} while (possibleStarts.size() > 0);

			if (validStart)
			{
				Vector2i from(entrance.x, entrance.y + 1);
				Room* newroom = 0;
				do
				{
					newroom = createRoom(entrance, from);
				} while (newroom == 0);

				if (newroom != 0)
				{
					placeRoom(newroom);
					room->top = newroom;
					roomStack.push_back(newroom);
					rooms.push_back(newroom);
				}

			}
		}

		if (room->bottomRight.y < myHeight - 1 && room->bottom == 0)
		{
			//bottom
			std::vector<Vector2i> possibleStarts;
			for (int x = room->topLeft.x; x <= room->bottomRight.x; x++)
				possibleStarts.push_back(Vector2i(x, room->bottomRight.y + 1));
			std::random_shuffle(possibleStarts.begin(), possibleStarts.end());

			//try to find an unassigned adjacent cell to the bottom
			Vector2i entrance;
			bool validStart = false;
			do
			{
				entrance = *possibleStarts.begin();
				possibleStarts.erase(possibleStarts.begin());
				if (myMapCells[entrance.y][entrance.x] == '.')
				{
					validStart = true;
					break;
				}
			} while (possibleStarts.size() > 0);

			if (validStart)
			{
				Vector2i from(entrance.x, entrance.y - 1);
				Room* newroom = 0;
				do
				{
					newroom = createRoom(entrance, from);
				} while (newroom == 0);

				if (newroom != 0)
				{
					placeRoom(newroom);
					room->bottom = newroom;
					roomStack.push_back(newroom);
					rooms.push_back(newroom);
				}
			}
		}

		//shuffle the remaining rooms
		if (roomStack.size() > 0)
			std::random_shuffle(roomStack.begin(), roomStack.end());
	}

	//pick a random two rooms as the start/finish rooms
	Room* start;
	Room* finish;
	do
	{
		start = rooms[stx_rand() % rooms.size()];
		finish = rooms[stx_rand() % rooms.size()];

		int mindistance = (myWidth > myHeight) ? myWidth * 0.75 : myHeight * 0.75;
		if (deltaDistanceVector2i(start->topLeft, finish->topLeft) < mindistance)
			continue;
		if (start->isDeadEnd() || finish->isDeadEnd())
			continue;

		start->isStarting = true;
		finish->isEnding = true;
		startRoom = start;
		endRoom = finish;
		break;
	} while (true);

}

bool isRoomInBounds(Vector2i &topLeft, Vector2i &bottomRight)
{
	if (topLeft.x < 0 || bottomRight.x < 0)
		return false;
	if (topLeft.x > myWidth || bottomRight.x >= myWidth)
		return false;
	if (topLeft.y < 0 || bottomRight.y < 0)
		return false;
	if (topLeft.y > myHeight || bottomRight.y >= myHeight)
		return false;

	return true;
}

void placeRoom(Room* room)
{
	for (int y = room->topLeft.y; y <= room->bottomRight.y; y++)
	{
		for (int x = room->topLeft.x; x <= room->bottomRight.x; x++)
		{
			myMapCells[y][x] = 'R';
		}
	}
}

void removeEmpties()
{
	for (int y = 0; y < myHeight; y++)
	{
		for (int x = 0; x < myWidth; x++)
		{
			if (myMapCells[y][x] == 'X')
				myMapCells[y][x] = '.';
		}
	}
}

void replacePeriods(Vector2i topLeft, Vector2i bottomRight, char replacement)
{
	for(int y = topLeft.y ; y <= bottomRight.y ; y++)
	{
		for(int x = topLeft.x ; x <= bottomRight.x ; x++)
		{
			if(myMapCells[y][x] == '.')			
				myMapCells[y][x] = replacement;			
		}
	}
}
public:
void DrawMap()
{
	D3DXFROMWINEVECTOR4 drawColor;

	Uint8 red = 80;

	int tilesize = 5;
	int tilesPerRoom = 10;
	int roomBuffer = 1;//each room is 2 tiles from the edge of the cell

	for (int i = 0; i < rooms.size(); i++)
	{
		Room* room = rooms[i];

		if (room == startRoom)
			drawColor=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 255.0f/256.0f, 255.0f/256.0f);
		else if (room == endRoom)
			drawColor=D3DXFROMWINEVECTOR4((9*16.0f+9.0f)/256.0f, 0.0f, 255.0f/256.0f, 255.0f/256.0f);
		else
		{
			drawColor=D3DXFROMWINEVECTOR4(255.0f/256.0f, 0.0f, 0.0f, 255.0f/256.0f);//255.0f/256.0f, 0.0f, 0.0f, 255.0f/256.0f);
			red += 2;
			if (red > 255.0f/256.0f)
			{
				red = 255.0f/256.0f;
			}
		}
		
		int horizCellCount = room->getWidth() + 1;
		horizCellCount *= tilesPerRoom;
		horizCellCount -= 2 * roomBuffer;

		int vertCellCount = room->getHeight() + 1;
		vertCellCount *= tilesPerRoom;
		vertCellCount -= 2 * roomBuffer;

		int startx = (room->topLeft.x * tilesize * tilesPerRoom);
		startx += roomBuffer * tilesize;

		int starty = (room->topLeft.y * tilesize * tilesPerRoom);
		starty += roomBuffer * tilesize;

		for (int y = 0; y < vertCellCount; y++)
		{
			for (int x = 0; x < horizCellCount; x++)
			{
				STX_Rect rekt;
				rekt.x = startx + (x * tilesize);
				rekt.y = starty + (y * tilesize);
				rekt.h = tilesize;
				rekt.w = tilesize;
				STX_RenderFillRect(rekt);
			}
		}

		//draw connections
		if (room->left != 0)
		{
			Vector2i startingCell = room->left->startingCell;
			int startx = (startingCell.x + 1) * tilesize * tilesPerRoom;
			startx -= (tilesize * roomBuffer);
			int starty = (startingCell.y * tilesize * tilesPerRoom) + ((tilesPerRoom * tilesize) / 2);

			for (int y = 0; y < 2; y++)
			{
				for (int x = 0; x < roomBuffer * 2; x++)
				{
					STX_Rect rekt;
					rekt.x = startx + (x * tilesize);
					rekt.y = starty + (y * tilesize);
					rekt.w = tilesize;
					rekt.h = tilesize;
					STX_RenderFillRect(rekt);
				}
			}
		}

		if (room->right != 0)
		{
			Vector2i startingCell = room->right->startingCell;
			int startx = startingCell.x * tilesize * tilesPerRoom;
			startx -= (tilesize * roomBuffer);
			int starty = (startingCell.y * tilesize * tilesPerRoom) + ((tilesPerRoom * tilesize) / 2);

			for (int y = 0; y < 2; y++)
			{
				for (int x = 0; x < roomBuffer * 2; x++)
				{
					STX_Rect rekt;
					rekt.x = startx + (x * tilesize);
					rekt.y = starty + (y * tilesize);
					rekt.w = tilesize;
					rekt.h = tilesize;
					STX_RenderFillRect(rekt);
				}
			}
		}

		if (room->top != 0)
		{
			Vector2i thisTopLeft = room->topLeft;
			Room top = *rooms[i]->top;

			Vector2i startingCell = room->top->startingCell;
			int starty = (startingCell.y + 1) * tilesize * tilesPerRoom;
			starty -= (tilesize * roomBuffer);

			int startx = (startingCell.x * tilesize * tilesPerRoom) + ((tilesPerRoom * tilesize) / 2);

			for (int y = 0; y < roomBuffer * 2; y++)
			{
				for (int x = 0; x < 2; x++)
				{
					STX_Rect rekt;
					rekt.x = startx + (x * tilesize);
					rekt.y = starty + (y * tilesize);
					rekt.w = tilesize;
					rekt.h = tilesize;
					STX_RenderFillRect(rekt);
				}
			}
		}

		if (room->bottom != 0)
		{
			Vector2i thisBottomRight = room->bottomRight;
			Room* top = rooms[i]->top;

			Vector2i startCell = room->bottom->startingCell;
			int starty = startCell.y * tilesize * tilesPerRoom;
			starty -= tilesize * roomBuffer;

			int startx = (startCell.x * tilesize * tilesPerRoom) + ((tilesPerRoom * tilesize) / 2);

			for (int y = 0; y < roomBuffer * 2; y++)
			{
				for (int x = 0; x < 2; x++)
				{
					STX_Rect rekt;
					rekt.x = startx + (x * tilesize);
					rekt.y = starty + (y * tilesize);
					rekt.w = tilesize;
					rekt.h = tilesize;
					STX_RenderFillRect(rekt);
				}
			}
		}

	}
}

};

#endif //GAME_MAP_HPP_

