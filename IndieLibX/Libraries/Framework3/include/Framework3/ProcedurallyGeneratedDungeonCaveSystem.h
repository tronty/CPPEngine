#ifndef ProcedurallyGeneratedDungeonCaveSystem_HPP_
#define ProcedurallyGeneratedDungeonCaveSystem_HPP_

// https://gamedevelopment.tutsplus.com/tutorials/create-a-procedurally-generated-dungeon-cave-system--gamedev-10099

/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

class Room
{
public:
    std::vector < std::vector < bool > > Map;
    // these values hold grid coordinates for each corner of the room
    int x1
    int x2
    int y1
    int y2
 
    // width and height of room in terms of grid
    int w
    int h
 
    // center point of the room
    Vector2i center;
 
    // constructor for creating new rooms
    Room(int x, int y, int w, int h) {
        x1 = x;
        x2 = x + w;
        y1 = y;
        y2 = y + h;
        this.x = x * Main.TILE_WIDTH;
        this.y = y * Main.TILE_HEIGHT;
        this.w = w;
        this.h = h;
        center = new Point(stx_floor((x1 + x2) / 2),
            stx_floor((y1 + y2) / 2));
    }
 
    // return true if this room intersects provided room
    bool intersects(Room& room) {
        return (x1 <= room.x2 && x2 >= room.x1 &&
            y1 <= room.y2 && room.y2 >= room.y1);
    }

	
void placeRooms1() {
    // create array for room storage for easy access
    std::vector<Room*> rooms;
 
    // randomize values for each room
    for(unsigned int r=0;r<maxRooms;r++){
        w = minRoomSize + stx_rand(maxRoomSize - minRoomSize + 1);
        h = minRoomSize + stx_rand(maxRoomSize - minRoomSize + 1);
        x = stx_rand(MAP_WIDTH - w - 1) + 1;
        y = stx_rand(MAP_HEIGHT - h - 1) + 1;
 
        // create room with randomized values
        Room* newRoom=new Room(x, y, w, h);
 
        bool failed = false;
        for(unsigned int i=0;i<rooms.size();i++){
        	otherRoom=rooms[i];
            if (newRoom.intersects(otherRoom)) {
                failed = true;
                break;
            }
        }
        if (!failed) {
            // local to carve out new room
            createRoom(newRoom);
 
            // push new room into rooms array
            rooms.push_back(newRoom)
        }
    }
}
	
void hCorridor(int x1, int x2, y) {
        for(int x=int(stx_min(x1, x2));x<int(stx_max(x1, x2)) + 1;x++){
            // destory the tiles to "carve" out corridor
            Map[x][y].parent.removeChild(Map[x][y]);
 
            // place a new unblocked tile
            Map[x][y] = new Tile(Tile.DARK_GROUND, false, false);
 
            // add tile as a new game object
            addChild(Map[x][y]);
 
            // set the location of the tile appropriately
            Map[x][y].setLoc(x, y);
        }
    }
 
    // create vertical corridor to connect rooms
    void vCorridor(int y1, int y2, x) {
        for(int y=int(stx_min(y1, y2));y<int(stx_max(y1, y2)) + 1;y++){
            // destroy the tiles to "carve" out corridor
            Map[x][y].parent.removeChild(Map[x][y]);
 
            // place a new unblocked tile
            Map[x][y] = new Tile(Tile.DARK_GROUND, false, false);
 
            // add tile as a new game object
            addChild(Map[x][y]);
 
            // set the location of the tile appropriately
            Map[x][y].setLoc(x, y);
        }
    }


	
void placeRooms() {
// store rooms in an array for easy access
    std::vector<Room*> rooms;
 
// variable for tracking center of each room
newCenter = 0;
 
// randomize values for each room
        for(unsigned int r=0;r<maxRooms;r++){
    w = minRoomSize + stx_rand(maxRoomSize - minRoomSize + 1);
    h = minRoomSize + stx_rand(maxRoomSize - minRoomSize + 1);
    x = stx_rand(MAP_WIDTH - w - 1) + 1;
    y = stx_rand(MAP_HEIGHT - h - 1) + 1;
 
    // create room with randomized values
    newRoom = new Room(x, y, w, h);
 
    failed = false;
        for(unsigned int r=0;r<rooms.size();r++){
        	Room* otherRoom=rooms[i];
        if (newRoom.intersects(otherRoom)) {
            failed = true;
            break;
        }
    }
    if (!failed) {
        // local to carve out new room
        createRoom(newRoom);
 
        // store center for new room
        newCenter = newRoom.center;
 
        if(rooms.length != 0){
            // store center of previous room
            prevCenter = rooms[rooms.length - 1].center;
 
            // carve out corridors between rooms based on centers
            // randomly start with horizontal or vertical corridors
            if (stx_rand(2) == 1) {
                hCorridor(int(prevCenter.x), int(newCenter.x),
                    int(prevCenter.y));
                vCorridor(int(prevCenter.y), int(newCenter.y),
                    int(newCenter.x));
            } else {
                vCorridor(int(prevCenter.y), int(newCenter.y),
                    int(prevCenter.x));
                hCorridor(int(prevCenter.x), int(newCenter.x),
                    int(newCenter.y));
                }
            }
        }
    if(!failed) rooms.push_back(newRoom);
    }
}
void Quadify()
{
   D3DXFROMWINEVECTOR2 vS(2.0f/Map_size_x, 2.0f/Map_size_y);
   for (int y=1;y<Map_size_y-1;y++) 
   {
	for (int x=1;x<Map_size_x-1;x++) 
        { 
		if (Map[y][x]==true)
		{
			if (Map[y-1][x]==false)
			{
				D3DXFROMWINEVECTOR3 vT(2.0f*float(x)/(Map_size_x-1)-1.0f, 0.0f, 2.0f*float(y-0.5f)/(Map_size_y-1)-1.0f);
				D3DXFROMWINEVECTOR3 vR(0.0f, 0.0f, 0.0f);
				D3DXFROMWINEMATRIX w=stx_GetPivotMatrix(vR, vT);
				DrawQuad(w, vS);
			}
			else if (Map[y+1][x]==false)
			{
				D3DXFROMWINEVECTOR3 vT(2.0f*float(x)/(Map_size_x-1)-1.0f, 0.0f, 2.0f*float(y+0.5f)/(Map_size_y-1)-1.0f);
				D3DXFROMWINEVECTOR3 vR(0.0f, 0.0f, 0.0f);
				D3DXFROMWINEMATRIX w=stx_GetPivotMatrix(vR, vT);
				DrawQuad(w, vS);
			}
			else if (Map[y][x-1]==false)
			{
				D3DXFROMWINEVECTOR3 vT(2.0f*float(x-0.5f)/(Map_size_x-1)-1.0f, 0.0f, 2.0f*float(y)/(Map_size_y-1)-1.0f);
				D3DXFROMWINEVECTOR3 vR(90.0f, 0.0f, 0.0f);
				D3DXFROMWINEMATRIX w=stx_GetPivotMatrix(vR, vT);
				DrawQuad(w, vS);
			}
			else if (Map[y][x+1]==false)
			{
				D3DXFROMWINEVECTOR3 vT(2.0f*float(x+0.5f)/(Map_size_x-1)-1.0f, 0.0f, 2.0f*float(y)/(Map_size_y-1)-1.0f);
				D3DXFROMWINEVECTOR3 vR(90.0f, 0.0f, 0.0f);
				D3DXFROMWINEMATRIX w=stx_GetPivotMatrix(vR, vT);
				DrawQuad(w, vS);
			}
		}
	}
   }
}
};
#endif

