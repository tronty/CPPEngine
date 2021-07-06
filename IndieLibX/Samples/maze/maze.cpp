#include <Framework3/IRenderer.h>

// Maze Generator in C++

// Simple maze generator written in 10 minutes :) The source code is public domain.

// Simple Maze Generator in C++ by Jakub Debski '2006 

#include <stdio.h>      /* STX_PRINT, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <vector>
#include <list>
using namespace std;

int ApplicationLogic() 
{
   stx_srand(time(0)); 
 
   const int maze_size_x=80; 
   const int maze_size_y=25; 
   vector < vector < bool > > maze; 
   list < pair < int, int> > drillers; 
 
   maze.resize(maze_size_y); 
   for (size_t y=0;y<maze_size_y;y++) 
           maze[y].resize(maze_size_x); 
 
   for (size_t x=0;x<maze_size_x;x++) 
           for (size_t y=0;y<maze_size_y;y++) 
                   maze[y][x]=false; 
 
   drillers.push_back(make_pair(maze_size_x/2,maze_size_y/2)); 
   while(drillers.size()>0) 
   { 
           list < pair < int, int> >::iterator m,_m,temp; 
           m=drillers.begin(); 
           _m=drillers.end(); 
           while (m!=_m) 
           { 
                   bool remove_driller=false; 
                   switch(stx_rand()%4) 
                   { 
                   case 0: 
                           (*m).second-=2; 
                           if ((*m).second<0 || maze[(*m).second][(*m).first]) 
                           { 
                                   remove_driller=true; 
                                   break; 
                           } 
                           maze[(*m).second+1][(*m).first]=true; 
                           break; 
                   case 1: 
                           (*m).second+=2; 
                           if ((*m).second>=maze_size_y || maze[(*m).second][(*m).first]) 
                           { 
                                   remove_driller=true; 
                                   break; 
                           } 
                           maze[(*m).second-1][(*m).first]=true; 
                           break; 
                   case 2: 
                           (*m).first-=2; 
                           if ((*m).first<0 || maze[(*m).second][(*m).first]) 
                           { 
                                   remove_driller=true; 
                                   break; 
                           } 
                           maze[(*m).second][(*m).first+1]=true; 
                           break; 
                   case 3: 
                           (*m).first+=2; 
                           if ((*m).first>=maze_size_x || maze[(*m).second][(*m).first]) 
                           { 
                                   remove_driller=true; 
                                   break; 
                           } 
                           maze[(*m).second][(*m).first-1]=true; 
                           break; 
                   } 
                   if (remove_driller) 
                           m = drillers.erase(m); 
                   else 
                   { 
                           drillers.push_back(make_pair((*m).first,(*m).second)); 
                           // uncomment the line below to make the maze easier 
                           // if (stx_rand()%2) 
                           drillers.push_back(make_pair((*m).first,(*m).second)); 
 
                           maze[(*m).second][(*m).first]=true; 
                           ++m; 
                   } 
           } 
   } 
 
   // Done 
   char buf[maze_size_x+1];
   buf[maze_size_x]='\0';
   for (size_t y=0;y<maze_size_y;y++) 
   { 
	   for (size_t x=0;x<maze_size_x;x++) 
           { 
                   if (maze[y][x]==true)
			   buf[x]='.';
                   else 
                           buf[x]='#'; 
           }
   }
   LOG_PRINT("%s\n", buf); 
 
   return 0; 
}

