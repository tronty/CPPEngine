#ifndef _BSPDungeon_H_
#define _BSPDungeon_H_

// https://eskerda.com/bsp-dungeon-generation/

/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>
#include <STX/LuaUtil.h>
using namespace LuaUtil;
#include <tinyxml/tinyxml.h>

#if 0
unsigned int MAP_SIZE         = 50;
unsigned int N_ITERATIONS     = 4;
#else
unsigned int MAP_SIZE         = 500;
unsigned int N_ITERATIONS     = 9;
#endif

const bool DISCARD_BY_RATIO = true;
const float H_RATIO          = 0.45;
const float W_RATIO          = 0.45;

inline int random_(int imin, int imax)
{
	//int r = (imin + (stx_rand() % (int)(imax - imin + 1)));
	//LOG_PRINT("r=%f\n", r);
	return stx_rand();
}
inline unsigned int random__(unsigned int nMin, unsigned int nMax)
{
    return nMin + (unsigned int)((double)stx_rand() / (RAND_MAX+1) * (nMax-nMin+1));
}

class BSPContainer
{
public:
	unsigned int x;
	unsigned int y;
	unsigned int w;
	unsigned int h;
	Vector2i center;
	BSPContainer(unsigned int ax=0, unsigned int ay=0, unsigned int aw=0, unsigned int ah=0)
	{
    		x = ax;
    		y = ay;
    		w = aw;
    		h = ah;
    		center = Vector2i(x + (w/2), y + (h/2));
	}
	BSPContainer(const BSPContainer& rhs)
	{
    		x = rhs.x;
    		y = rhs.y;
    		w = rhs.w;
    		h = rhs.h;
    		center = rhs.center;
	}

	void paint()
	{
		STX_Rect rekt;
		rekt.x = x;
		rekt.y = y;
		unsigned int SQUARE = 1;// ??? IRenderer::GetRendererInstance()->GetViewportWidth() / MAP_SIZE;
		rekt.w = w * SQUARE;
		rekt.h = h * SQUARE;
		//LOG_FNLN;
		//LOG_PRINT("x=%d\n", x);
		//LOG_PRINT("y=%d\n", y);
		//LOG_PRINT("w=%d\n", w);
		//LOG_PRINT("h=%d\n", h);
		STX_RenderFillRect(rekt);
	}
	void dump()
	{
		#if 0
		LOG_PRINT("x=%d\n", x);
		LOG_PRINT("y=%d\n", y);
		LOG_PRINT("w=%d\n", w);
		LOG_PRINT("h=%d\n", h);
		#endif
	}
};
typedef BSPContainer BSPLeaf; // ???
typedef BSPContainer BSPChild; // ???

class BSPTree
{
public:
    	BSPLeaf* leaf;
    	BSPTree* lchild;
    	BSPTree* rchild;
	BSPTree(BSPLeaf* aleaf=0)
	{
    		leaf = aleaf;
    		lchild = 0;
    		rchild = 0;
	}
	std::vector<BSPLeaf*> getLeafs()
	{
		std::vector<BSPLeaf*> vLeaf;
    		if (lchild == 0 && rchild == 0)
			vLeaf.push_back(leaf);
		else
		{
			std::vector<BSPLeaf*> lLeafs=lchild->getLeafs();
			for(unsigned int i=0;i<lLeafs.size();i++)
				vLeaf.push_back(lLeafs[i]);
			std::vector<BSPLeaf*> rLeafs=rchild->getLeafs();
			for(unsigned int i=0;i<rLeafs.size();i++)
				vLeaf.push_back(rLeafs[i]);
		}
		return vLeaf;
	}

	void paint()
	{
		leaf->paint();
    		if (lchild!=0)
        		lchild->paint();
    		if (rchild!=0)
        		rchild->paint();
	}
};

inline std::vector<BSPContainer*> random_split_(BSPContainer* container)
	{
		//LOG_FNLN;
		container->dump();
    		BSPContainer* r1=0;
		BSPContainer* r2=0;
    		if (random_(0, 1) == 0)
		{
        		// Vertical
        		r1 = new BSPContainer(container->x, container->y, random_(1, container->w), container->h);
        		r2 = new BSPContainer(container->x + r1->w, container->y, container->w - r1->w, container->h);
		}
    		else
		{
        		// Horizontal
        		r1 = new BSPContainer(container->x, container->y, container->w, random_(1, container->h));
        		r2 = new BSPContainer(container->x, container->y + r1->h, container->w, container->h - r1->h);
		}
		std::vector<BSPContainer*> mergedlist;
		mergedlist.push_back(r1);
		mergedlist.push_back(r2);
		return mergedlist;
	}
inline std::vector<BSPContainer*> random_split(BSPContainer* container)
	{
		//LOG_FNLN;
		container->dump();
    		BSPContainer* r1=0;
		BSPContainer* r2=0;
    		if (random_(0, 1) == 0)
		{
        		// Vertical
        		r1 = new BSPContainer(container->x, container->y, random_(1, container->w), container->h);
     			
        		r2 = new BSPContainer(container->x + r1->w, container->y, container->w - r1->w, container->h);

        		if (DISCARD_BY_RATIO)
			{
            			float r1_w_ratio = r1->w / r1->h;
            			float r2_w_ratio = r2->w / r2->h;
            			if (r1_w_ratio < W_RATIO or r2_w_ratio < W_RATIO)
					return random_split(container);
			}
		}
    		else
		{
        		// Horizontal
        		r1 = new BSPContainer(container->x, container->y, container->w, random_(1, container->h));
        		r2 = new BSPContainer(container->x, container->y + r1->h, container->w, container->h - r1->h);

        		if (DISCARD_BY_RATIO)
			{
            			float r1_h_ratio = r1->h / r1->w;
            			float r2_h_ratio = r2->h / r2->w;
            			if (r1_h_ratio < H_RATIO or r2_h_ratio < H_RATIO)
					return random_split(container);
			}
		std::vector<BSPContainer*> mergedlist;
		mergedlist.push_back(r1);
		mergedlist.push_back(r2);
		return mergedlist;
		}
	}

	inline BSPTree* split_container(BSPContainer* container, unsigned int iter)
	{
    		BSPTree* root = new BSPTree(container);
    		if (iter != 0)
		{
        		std::vector<BSPContainer*> sr;
			sr = random_split(container);
        		root->lchild = split_container(sr[0], iter-1);
        		root->rchild = split_container(sr[1], iter-1);
    			return root;
		}
	}


class BSPDungeon
{
public:
	BSPTree* container_tree;
	BSPDungeon()
	{
		container_tree=0;
		init();
	}
	void init()
	{
#if 0
		LuaScript script;
		bool r=script.Load("/BSPDungeon/BSPDungeon.lua");
		if(!r)
		{
			MAP_SIZE = script.get<int>("MAP_SIZE");
			N_ITERATIONS = script.get<int>("N_ITERATIONS");
		}
#else
	TiXmlDocument* document;
	TiXmlHandle* docHandle;
	TiXmlHandle* BodyHandle;
	std::string fn=stx_convertpath("/BSPDungeon/BSPDungeon.xml");
	LOG_PRINT("fn.c_str()=%s\n", fn.c_str());

	TiXmlDocument document(fn.c_str());
	document.LoadFile();
	TiXmlHandle docHandle( &document );
	TiXmlHandle BodyHandle = docHandle.FirstChild("Body");
	TiXmlElement* BodyElement = BodyHandle.Element();
	if(BodyElement) {LOG_PRINT("!BodyElement\n");stx_exit(0);}

	MAP_SIZE = atoi(BodyHandle.FirstChild("MAP_SIZE").Element()->Attribute("Value"));
	N_ITERATIONS = atoi(BodyHandle.FirstChild("N_ITERATIONS").Element()->Attribute("Value"));
#endif
		LOG_PRINT("MAP_SIZE=%d\n", MAP_SIZE);
		LOG_PRINT("N_ITERATIONS=%d\n", N_ITERATIONS);

		BSPContainer* main_container = new BSPContainer(0, 0, IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight());
		//LOG_FNLN;
		main_container->dump();
		container_tree = split_container(main_container, N_ITERATIONS);
	}

	void render()
	{
		container_tree->paint();
	}
};
#endif

