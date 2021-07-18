/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include <iostream>
#include <fstream>
using namespace std;

const char* filename[] =
{
"../../IndieLib_resources/blender_3ds/cube.3ds",
"../../IndieLib_resources/blender_3ds/cup/cup.obj",
"../../IndieLib_resources/blender_3ds/cup/cup.mtl",
"../../IndieLib_resources/blender_3ds/cube/untitled.obj",
"../../IndieLib_resources/blender_3ds/cube/untitled.mtl"
};
#if 1

//>------ Id Chunk

#define MAIN3DS       0x4D4D

//>------ Main Chunks

#define EDIT3DS       0x3D3D  // this is the start of the editor config
#define KEYF3DS       0xB000  // this is the start of the keyframer config

//>------ sub defines of EDIT3DS

#define EDIT_MATERIAL 0xAFFF
#define EDIT_CONFIG1  0x0100
#define EDIT_CONFIG2  0x3E3D
#define EDIT_VIEW_P1  0x7012
#define EDIT_VIEW_P2  0x7011
#define EDIT_VIEW_P3  0x7020
#define EDIT_VIEW1    0x7001
#define EDIT_BACKGR   0x1200
#define EDIT_AMBIENT  0x2100
#define EDIT_OBJECT   0x4000

#define EDIT_UNKNW01  0x1100
#define EDIT_UNKNW02  0x1201
#define EDIT_UNKNW03  0x1300
#define EDIT_UNKNW04  0x1400
#define EDIT_UNKNW05  0x1420
#define EDIT_UNKNW06  0x1450
#define EDIT_UNKNW07  0x1500
#define EDIT_UNKNW08  0x2200
#define EDIT_UNKNW09  0x2201
#define EDIT_UNKNW10  0x2210
#define EDIT_UNKNW11  0x2300
#define EDIT_UNKNW12  0x2302 // new chunk type
#define EDIT_UNKNW13  0x3000
#define EDIT_UNKNW14  0xAFFF

//>------ sub defines of EDIT_MATERIAL
#define MAT_NAME01    0xA000  //> includes name (see mli doc for materials)

//>------ sub defines of EDIT_OBJECT

#define OBJ_TRIMESH   0x4100
#define OBJ_LIGHT     0x4600
#define OBJ_CAMERA    0x4700

#define OBJ_UNKNWN01  0x4010
#define OBJ_UNKNWN02  0x4012 //>>---- Could be shadow

//>------ sub defines of OBJ_CAMERA
#define CAM_UNKNWN01  0x4710 // new chunk type
#define CAM_UNKNWN02  0x4720 // new chunk type

//>------ sub defines of OBJ_LIGHT
#define LIT_OFF       0x4620
#define LIT_SPOT      0x4610
#define LIT_UNKNWN01  0x465A

//>------ sub defines of OBJ_TRIMESH
#define TRI_VERTEXL   0x4110
#define TRI_FACEL2    0x4111 // unknown yet
#define TRI_FACEL1    0x4120
#define TRI_SMOOTH    0x4150
#define TRI_LOCAL     0x4160
#define TRI_VISIBLE   0x4165

//>>------ sub defs of KEYF3DS

#define KEYF_UNKNWN01 0xB009
#define KEYF_UNKNWN02 0xB00A
#define KEYF_FRAMES   0xB008
#define KEYF_OBJDES   0xB002

#define KEYF_OBJHIERARCH  0xB010
#define KEYF_OBJDUMMYNAME 0xB011
#define KEYF_OBJUNKNWN01  0xB013
#define KEYF_OBJUNKNWN02  0xB014
#define KEYF_OBJUNKNWN03  0xB015  
#define KEYF_OBJPIVOT     0xB020  
#define KEYF_OBJUNKNWN04  0xB021  
#define KEYF_OBJUNKNWN05  0xB022  

//>>------  these define the different color chunk types
#define COL_RGB  0x0010
#define COL_TRU  0x0011
#define COL_UNK  0x0013 // unknown

//>>------ defines for viewport chunks

#define TOP           0x0001
#define BOTTOM        0x0002
#define LEFT          0x0003
#define RIGHT         0x0004
#define FRONT         0x0005
#define BACK          0x0006
#define USER          0x0007
#define CAMERA        0x0008 // 0xFFFF is the code read from file
#define LIGHT         0x0009
#define DISABLED      0x0010
#define BOGUS         0x0011
#define __DEBUG__ 1
unsigned char ReadChar (void)
{
 return (fgetc (bin3ds));
}
unsigned int ReadInt (void)
{
 unsigned int temp = ReadChar();
 return ( temp | (ReadChar () << 8));
}
unsigned int ReadLong (void)
{
 unsigned int temp1,temp2;
 unsigned int temp3,temp4;

 temp1=ReadInt ();
 temp2=ReadInt ();

 return (temp3+(temp4*0x10000L));
}
unsigned int GetChunkPointer (void)
{
 return (ftell (bin3ds)-2); // compensate for the already read Marker
}
unsigned int ReadChunkPointer (void)
{
 return (ReadLong ());
}
unsigned int ReadUnknownChunk (unsigned int chunk_id)
{
 unsigned int current_pointer;
 unsigned int temp_pointer;

 chunk_id=chunk_id;

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 ChangeChunkPointer (current_pointer+temp_pointer); 
 // move to the new chunk position
 return (temp_pointer);
}
unsigned int ReadKeyfChunk (void)
{
 unsigned char end_found=FALSE;
 unsigned int temp_int;
 unsigned int current_pointer;
 unsigned int temp_pointer;
 unsigned int tellertje=6L;

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 while (end_found==FALSE)
 {
   temp_int=ReadInt ();

       switch (temp_int)
       {
        case KEYF_UNKNWN01 :tellertje+=ReadUnknownChunk (temp_int);break;
        case KEYF_UNKNWN02 :tellertje+=ReadUnknownChunk (temp_int);break;
        case KEYF_FRAMES   :
                            #ifdef __DEBUG__
                            LOG_PRINT (">>> Found Keyframer frames chunk id of %0X\n",
                                    temp_int);
                            #endif
                            tellertje+=ReadUnknownChunk (temp_int);
                            break;
        case KEYF_OBJDES   :
                            #ifdef __DEBUG__
                            LOG_PRINT (">>> Found Keyframer object description chunk id of %0X\n",
                                    temp_int);
                            #endif
                            tellertje+=ReadUnknownChunk (temp_int);
                            break;
        case EDIT_VIEW1    :
                            #ifdef __DEBUG__
                            LOG_PRINT (">>> Found View main def chunk id of %0X\n",
                                    temp_int);
                            #endif
                            tellertje+=ReadViewChunk ();
                            break;
        default:            break;
       }

   tellertje+=2;
   if (tellertje>=temp_pointer)
     end_found=TRUE;
 }

 ChangeChunkPointer (current_pointer+temp_pointer); 
 // move to the new chunk position
 return (temp_pointer);
}
unsigned int ReadEditChunk (void)
{
 unsigned char end_found=FALSE;
 unsigned int temp_int;
 unsigned int current_pointer;
 unsigned int temp_pointer;
 unsigned int tellertje=6L;

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 while (end_found==FALSE)
 {
   temp_int=ReadInt ();

       switch (temp_int)
       {
        case EDIT_UNKNW01:tellertje+=ReadUnknownChunk (EDIT_UNKNW01);break;
        case EDIT_UNKNW02:tellertje+=ReadUnknownChunk (EDIT_UNKNW02);break;
        case EDIT_UNKNW03:tellertje+=ReadUnknownChunk (EDIT_UNKNW03);break;
        case EDIT_UNKNW04:tellertje+=ReadUnknownChunk (EDIT_UNKNW04);break;
        case EDIT_UNKNW05:tellertje+=ReadUnknownChunk (EDIT_UNKNW05);break;
        case EDIT_UNKNW06:tellertje+=ReadUnknownChunk (EDIT_UNKNW06);break;
        case EDIT_UNKNW07:tellertje+=ReadUnknownChunk (EDIT_UNKNW07);break;
        case EDIT_UNKNW08:tellertje+=ReadUnknownChunk (EDIT_UNKNW08);break;
        case EDIT_UNKNW09:tellertje+=ReadUnknownChunk (EDIT_UNKNW09);break;
        case EDIT_UNKNW10:tellertje+=ReadUnknownChunk (EDIT_UNKNW10);break;
        case EDIT_UNKNW11:tellertje+=ReadUnknownChunk (EDIT_UNKNW11);break;
        case EDIT_UNKNW12:tellertje+=ReadUnknownChunk (EDIT_UNKNW12);break;
        case EDIT_UNKNW13:tellertje+=ReadUnknownChunk (EDIT_UNKNW13);break;

        case EDIT_MATERIAL :
                            #ifdef __DEBUG__
                            LOG_PRINT (">>> Found Materials chunk id of %0X\n",
                                    temp_int);
                            #endif
                            tellertje+=ReadMaterialChunk ();
                            break;
        case EDIT_VIEW1    :
                            #ifdef __DEBUG__
                            LOG_PRINT (">>> Found View main def chunk id of %0X\n",
                                    temp_int);
                            #endif
                            tellertje+=ReadViewChunk ();
                            break;
        case EDIT_BACKGR   :
                            #ifdef __DEBUG__
                            LOG_PRINT (">>> Found Backgr chunk id of %0X\n",
                                    temp_int);
                            #endif
                            tellertje+=ReadBackgrChunk ();
                            break;
        case EDIT_AMBIENT  :
                            #ifdef __DEBUG__
                            LOG_PRINT (">>> Found Ambient chunk id of %0X\n",
                                    temp_int);
                            #endif
                            tellertje+=ReadAmbientChunk ();
                            break;
        case EDIT_OBJECT   :
                            #ifdef __DEBUG__
                            LOG_PRINT (">>> Found Object chunk id of %0X\n",
                                    temp_int);
                            #endif
                            tellertje+=ReadObjectChunk ();
                            break;
        default:            break;
       }

   tellertje+=2;
   if (tellertje>=temp_pointer)
     end_found=TRUE;
 }

 ChangeChunkPointer (current_pointer+temp_pointer); 
 // move to the new chunk position
 return (temp_pointer);
}
unsigned int ReadMainChunk (void)
{
 unsigned char end_found=FALSE;
 unsigned int temp_int;
 unsigned int current_pointer;
 unsigned int temp_pointer;
 unsigned int tellertje=6L;

 current_pointer=GetChunkPointer ();
 temp_pointer   =ReadChunkPointer ();

 while (end_found==FALSE)
 {
   temp_int=ReadInt ();

       switch (temp_int)
       {
        case KEYF3DS :
                      #ifdef __DEBUG__
                      LOG_PRINT (">> Found *Keyframer* chunk id of %0X\n",KEYF3DS);
                      #endif
                      tellertje+=ReadKeyfChunk ();
                      break;
        case EDIT3DS :
                      #ifdef __DEBUG__
                      LOG_PRINT (">> Found *Editor* chunk id of %0X\n",EDIT3DS);
                      #endif
                      tellertje+=ReadEditChunk ();
                      break;
        default:      break;
       }

   tellertje+=2;
   if (tellertje>=temp_pointer)
    end_found=TRUE;
 }

 ChangeChunkPointer (current_pointer+temp_pointer); 
 // move to the new chunk position
 return (temp_pointer);
}
void readFile(const char* filename,
std::vector<D3DXFROMWINEVECTOR3>& vertices,
std::vector<D3DXFROMWINEVECTOR3>& normals,
std::vector<D3DXFROMWINEVECTOR2>& texcoords)
{
 unsigned char version;

 if (ReadInt ()==MAIN3DS)
 {
  #ifdef __DEBUG__
  LOG_PRINT ("> Found Main chunk id of %0X\n",MAIN3DS);
  #endif
  //>---------- find version number
  fseek (bin3ds,28L,SEEK_SET);
  version=ReadChar ();
  if (version<3)
  {
   #ifdef __DEBUG__
   LOG_PRINT ("Sorry this lib can only read 3ds files of version 3.0 and higher\n");
   LOG_PRINT ("The version of the file you want to read is: %d\n",version);
   #endif
   return (1);
  }
  fseek (bin3ds,2,SEEK_SET);
  ReadMainChunk ();
 }
 else
  return (1);

 return (0);
}
#endif
#if 0
void readFile(const char* filename,
std::vector<D3DXFROMWINEVECTOR3>& vertices,
std::vector<D3DXFROMWINEVECTOR3>& normals,
std::vector<D3DXFROMWINEVECTOR2>& texcoords)
{
	vertices.clear();
	normals.clear();
	texcoords.clear();
  int length = 0;
  char * buffer = 0;
  std::ifstream file (filename, std::ifstream::binary);
  if (file) {
    // get length of file:
    file.seekg (0, file.end);
    length = file.tellg();
    file.seekg (0, file.beg);

    buffer = new char [length];

    std::cout << "Reading " << length << " characters... ";
    // read data as a block:
    file.read (buffer,length);

    if (file)
      std::cout << "all characters read successfully.";
    else
      std::cout << "error: only " << file.gcount() << " could be read";
    file.close();

    // ...buffer contains the entire file...

    delete[] buffer;
  }

  union {
  	float f;
  	unsigned char b[4];
  } u;
  unsigned char b3, b2, b1, b0;

  for(unsigned int i=0;i<length;i++)
  {
	if(buffer[i++]==0xa)
	{
        	//file >> prefix;
		//cout << "prefix:" << prefix << "\n";
        switch (buffer[i++]) {
            case 'v' : {
		//cout << "prefix:" << prefix << "\n";
                switch (buffer[i++]) {
                    case 't' : {
                        u.b[3]=buffer[i++];
                        u.b[2]=buffer[i++];
                        u.b[1]=buffer[i++];
                        u.b[0]=buffer[i++];
			float x=u.f;
                        u.b[3]=buffer[i++];
                        u.b[2]=buffer[i++];
                        u.b[1]=buffer[i++];
                        u.b[0]=buffer[i++];
                        float y=u.f;
			texcoords.push_back(D3DXFROMWINEVECTOR2(x, y));
			cout << "texcoords.push_back(" << x << ", " << y << ")\n";
                    }
			break; // texcoords
                    case 'n' : { // normals
                        u.b[3]=buffer[i++];
                        u.b[2]=buffer[i++];
                        u.b[1]=buffer[i++];
                        u.b[0]=buffer[i++];
                        float x=u.f;
                        u.b[3]=buffer[i++];
                        u.b[2]=buffer[i++];
                        u.b[1]=buffer[i++];
                        u.b[0]=buffer[i++];
                        float y=u.f;
                        u.b[3]=buffer[i++];
                        u.b[2]=buffer[i++];
                        u.b[1]=buffer[i++];
                        u.b[0]=buffer[i++];
                        float z=u.f;
			normals.push_back(D3DXFROMWINEVECTOR3(x, y, z));
			cout << "normals.push_back(" << x << ", " << y  << ", " << z << ")\n";
                    }
                    default : { // points
                        u.b[3]=buffer[i++];
                        u.b[2]=buffer[i++];
                        u.b[1]=buffer[i++];
                        u.b[0]=buffer[i++];
                        float x=u.f;
                        u.b[3]=buffer[i++];
                        u.b[2]=buffer[i++];
                        u.b[1]=buffer[i++];
                        u.b[0]=buffer[i++];
                        float y=u.f;
                        u.b[3]=buffer[i++];
                        u.b[2]=buffer[i++];
                        u.b[1]=buffer[i++];
                        u.b[0]=buffer[i++];
                        float z=u.f;
			vertices.push_back(D3DXFROMWINEVECTOR3(x, y, z));
			cout << "vertices.push_back(" << x << ", " << y  << ", " << z << ")\n";
                    }
                }
            } break;
            case 'f' : {
                unsigned short a, b, c;
                file >> a >> b >> c;

                // etc
            } break;
            default : {} // skip
        }}}
	file.close();
}
#else
std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
void readFile(const char* filename,
std::vector<D3DXFROMWINEVECTOR3>& temp_vertices,
std::vector<D3DXFROMWINEVECTOR3>& temp_normals,
std::vector<D3DXFROMWINEVECTOR2>& temp_uvs)
{
	FILE * file = fopen(filename, "r");
	if( file == NULL )
	{
    		LOG_PRINT("Impossible to open the file !\n");
    		return;
	}
	while( 1 )
	{
    		char lineHeader[128];
    		// read the first word of the line
    		int res = fscanf(file, "%s", lineHeader);
    		if (res == EOF)
        		break; // EOF = End Of File. Quit the loop.

    		// else : parse lineHeader

		if ( strcmp( lineHeader, "v" ) == 0 )
		{
    			D3DXFROMWINEVECTOR3 vertex;
    			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
    			temp_vertices.push_back(vertex);
		}
		else if ( strcmp( lineHeader, "vt" ) == 0 )
		{
    			D3DXFROMWINEVECTOR2 uv;
    			fscanf(file, "%f %f\n", &uv.x, &uv.y );
    			temp_uvs.push_back(uv);
		}
		else if ( strcmp( lineHeader, "vn" ) == 0 )
		{
    			D3DXFROMWINEVECTOR3 normal;
    			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
    			temp_normals.push_back(normal);
		}
		else if ( strcmp( lineHeader, "f" ) == 0 )
		{
    			std::string vertex1, vertex2, vertex3;
    			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
    			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
    			if (matches != 9)
			{
        			LOG_PRINT("File can't be read by our simple parser : ( Try exporting with other options\n");
        			return;
    			}
    			vertexIndices.push_back(vertexIndex[0]);
    			vertexIndices.push_back(vertexIndex[1]);
    			vertexIndices.push_back(vertexIndex[2]);
    			uvIndices    .push_back(uvIndex[0]);
    			uvIndices    .push_back(uvIndex[1]);
    			uvIndices    .push_back(uvIndex[2]);
    			normalIndices.push_back(normalIndex[0]);
    			normalIndices.push_back(normalIndex[1]);
    			normalIndices.push_back(normalIndex[2]);
		}
	}
}
#endif

int ApplicationLogic()
{
#if 0
  streampos size;
  char * memblock;

  ifstream file (filename[0], ios::in|ios::binary);
  if (file.is_open())
  {
    size = file.tellg();
    memblock = new char [size];
    file.seekg (0, ios::beg);
    file.read (memblock, size);
    file.close();

    cout << "the entire file content file in memory";

    delete[] memblock;
  }
  else cout << "Unable to open file";

  for(unsigned int i=0;i<size;i++)
  {
	LOG_PRINT("  \%02hhx", memblock[i]);
  }
#else
	std::vector<D3DXFROMWINEVECTOR3> vertices;
	std::vector<D3DXFROMWINEVECTOR3> normals;
	std::vector<D3DXFROMWINEVECTOR2> texcoords;
	readFile(filename[0], vertices, normals, texcoords);
	LOG_PRINT("vertices.size()=%d\n", vertices.size());
	LOG_PRINT("normals.size()=%d\n", normals.size());
	LOG_PRINT("texcoords.size()=%d\n", texcoords.size());
#endif
  return 0;
}

