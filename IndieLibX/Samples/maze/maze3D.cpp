/*
  Copyright (c) 2018 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

// Maze Generator in C++

// Simple maze generator written in 10 minutes :) The source code is public domain.

// Simple Maze Generator in C++ by Jakub Debski '2006

#include <stdio.h>      /* STX_PRINT, NULL */
#include <stdlib.h>     /* stx_srand, stx_rand */
#include <time.h>       /* time */
#include <vector>
#include <list>
using namespace std;

const int maze_size_x=50;
const int maze_size_y=50;

std::vector<Vertex> g_vertices_;
std::vector<TextureID> textures;
std::vector<stx_Texture> Textures;
std::vector<stx_Material> vt;
std::vector<stx_Mesh> mesh_;
TextureID texID=-1;

std::vector<D3DXFROMWINEVECTOR3> g_vertices3;
std::vector<D3DXFROMWINEVECTOR3> g_normals3;
std::vector<D3DXFROMWINEVECTOR2> g_texcoords2;
std::vector<__WORD__> g_indices1;
std::vector<__WORD__> g_indices2;

void DrawQuad(D3DXFROMWINEMATRIX& aM, D3DXFROMWINEVECTOR2 asize=D3DXFROMWINEVECTOR2(1.0f, 1.0f))
{
	{D3DXFROMWINEVECTOR3 v(-asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(0.0f, 0.0f);Vertex vx(vo, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);}
	{D3DXFROMWINEVECTOR3 v( asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(1.0f, 0.0f);Vertex vx(vo, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);}
	{D3DXFROMWINEVECTOR3 v( -asize.x, asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(0.0f, 1.0f);Vertex vx(vo, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);}
	if(0==((g_indices1.size()%3)))
	{
		if(textures.size() && (texID>-1))
		{
			unsigned int  pos = find(textures.begin(), textures.end(), texID) - textures.begin();
      LOG_FNLN;
      LOG_PRINT("textures.size()=%d\n", textures.size());
      LOG_PRINT("pos=%d\n", pos);
			if(texID==textures[pos])
				g_indices2.push_back(pos);
			else
				g_indices2.push_back(0);
		}
		else
			g_indices2.push_back(0);
	}
	{D3DXFROMWINEVECTOR3 v(-asize.x, asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(0.0f, 1.0f);Vertex vx(vo, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);}
	{D3DXFROMWINEVECTOR3 v( asize.x, asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(1.0f, 1.0f);Vertex vx(vo, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);}
	{D3DXFROMWINEVECTOR3 v(asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(1.0f, 0.0f);Vertex vx(vo, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);}
	if(0==((g_indices1.size()%3)))
	{
		if(textures.size() && (texID>-1))
		{
			unsigned int  pos = find(textures.begin(), textures.end(), texID) - textures.begin();
      LOG_FNLN;
      LOG_PRINT("textures.size()=%d\n", textures.size());
      LOG_PRINT("pos=%d\n", pos);
			if(texID==textures[pos])
				g_indices2.push_back(pos);
			else
				g_indices2.push_back(0);
		}
		else
			g_indices2.push_back(0);
	}
}

int CreateMaze()
{
   stx_srand(time(0));
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
   D3DXFROMWINEVECTOR2 vS(2.0f/maze_size_x, 2.0f/maze_size_y);
   D3DXFROMWINEVECTOR3 vT, vR;
   for (int y=1;y<maze_size_y-1;y++)
   {
	for (int x=1;x<maze_size_x-1;x++)
        {
		if (maze[y][x]==true)
		{
			if (maze[y-1][x]==false)
			{
				vT=D3DXFROMWINEVECTOR3(2.0f*float(x)/(maze_size_x-1)-1.0f, 0.0f, 2.0f*float(y-0.5f)/(maze_size_y-1)-1.0f);
				vR=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
			}
			else if (maze[y+1][x]==false)
			{
				vT=D3DXFROMWINEVECTOR3(2.0f*float(x)/(maze_size_x-1)-1.0f, 0.0f, 2.0f*float(y+0.5f)/(maze_size_y-1)-1.0f);
				vR=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
			}
			else if (maze[y][x-1]==false)
			{
				vT=D3DXFROMWINEVECTOR3(2.0f*float(x-0.5f)/(maze_size_x-1)-1.0f, 0.0f, 2.0f*float(y)/(maze_size_y-1)-1.0f);
				vR=D3DXFROMWINEVECTOR3(90.0f, 0.0f, 0.0f);
			}
			else if (maze[y][x+1]==false)
			{
				vT=D3DXFROMWINEVECTOR3(2.0f*float(x+0.5f)/(maze_size_x-1)-1.0f, 0.0f, 2.0f*float(y)/(maze_size_y-1)-1.0f);
				vR=D3DXFROMWINEVECTOR3(90.0f, 0.0f, 0.0f);
			}
			D3DXFROMWINEMATRIX w=stx_GetPivotMatrix(vR, vT);
			DrawQuad(w, vS);
		}
	}
   }

	#if 0
	stx_Materials_(vt, g_indices2, mesh_);
	stx_writeXFile("/maze3D.x", mesh_);
	#endif

   return 0;
}

int init(const char* aTitle)
{
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	texID=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/relief_rockwall.jpg", false, ss);

	CreateMaze();

	LOG_PRINT("g_vertices3.size() =%d\n", g_vertices3.size());
	LOG_PRINT("g_normals3.size()  =%d\n", g_normals3.size());
	LOG_PRINT("g_texcoords2.size()=%d\n", g_texcoords2.size());
	LOG_PRINT("g_indices1.size()  =%d\n", g_indices1.size());
	LOG_PRINT("g_indices2.size()  =%d\n", g_indices2.size());

	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	static float mAngleX=45.0f;
	static float mAngleY=45.0f;

	stx_GetAngles(mAngleX, mAngleY, 0.25f);

	IRenderer::GetRendererInstance()->InitTex3(IRenderer::GetRendererInstance()->GettexShader3(), IRenderer::GetRendererInstance()->GettexVF3());
	IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GettexShader3());
	IRenderer::GetRendererInstance()->setVertexFormat(IRenderer::GetRendererInstance()->GettexVF3());
	D3DXFROMWINEMATRIX R, S, W;
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, mAngleX, mAngleY, 0.0f);
	D3DXFROMWINEMatrixScaling(&S, 1.25f, 1.25f, 1.25f);
	W=R*S;
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
	IRenderer::GetRendererInstance()->setTexture("Sampler", texID);
	if(g_indices1.size())
		IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, g_vertices_.size(), g_indices1.size()/3, &g_indices1[0], &g_indices1[0], CONSTANT_INDEX2, &g_vertices_[0], &g_vertices_[0], sizeof(Vertex));
	else
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, g_vertices_.size()/3, &g_vertices_[0], &g_vertices_[0], sizeof(Vertex));

	const char* txt = "Use mouse buttons to rotate the model.";
	IRenderer::GetRendererInstance()->drawText(txt, 10, 10,
		15, 18,
		IRenderer::GetRendererInstance()->GetdefaultFont(),
		IRenderer::GetRendererInstance()->GetlinearClamp(),
		IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
		IRenderer::GetRendererInstance()->GetnoDepthTest());

	txt = "CTRL-r resets the scene.";
	IRenderer::GetRendererInstance()->drawText(txt, 10, 30,
		15, 18,
		IRenderer::GetRendererInstance()->GetdefaultFont(),
		IRenderer::GetRendererInstance()->GetlinearClamp(),
		IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
		IRenderer::GetRendererInstance()->GetnoDepthTest());

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("maze3D");
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}
/*
		   	   x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float));
*/
/*
		  	   x	  y     tx    ty
	float v[16] =  { -1.0f, -1.0f, 0.0f, 1.0f,
			  1.0f, -1.0f, 1.0f, 1.0f,
			  1.0f,  1.0f, 1.0f, 0.0f,
			 -1.0f,  1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, &v[0], &v[0], 4*sizeof(float));
*/

