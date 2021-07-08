#ifndef __ProceduralHouse__
#define __ProceduralHouse__
/*
  Copyright(c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/Turtle.h>

#define COLONWITHQUOTES ':'
#define COLONWITHDOUBLEQUOTES ":"

//define __WRITEXFILE__ 1

#ifdef __WRITEXFILE__
std::vector<stx_Material> vt;
std::vector<stx_Mesh> mesh_;
#endif
std::vector<__WORD__> inds;
std::vector<__WORD__> inds2;
int texid=-1;

#if 0
inline std::vector<std::string> regexmatch5(std::string& inst)
{
std::vector<std::string> v;
std::regex rgx("\\s*\\bComp\\b\\s*");
std::sregex_token_iterator iter(inst.begin(),
    inst.end(),
    rgx,
    -1);
std::sregex_token_iterator end;
for ( ; iter != end; ++iter)
    //std::cout << *iter << '\n';
	v.push_back(*iter);
return v;
}
inline std::cmatch regexmatch2(const char* cstr)
{
  //const char cstr[] = "Repeat(X, 2, wall_window_wall)";
  std::regex e("\\bRepeat\\b\\s?\\((.*?)\\)");

  std::cmatch cm;
  std::regex_match(cstr,cm,e);
  std::cout << "string literal with " << cm.size() << " matches\n";

  std::cout << "the matches1 were: " << std::endl;
  for(unsigned i=0; i<cm.size(); ++i) {
    std::cout << "[" << cm[i] << "] " << std::endl;
  }

  std::string str1=cm[2];
  std::vector<std::string> instv1=stx_split(str1, ',');
  for(unsigned i=0; i<instv1.size(); ++i) {
    std::cout << "[" << i << ": "<< instv1[i] << "] " << std::endl;
  }

  std::cout << std::endl;
	return cm;
}

inline std::cmatch regexmatch3(const char* cstr)
{
  //const char cstr[] = "Split(Y,(40%, 40%, 20%),(Wall, Window, Wall))";
  std::regex e("\\bSplit\\b\\s?\\(\\s?([XYZ])\\s?,\\s?\\((.*?)\\)\\s?,\\s?\\((.*?)\\)\\s?\\)");

  std::cmatch cm;
  std::regex_match(cstr,cm,e);
  std::cout << "string literal with " << cm.size() << " matches\n";

  std::cout << "the matches1 were: " << std::endl;
  for(unsigned i=0; i<cm.size(); ++i) {
    std::cout << "[" << i << ": "<< cm[i] << "] " << std::endl;
  }
	return cm;
}
inline std::cmatch regexmatch4(const char* cstr)
{
  //const char cstr[] = "Comp(side_face_x, wall)";
  std::regex e("[:]?\\bComp\\b\\s?\\((.*?)\\)[:]?");

  std::cmatch cm;
  std::regex_match(cstr,cm,e);
  std::cout << "string literal with " << cm.size() << " matches\n";

  std::cout << "the matches1 were: " << std::endl;
  for(unsigned i=0; i<cm.size(); ++i) {
    std::cout << "[" << i << ": "<< cm[i] << "] " << std::endl;
  }
	return cm;
}
#endif

class Turtle_
{
protected:
	D3DXFROMWINEVECTOR3 p;
	float angle;
	D3DXFROMWINEVECTOR4 m_color;
	std::vector<D3DXFROMWINEVECTOR3> pv;
public:
	std::vector<D3DXFROMWINEVECTOR3> mVertices;
	std::vector<D3DXFROMWINEVECTOR3> mNormals;
	std::vector<D3DXFROMWINEVECTOR2> mTexCoords;
public:
	void Clear()
	{
		mVertices.clear();
		mNormals.clear();
		inds.clear();
		inds2.clear();
		mTexCoords.clear();
	}

	void Vertex3f(float x,float  y, float  z)
	{
		IRenderer::GetRendererInstance()->Vertex3f(x, y, z);
#ifdef __WRITEXFILE__
		D3DXFROMWINEVECTOR3 v(x, y, z);
		mVertices.push_back(v);
		D3DXFROMWINEVECTOR3 n;
		D3DXFROMWINEVec3Normalize(&n, &v);
		mNormals.push_back(n);
#endif
	}
	void Color3fv(const float* v)
	{
		IRenderer::GetRendererInstance()->Color3fv(v);
	}
	void TexCoord2f(float x,float  y)
	{
		IRenderer::GetRendererInstance()->TexCoord2f(x, y);
#ifdef __WRITEXFILE__
		mTexCoords.push_back(D3DXFROMWINEVECTOR2(x, y));
#endif
	}
    Turtle_(D3DXFROMWINEVECTOR3 ap=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), float a=0.0f, D3DXFROMWINEVECTOR4 acolor=D3DXFROMWINEVECTOR4(1.0f, 1.0f, 1.0f, 1.0f))
    {
	p = ap;
	pv.push_back(D3DXFROMWINEVECTOR3(p.x, p.y, 0.0f));
	angle = a;
	m_color=acolor;
    }
    Turtle_(const Turtle_& rhs)
    {
	p = rhs.p;
	pv = rhs.pv;
	angle = rhs.angle;
	m_color=rhs.m_color;
    }
#ifndef _MSC_VER
	static std::stack<Turtle_> _Turtlestack__;
	static void push(Turtle_& aTurtle){_Turtlestack__.push(aTurtle);}
	static Turtle_ pop()
{
	if(!_Turtlestack__.empty())
	{
		Turtle_ turtle=_Turtlestack__.top();
		_Turtlestack__.pop();
		return turtle;
	}
	else
		LOG_PRINT("Turtle_ stack empty.\n");
	Turtle_ turtle;
	return turtle;
}
#else
	void push(Turtle_& aTurtle);
	Turtle_ pop();
#endif
    	void move(float adistance)
    	{
		//LOG_PRINT("adistance=%f\n",adistance);
		//LOG_PRINT("angle=%f\n",angle);
        	p.x += adistance * cosf(angle);
        	p.y += adistance * sinf(angle);
		//LOG_PRINT("p.x=%f\n",p.x);
		//LOG_PRINT("p.y=%f\n",p.y);
	}
    	void forward(float adistance)
    	{
        	move(adistance);
		pv.push_back(D3DXFROMWINEVECTOR3(p.x, p.y, 0.0f));
    }
    void turn(float a)
    {
        angle += a;
    }
    void color(D3DXFROMWINEVECTOR4& acolor)
    {
        m_color = acolor;
    }
    void draw(TextureID id, const D3DXFROMWINEMATRIX& MVP)
    {
    		D3DXFROMWINEVECTOR4 color(1.0f, 1.0f, 1.0f, 1.0f);
		D3DXFROMWINEVECTOR2 tc[3]={	D3DXFROMWINEVECTOR2(0.0f, 0.0f),
						D3DXFROMWINEVECTOR2(0.0f, 1.0f),
						D3DXFROMWINEVECTOR2(1.0f, 1.0f)};
    		IRenderer::GetRendererInstance()->Begin(PRIM_TRIANGLES);
		//LOG_PRINT("pv.size()=%d\n", pv.size());
		for(unsigned int i=0;i<pv.size()/3;i++)
		{
    			Vertex3f(pv[3*i].x, pv[3*i].y, 0.0f);
    			TexCoord2f(tc[0].x, tc[0].y);
    			Color3fv(&color.x);
    			Vertex3f(pv[3*i+1].x, pv[3*i+1].y, 0.0f);
    			TexCoord2f(tc[1].x, tc[1].y);
    			Color3fv(&color.x);
    			Vertex3f(pv[3*i+2].x, pv[3*i+2].y, 0.0f);
    			TexCoord2f(tc[2].x, tc[2].y);
    			Color3fv(&color.x);
			inds2.push_back(3*i+0);
			inds2.push_back(3*i+1);
			inds2.push_back(3*i+2);
			inds.push_back(0);
		}
    		IRenderer::GetRendererInstance()->End(id, &MVP);
		pv.clear();
    }
};
inline void drawLsystem_(Turtle_& aTurtle, vector<string>& ainstructions, float aangle, float adistance, TextureID id, const D3DXFROMWINEMATRIX& MVP)
{
	for(unsigned int i = 0;i<ainstructions.size();i++)
	{
		string symbol=ainstructions[i];
		//LOG_PRINT("symbol=%s\n", symbol.c_str());
		if(symbol == "[")
			aTurtle.push(aTurtle);
		else if(symbol == "]")
			aTurtle.pop();
		else if(symbol == "F")
			aTurtle.forward(adistance);
		else if(symbol == "b")
			aTurtle.move(adistance);
		else if(symbol == "B")
			aTurtle.forward(-adistance);
		else if(symbol == "+")
			aTurtle.turn(aangle);
		else if(symbol == "-")
			aTurtle.turn(-aangle);
		else if(symbol[0] == 'c')
		{
			std::size_t foundo = symbol.find("(");
  			std::size_t foundc = symbol.find(")");
  			if((foundo!=std::string::npos) &&(foundc!=std::string::npos) &&(foundo<foundc))
			{
				std::vector<std::string> v=stx_split(symbol.substr(foundo+1,foundc-1), ',');
				if(v.size()==4)
				{
					D3DXFROMWINEVECTOR4 c(atof(v[0].c_str()), atof(v[1].c_str()), atof(v[2].c_str()), atof(v[3].c_str()));
					aTurtle.color(c);
				}
			}
		}
	}
	aTurtle.draw(id, MVP);
}

struct ProceduralHouse
{
	Turtle_ mTurtle;
	unsigned int NFLOORS;
	std::string axiom;
  	std::vector<stx_Rule> rules;

	D3DXFROMWINEMATRIX MVP;
	TextureID texture;
	TextureID textureroof;
	TextureID texturewindow;
	TextureID texturedoor;
	std::string textureName;
	std::string textureroofName;
	std::string texturewindowName;
	std::string texturedoorName;

	std::vector<std::string> instv_;

	ProceduralHouse(){}
	void init()
	{
		NFLOORS=0;
		axiom="";
		texture=-1;
		textureroof=-1;
		texturewindow=-1;
		texturedoor=-1;
		textureName="";
		textureroofName="";
		texturewindowName="";
		texturedoorName="";
	}
	virtual ~ProceduralHouse()
	{
		Clear();
	}
	void Clear()
	{
		NFLOORS=0;
		axiom="";
		rules.clear();
		instv_.clear();
		textureName="";
		textureroofName="";
		texturewindowName="";
		texturedoorName="";
	}
	void SetWallTexture(const char* aTextureFile)
	{
		textureName=aTextureFile;
	}
	void SetRoofTexture(const char* aTextureFile)
	{
		textureroofName=aTextureFile;
	}
	void SetWindowTexture(const char* aTextureFile)
	{
		texturewindowName=aTextureFile;
	}
	void SetDoorTexture(const char* aTextureFile)
	{
		texturedoorName=aTextureFile;
	}

	void SetFloorCount(unsigned int aNFLOORS){NFLOORS=aNFLOORS;}
	void SetAxiom(std::string aaxiom, std::string aaxiomrulebody){rules.clear();axiom=aaxiom;rules.push_back(stx_Rule(aaxiom, aaxiomrulebody));}
	void AddRule(std::string arulename, std::string arulebody)
	{
		if(axiom=="")
		{
			LOG_PRINT("You should call ProceduralHouse::SetAxiom before ProceduralHouse::AddRule calls!\n");
			return;
		}
		rules.push_back(stx_Rule(arulename, arulebody));
	}

void getRM(D3DXFROMWINEMATRIX& R, const float mAngleX, const float mAngleY, const unsigned int n=0, const unsigned int m=0)
{
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R,
		                            D3DXFROMWINEToRadian(mAngleX)+n*D3DXFROMWINE_PI/2.0f,
		                            D3DXFROMWINEToRadian(mAngleY)+m*D3DXFROMWINE_PI/2.0f,
		                            0.0f);
}

void drawTriangle(const float x0, const float y0, const float x1, const float y1, const float z=0.0f, D3DXFROMWINEVECTOR4 color=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 1.0f))
{
    mTurtle.Vertex3f(-1.0f, 1.0f, 1.0f);
    mTurtle.TexCoord2f(0.0f,  0.0f);
    mTurtle.Color3fv(&color.x);
    mTurtle.Vertex3f( 1.0f, 1.0f, 1.0f);
    mTurtle.TexCoord2f(1.0f,  0.0f);
    mTurtle.Color3fv(&color.x);
    mTurtle.Vertex3f( 0.0f, 1.5f, -1.0f);
    mTurtle.TexCoord2f(0.0f,  1.0f);
    mTurtle.Color3fv(&color.x);
    
    inds.push_back(texid);
}

void drawQuad(const float x0, const float y0, const float x1, const float y1, const float z, D3DXFROMWINEVECTOR4 acolor=D3DXFROMWINEVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), TextureID id=-1)
{
	D3DXFROMWINEVECTOR4 color=acolor;
	if(id!=-1)
	{
		color=D3DXFROMWINEVECTOR4(0.5f, 0.5f, 0.5f, 1.0f);
		IRenderer::GetRendererInstance()->End(texture, &MVP);
		IRenderer::GetRendererInstance()->Begin(PRIM_TRIANGLES);
	}
	float y1_=(y1-y0);
		
    mTurtle.Vertex3f(x0, y0, z);
    mTurtle.TexCoord2f(0.0f,  0.0f);
    mTurtle.Color3fv(&color.x);
    mTurtle.Vertex3f(x0, y1, z);
    mTurtle.TexCoord2f(0.0f, y1_);
    mTurtle.Color3fv(&color.x);
    mTurtle.Vertex3f(x1, y1, z);
    mTurtle.TexCoord2f(1.0f, y1_);
    mTurtle.Color3fv(&color.x);
    
    inds.push_back(texid);
    mTurtle.Vertex3f(x0, y0, z);
    mTurtle.TexCoord2f(0.0f, 0.0f);
    mTurtle.Color3fv(&color.x);
    mTurtle.Vertex3f(x1, y1, z);
    mTurtle.TexCoord2f(1.0f, y1_);
    mTurtle.Color3fv(&color.x);
    mTurtle.Vertex3f(x1, y0, z);
    mTurtle.TexCoord2f(1.0f, 0.0f);
    mTurtle.Color3fv(&color.x);
    
    inds.push_back(texid);
	if(id!=-1)
	{
		IRenderer::GetRendererInstance()->End(id, &MVP);
		IRenderer::GetRendererInstance()->Begin(PRIM_TRIANGLES);
	}
}

void drawHouseFacade(std::string& instv__, const D3DXFROMWINEMATRIX& w, const float z=0.0f)
{
	std::vector<std::string> instv=stx_split(instv__, ':');
	float y=-1.0f;
	float dx=2.0f/instv.size();
	float dy=2.0f/NFLOORS;
	D3DXFROMWINEVECTOR4 windowcolor(0.0f, 0.0f, 0.0f, 1.0f);
	for(unsigned int j=0;j<NFLOORS;j++)
	{
		//LOG_PRINT("j=%d\n", j);
		//LOG_PRINT("y=%f\n", y);
	float x=-1.0f;
	float xpdx=x+dx;
	unsigned int i=0;
	while(i<instv.size())
	{
		if(instv[i]=="wall")
		{
    			texid=0;
			//LOG_PRINT("wall\n");
			if(j==(NFLOORS-1))
			{
				drawQuad(x, y+0.000f/NFLOORS, xpdx, y+1.875f/NFLOORS, z, D3DXFROMWINEVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
				drawQuad(x, y+1.875f/NFLOORS, xpdx, y+2.000f/NFLOORS, z, D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 1.0f));
			}
			else
				drawQuad(x, y+0.000f/NFLOORS, xpdx, y+2.000f/NFLOORS, z, D3DXFROMWINEVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));				
		}
		else if((instv[i]=="window") ||((instv[i]=="door") && j))
		{
    			texid=2;
			//LOG_PRINT("window\n");
			//LOG_PRINT("j=%d\n", j);
			//LOG_PRINT("NFLOORS=%d\n", NFLOORS);
			if(j==(NFLOORS-1))
			{
				drawQuad(x, y+0.000f/NFLOORS, xpdx, y+0.500f/NFLOORS, z, D3DXFROMWINEVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
				drawQuad(x, y+0.500f/NFLOORS, xpdx, y+1.500f/NFLOORS, z, windowcolor, texturewindow);
				drawQuad(x, y+1.500f/NFLOORS, xpdx, y+1.875f/NFLOORS, z, D3DXFROMWINEVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
				drawQuad(x, y+1.875f/NFLOORS, xpdx, y+2.000f/NFLOORS, z, D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 1.0f));
			}
			else
			{
				drawQuad(x, y+0.00f/NFLOORS, xpdx, y+0.50f/NFLOORS, z, D3DXFROMWINEVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
				drawQuad(x, y+0.50f/NFLOORS, xpdx, y+1.50f/NFLOORS, z, windowcolor, texturewindow);
				drawQuad(x, y+1.50f/NFLOORS, xpdx, y+2.00f/NFLOORS, z, D3DXFROMWINEVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
			}
		}
		else if(instv[i]=="door")
		{
			texid=3;
			//LOG_PRINT("door\n");
			if(j==(NFLOORS-1))
			{
				drawQuad(x, y+0.000f/NFLOORS, xpdx, y+1.500f/NFLOORS, z, D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), texturedoor);
				drawQuad(x, y+1.500f/NFLOORS, xpdx, y+1.875f/NFLOORS, z, D3DXFROMWINEVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
				drawQuad(x, y+1.875f/NFLOORS, xpdx, y+2.000f/NFLOORS, z, D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 1.0f));
			}
			else
			{
				drawQuad(x, y+0.000f/NFLOORS, xpdx, y+1.875f/NFLOORS, z, D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), texturedoor);
				drawQuad(x, y+1.875f/NFLOORS, xpdx, y+2.000f/NFLOORS, z, D3DXFROMWINEVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
			}
		}
		//else LOG_PRINT("%d %s\n", i, instv[i]);
		i++;
		x+=dx;
		xpdx=x+dx;
		}
		y+=dy;
	}
	//drawTriangle(-1.0f, 1.0f, 1.0f, 1.5f, z);
}

std::string getRule(std::vector<stx_Rule>& arules, std::string& arulename)
{
	std::string rulename=stx_trim(arulename);
	std::size_t found = rulename.find(COLONWITHDOUBLEQUOTES);
  	if(found==std::string::npos)
	{
		for(unsigned int i=0;i<arules.size();i++)
		{
			stx_Rule rule=arules[i];
			if(rulename==rule.m_RuleName)
				return rule.m_RuleBody;
		}
	}
	return "";
}
std::string applyRules(std::vector<stx_Rule>& arules, std::string& aaxiom)
{
	std::string axiom=stx_trim(aaxiom);
	std::size_t found = axiom.find(COLONWITHDOUBLEQUOTES);
  	if(found==std::string::npos)
	{
		std::string rule=getRule(arules, axiom);
		if(!(rule==""))
		{
			std::string str=applyRules(arules, rule);
			return str;
		}
		else
			return axiom;
	}
	else
	{
		std::vector<std::string> v=stx_split(axiom, ':');
		std::string str=applyRules(arules, stx_trim(v[0]));
		for(unsigned int j=1;j<v.size();j++)
		{
			str+=COLONWITHDOUBLEQUOTES+applyRules(arules, stx_trim(v[j]));
		}
		return str;
	}
	return "";
}
#if 0

sphere(1,20,20)

Push():T(0, 0, 6):S(8, 10, 18):I(cube):Pop():T(6, 0, 0):S(7, 13, 18):I(cube):T(0, 0, 16):S(8, 15, 8):I(cylinder)

// implementationA
addRule('obj', I(cube))
// implementationB
addRule('obj', I(cylinder))
// implementationC
addRule('obj', I(sphere), chance = 0.5)

simpleHouseWall = Repeat(X, 2, wall_window_wall)
wall_window_wall = Split(X, [30%, 40%, 30%], [Box, vertical_window_wall, Box])
vertical_window_wall = Split(Z, [40%, 40%, 20%], [Box, Window, Box])

Repeat(axis, dimension, symbol, random)
n = dscope[axis]/dimensione
i f random
tmp =(rnd(1, random),..., rnd(1, random))
|
{z
100
unit = P n-1
i=0
else
}
n
tmp i
ratios =(tmp 0 ∗ unit %,..., tmp n-1 ∗ unit %)
ratios =(
100
100
%,...,
%)
| n
{z n }
n
return Split(axis, ratios,(symbol,..., symbol))

simpleHouseBody = Comp('side_face_x', wall_window_wall):Comp('side_face_y', wall_door_wall):Comp('side_faces_nx', wall_window_wall):Comp('side_faces_ny', wall_window_wall)

I(battlement, params = {'crenel_width' : 0.2, 'merlon_width' : 0.2, 'merlon_height' : 0.4 })

stone_window_obj = Push():T(0,-windowpillardim/2, 0):Module('iron_grid'):Pop():Module('stone_window_frame')

iron_grid = Repeat(X, 0.1, barX)):T(0, 0.001, 0):Repeat(Z, 0.1, barZ)
barZ = SizeZ(0.02):SizeY(0.02):I(cube)
barX = SizeX(0.02):SizeY(0.02):I(cube)
// left p a r t o f window frame
window_pillar_irr_left = Repeat(Z, windowpillardim, stone_small_brick_pillar_left, 0.5)
// b r i c k o f pillar
stone_small_brick_pillar_left = ResizeAligned(-X, prnd(1, 0.6)):ResizeAligned(-Y, prnd(1, 0.6)):T(rnd(0.0 1, 1), rnd(0.0 1, 1), 0):I(stone)

#endif
#if 0

simpleHouseBody = Comp('side_face_x', wall_window_wall):Comp('side_face_y', wall_door_wall):Comp('side_faces_nx', wall_window_wall):Comp('side_faces_ny', wall_window_wall)

simpleHouseWall = Repeat(X, 2, wall_window_wall)
wall_window_wall = Split(X, [30%, 40%, 30%], [Box, vertical_window_wall, Box])
vertical_window_wall = Split(Z, [40%, 40%, 20%], [Box, Window, Box])

wall_door_wall = Split(X, [30%, 40%, 30%], [Box, vertical_door_wall, Box])
vertical_door_wall = Split(Z, [80%, 20%], [Door, Box])

#endif
int SetTitle(const char* aTitle)
{
#if 0
	regexmatch2("Repeat(X, 2, wall-window-wall)");
	regexmatch3("Split(Y,(40%, 40%, 20%),(Wall, Window, Wall))");
	regexmatch4("Comp(side_face_x, wall)");
	stx_exit(0);
#endif
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	//SamplerStateID ss=IRenderer::GetRendererInstance()->GetlinearClamp();
	//SamplerStateID ss=IRenderer::GetRendererInstance()->GetnearestClamp();

	if(texture==-1)
		texture=IRenderer::GetRendererInstance()->addImageLibTexture(textureName.c_str(), false, ss);
	if(textureroof==-1)
		textureroof=IRenderer::GetRendererInstance()->addImageLibTexture(textureroofName.c_str(), false, ss);
	if(texturewindow==-1)
		texturewindow=IRenderer::GetRendererInstance()->addImageLibTexture(texturewindowName.c_str(), false, ss);
	if(texturedoor==-1)
		texturedoor=IRenderer::GetRendererInstance()->addImageLibTexture(texturedoorName.c_str(), false, ss);
#ifdef __WRITEXFILE__
	vt.push_back(stx_Material(textureName.c_str()+1, D3DXFROMWINEVECTOR3(0.5f, 0.0f, 0.0f)));
	vt.push_back(stx_Material(textureroofName.c_str()+1, D3DXFROMWINEVECTOR3(1.0f, 0.0f, 0.0f)));
	vt.push_back(stx_Material(texturewindowName.c_str()+1, D3DXFROMWINEVECTOR3(0.5f, 0.5f, 0.5f)));
	vt.push_back(stx_Material(texturedoorName.c_str()+1, D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f)));
#endif
	std::string inst=applyRules(rules, axiom);

	LOG_PRINT("axiom=%s\n", axiom.c_str());
	LOG_PRINT("inst=%s\n", inst.c_str());
#if 1
	instv_=stx_split(inst, ":Ry");
	if(instv_.size())
	for(unsigned int i=0;i<instv_.size();i++)
	{
		if(instv_[i][0]==':')
			instv_[i]=instv_[i].substr(1,instv_[i].length());
		else if((instv_[i][instv_[i].length()-2]==':') &&(instv_[i][instv_[i].length()-1]=='R'))
			instv_[i]=instv_[i].substr(0,instv_[i].length()-2);
		LOG_PRINT("instv_[%d]=%s\n", i, instv_[i].c_str());
	}
#else
	{
		std::vector<std::string> v=regexmatch5(inst);
		LOG_PRINT("v=%d\n", v.size());
		for(unsigned int i=0;i<v.size();i++)
			LOG_PRINT("v[%d]=%s\n", i, v[i].c_str());
	}
	/*
	instv_=stx_split(inst, ":");
	if(instv_.size())
	for(unsigned int i=0;i<instv_.size();i++)
	{
		unsigned int j=0;
		if(!i)
			j=2;
		if(instv_[i][0]==':')
			instv_[i]=instv_[i].substr(1+j,instv_[i].length());
	} */
	for(unsigned int i=0;i<instv_.size();i++)
	{
		LOG_PRINT("instv_[%d]=%s\n", i, instv_[i].c_str());
	}
#endif
	//stx_exit(0);
	return 0;
}

void render()
{
	mTurtle.Clear();

	float mAngleX=45.0f;
	float mAngleY=45.0f;
	stx_GetAngles(mAngleX, mAngleY, 0.25f);

	D3DXFROMWINEVECTOR3 black(0.0f, 0.0f, 0.0f);
	D3DXFROMWINEVECTOR3 gray(0.5f, 0.5f, 0.5f);

	D3DXFROMWINEVECTOR3 LightGray=stx_GetRGB(eLightGray);

		D3DXFROMWINEMATRIX I, R, R2, S, T;
		D3DXFROMWINEMatrixRotationYawPitchRoll(&R,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f);
		D3DXFROMWINEMatrixRotationYawPitchRoll(&R2,
		                            D3DXFROMWINEToRadian(mAngleX+90.0f),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f);

	D3DXFROMWINEMatrixScaling(&S, 0.5f, 0.5f, 0.5f);
	D3DXFROMWINEMatrixIdentity(&MVP);
	D3DXFROMWINEMatrixIdentity(&I);
		D3DXFROMWINEMatrixTranslation(&T, 0.0f, 0.0f, 0.0f);
/*
 D3DXFROMWINEVec3TransformCoord(D3DXFROMWINEVECTOR3 *pout, CONST D3DXFROMWINEVECTOR3 *pv, CONST D3DXFROMWINEMATRIX *pm);
 D3DXFROMWINEVec3TransformCoordArray(D3DXFROMWINEVECTOR3 *pout, UINT outstride, CONST D3DXFROMWINEVECTOR3 *pv, UINT vstride, CONST D3DXFROMWINEMATRIX *pm, UINT n);
*/
	MVP=R*S;
{	
	IRenderer::GetRendererInstance()->Begin(PRIM_TRIANGLES);
    float y=-1.0f; // House Floor
    mTurtle.Vertex3f(-1.0f, y, -1.0f);
    mTurtle.TexCoord2f(0.0f,  0.0f);
    mTurtle.Color3fv(&black.x);
    mTurtle.Vertex3f(-1.0f, y, 1.0f);
    mTurtle.TexCoord2f(0.0f, 1.0f);
    mTurtle.Color3fv(&black.x);
    mTurtle.Vertex3f(1.0f, y, 1.0f);
    mTurtle.TexCoord2f(1.0f, 1.0f);
    mTurtle.Color3fv(&black.x);
    
    inds.push_back(texid);
    mTurtle.Vertex3f(-1.0f, y, -1.0f);
    mTurtle.TexCoord2f(0.0f,  0.0f);
    mTurtle.Color3fv(&black.x);
    mTurtle.Vertex3f(1.0f, y, 1.0f);
    mTurtle.TexCoord2f(1.0f, 1.0f);
    mTurtle.Color3fv(&black.x);
    mTurtle.Vertex3f(1.0f, y, -1.0f);
    mTurtle.TexCoord2f(1.0f, 0.0f);
    mTurtle.Color3fv(&black.x);
    
    inds.push_back(texid);
    IRenderer::GetRendererInstance()->End(texture, &MVP);

}
{
    texid=1;
    IRenderer::GetRendererInstance()->Begin(PRIM_TRIANGLES);
    float y=1.0f; // House Roof
#if 0 // Ridge Roof
    mTurtle.Vertex3f(-1.0f, y, -1.0f);
    mTurtle.TexCoord2f(0.0f,  0.0f);
    mTurtle.Color3fv(&LightGray.x);
    mTurtle.Vertex3f(-1.0f, y, 1.0f);
    mTurtle.TexCoord2f(0.0f, 1.0f);
    mTurtle.Color3fv(&LightGray.x);
    mTurtle.Vertex3f(0.0f, y+0.25f, 0.0f);
    mTurtle.TexCoord2f(1.0f, 1.0f);
    mTurtle.Color3fv(&LightGray.x);
    
    inds.push_back(texid);

    mTurtle.Vertex3f(1.0f, y, -1.0f);
    mTurtle.TexCoord2f(0.0f,  0.0f);
    mTurtle.Color3fv(&LightGray.x);
    mTurtle.Vertex3f(1.0f, y, 1.0f);
    mTurtle.TexCoord2f(0.0f, 1.0f);
    mTurtle.Color3fv(&LightGray.x);
    mTurtle.Vertex3f(0.0f, y+0.25f, 0.0f);
    mTurtle.TexCoord2f(1.0f, 1.0f);
    mTurtle.Color3fv(&LightGray.x);
    
    inds.push_back(texid);

    mTurtle.Vertex3f(-1.0f, y, -1.0f);
    mTurtle.TexCoord2f(0.0f,  0.0f);
    mTurtle.Color3fv(&LightGray.x);
    mTurtle.Vertex3f(1.0f, y, -1.0f);
    mTurtle.TexCoord2f(0.0f, 1.0f);
    mTurtle.Color3fv(&LightGray.x);
    mTurtle.Vertex3f(0.0f, y+0.25f, 0.0f);
    mTurtle.TexCoord2f(1.0f, 1.0f);
    mTurtle.Color3fv(&LightGray.x);
    
    inds.push_back(texid);

    mTurtle.Vertex3f(-1.0f, y, 1.0f);
    mTurtle.TexCoord2f(0.0f,  0.0f);
    mTurtle.Color3fv(&LightGray.x);
    mTurtle.Vertex3f(1.0f, y, 1.0f);
    mTurtle.TexCoord2f(0.0f, 1.0f);
    mTurtle.Color3fv(&LightGray.x);
    mTurtle.Vertex3f(0.0f, y+0.25f, 0.0f);
    mTurtle.TexCoord2f(1.0f, 1.0f);
    mTurtle.Color3fv(&LightGray.x);
    
    inds.push_back(texid);
    IRenderer::GetRendererInstance()->End(textureroof, &MVP);
#else // Flat Roof
    mTurtle.Vertex3f(-1.0f, y, -1.0f);
    mTurtle.TexCoord2f(0.0f,  0.0f);
    mTurtle.Color3fv(&gray.x);
    mTurtle.Vertex3f(-1.0f, y, 1.0f);
    mTurtle.TexCoord2f(0.0f, 1.0f);
    mTurtle.Color3fv(&gray.x);
    mTurtle.Vertex3f(1.0f, y, 1.0f);
    mTurtle.TexCoord2f(1.0f, 1.0f);
    mTurtle.Color3fv(&gray.x);
    
    inds.push_back(texid);
    mTurtle.Vertex3f(-1.0f, y, -1.0f);
    mTurtle.TexCoord2f(0.0f,  0.0f);
    mTurtle.Color3fv(&gray.x);
    mTurtle.Vertex3f(1.0f, y, 1.0f);
    mTurtle.TexCoord2f(1.0f, 1.0f);
    mTurtle.Color3fv(&gray.x);
    mTurtle.Vertex3f(1.0f, y, -1.0f);
    mTurtle.TexCoord2f(1.0f, 0.0f);
    mTurtle.Color3fv(&gray.x);
    
    inds.push_back(texid);
    IRenderer::GetRendererInstance()->End(textureroof, &MVP);
#endif
}
#if 1
	if(instv_.size())
	{
    texid=0;
    IRenderer::GetRendererInstance()->Begin(PRIM_TRIANGLES);
	drawHouseFacade(instv_[0], MVP, -1.0f);
	drawHouseFacade(instv_[1], MVP, 1.0f);
	IRenderer::GetRendererInstance()->End(texture, &MVP);
    texid=0;
	IRenderer::GetRendererInstance()->Begin(PRIM_TRIANGLES);
	MVP=R2*S;
	drawHouseFacade(instv_[2], MVP, -1.0f);
	drawHouseFacade(instv_[3], MVP, 1.0f);
	IRenderer::GetRendererInstance()->End(texture, &MVP);
	}
	else
	{LOG_PRINT(" instv_.size()==0 !\n");}
#elif 0
	int s=-1.0f;
	if(instv_.size())
	for(unsigned int i=0;i<4;i++)
	{
		D3DXFROMWINEMatrixRotationYawPitchRoll(&R,
		                            D3DXFROMWINEToRadian(mAngleX)+i*D3DXFROMWINE_PI/2.0f,
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f);
		MVP=R*S;
    		texid=0;
		drawHouseFacade(instv_[i], MVP, s);
		s*=-1.0f;
	}
	#if 0
	for(unsigned int i=1;i<4;i+=2)
	{
		D3DXFROMWINEMatrixRotationYawPitchRoll(&R,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY)+i*D3DXFROMWINE_PI/2.0f,
		                            0.0f);
		MVP=R*S*T;
		drawQuad(MVP);
	}
	#endif
#endif

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
	//mTurtle.mVertices.clear();
#ifdef __WRITEXFILE__
	LOG_PRINT("inds.size()=%d\n", inds.size())
	stx_Materials_(vt, inds, mesh_);
	stx_writeXFile("/ProceduralHouse.x", mesh_);//, mTurtle.mVertices, mTurtle.mNormals, mTurtle.mTexCoords, inds2, textures);
#endif
	mTurtle.Clear();
	//stx_exit(0);
}
};
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
#endif

