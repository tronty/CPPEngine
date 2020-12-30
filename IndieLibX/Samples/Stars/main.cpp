/*
  Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#if 1
#define Vec3TransformCoord D3DXFROMWINEVec3TransformCoord
#define Vec3ProjectArray D3DXFROMWINEVec3ProjectArray
#define Vec3Project D3DXFROMWINEVec3Project
#else
#define Vec3TransformCoord
#define Vec3ProjectArray
#define Vec3Project
#endif

#include <libnova/solar.h>
#include <libnova/lunar.h>

#include <libnova/mercury.h>
#include <libnova/venus.h>
#include <libnova/earth.h>
#include <libnova/mars.h>
#include <libnova/jupiter.h>
#include <libnova/saturn.h>
#include <libnova/uranus.h>
#include <libnova/neptune.h>
#include <libnova/pluto.h>

#include <libnova/sidereal_time.h>
#include <libnova/julian_day.h>
#include <libnova/rise_set.h>
#include <libnova/transform.h>
#include <libnova/utility.h>

const int NSTARS=9111;
float g_CameraSpeed;
D3DXFROMWINEVECTOR3 vO;

ShaderID shd3=-1;
VertexFormatID vf3=-1;

ShaderID shd4=-1;
VertexFormatID vf4=-1;

const int NCONSTELLATIONFIGURES=11;
char constellationFigureFiles[NCONSTELLATIONFIGURES][20]={
"dhmenz11",
"Karkoschka_cf",
"rukl_cf",
"Skywatch8",
"hareyv8",
"dhmenzv8",
"hareyv5",
"dhmenzv5",
"Skywatch",
"harey",
"dhmenzel"
};
int figureFileID=0;
struct FigureVertex2
{
	int cmd,id;
};
FigureVertex2 constellationFigures[9820];//9968-12-136
int constellationFigureStart[NCONSTELLATIONFIGURES];
int constellationFigureNum2[NCONSTELLATIONFIGURES];

const int NCONSTELLATIONS=89;
const int NCONSTELLATIONBORDERS=1566;
struct BorderVertex2
{
	float ra,dec;
};
BorderVertex2 constellationBorders[NCONSTELLATIONBORDERS];
int constellationBorderNum[NCONSTELLATIONS];
D3DXFROMWINEVECTOR3 constellationborders[NCONSTELLATIONBORDERS];
D3DXFROMWINEVECTOR3 constellationscreenborders[NCONSTELLATIONBORDERS];

#define SKY_RADIUS 1.0f

D3DXFROMWINEVECTOR3 g_CameraPosition;
D3DXFROMWINEMATRIX I;
D3DXFROMWINEMATRIX g_CameraProjMatrix;
D3DXFROMWINEMATRIX g_CameraViewMatrix;
D3DXFROMWINEMATRIX mvp;
__DWORD__ stride;
D3DFROMWINEVIEWPORT9 viewport;
D3DXFROMWINEMATRIX matProj;
D3DXFROMWINEMATRIX matView;
D3DXFROMWINEMATRIX matWorld;
/*
enum x {
    PRIM_LINES,
    PRIM_LINE_STRIP,
};*/
char names[10][256]={
"Sun",
"Moon",
"Mercury",
"Venus",
"Mars",
"Jupiter",
"Saturn",
"Uranus",
"Neptunus",
"Pluto"
};
char constellationnames[NCONSTELLATIONS][25]={
"Andromeda",
"Antlia",
"Apus",
"Aquarius",
"Aquila",
"Ara",
"Aries",
"Auriga",
"Bootes",
"Caelum",
"Camelopardus",
"Cancer",
"Canes Venatici",
"Canis Major",
"Canis Minor",
"Capricornus",
"Carina",
"Cassiopeia",
"Centaurus",
"Cepheus",
"Cetus",
"Chamaeleon",
"Circinus",
"Columba",
"Coma Berenices",
"Corona Australis",
"Corona Borealis",
"Corvus",
"Crater",
"Crux",
"Cygnus",
"Delphinus",
"Dorado",
"Draco",
"Equuleus",
"Eridanus",
"Fornax",
"Gemini",
"Grus",
"Hercules",
"Horologium",
"Hydra",
"Hydrus",
"Indus",
"Lacerta",
"Leo",
"Leo Minor",
"Lepus",
"Libra",
"Lupus",
"Lynx",
"Lyra",
"Mensa",
"Microscopium",
"Monoceros",
"Musca",
"Norma",
"Octans",
"Ophiuchus",
"Orion",
"Pavo",
"Pegasus",
"Perseus",
"Phoenix",
"Pictor",
"Pisces",
"Pisces Austrinus",
"Puppis",
"Pyxis",
"Reticulum",
"Sagitta",
"Sagittarius",
"Scorpius",
"Sculptor",
"Scutum",
"Serpens",
"Sextans",
"Taurus",
"Telescopium",
"Triangulum",
"Triangulum Australe",
"Tucana",
"Ursa Major",
"Ursa Minor",
"Vela",
"Virgo",
"Volans",
"Vulpecula"};

char constellationabbr[NCONSTELLATIONS][4]={"And","Ant","Aps","Aqr","Aql","Ara","Ari","Aur","Boo","Cae","Cam","Cnc","CVn","CMa","CMi","Cap","Car","Cas","Cen","Cep","Cet",
		"Cha","Cir","Col","Com","CrA","CrB","Crv","Crt","Cru","Cyg","Del","Dor","Dra","Equ","Eri","For","Gem","Gru","Her","Hor","Hya",
		"Hyi","Ind","Lac","Leo","LMi","Lep","Lib","Lup","Lyn","Lyr","Men","Mic","Mon","Mus","Nor","Oct","Oph","Ori","Pav","Peg","Per",
		"Phe","Pic","Psc","PsA","Pup","Pyx","Ret","Sge","Sgr","Sco","Scl","Sct","Ser","Sex","Tau","Tel","Tri","TrA","Tuc","UMa","UMi",
		"Vel","Vir","Vol","Vul"
};

D3DXFROMWINEVECTOR3 constellationpositions[NCONSTELLATIONS];
D3DXFROMWINEVECTOR3 constellationscreenpositions[NCONSTELLATIONS];

// Particle emitter types
enum EMITTERS
{
	EMITTER_CLOUD,
	EMITTER_VEGETATION,
	EMITTER_PEOPLE,
	EMITTER_SKY,
	EMITTER_ARBARO,
};
// Particle types and # of particle types
enum PARTICLES
{
	PARTICLE_TREE1,
	PARTICLE_TREE2,
	PARTICLE_TREE3,
	PARTICLE_TREE4,
	PARTICLE_TREE5,
	PARTICLE_TREE6,
	PARTICLE_TREE7,
	PARTICLE_TREE8,
	PARTICLE_TREE9,

	PARTICLE_EARTH,
	PARTICLE_SUN,
	PARTICLE_MOON,
	PARTICLE_STAR,
	PARTICLE_MERCURY,
	PARTICLE_VENUS,
	PARTICLE_MARS,
	PARTICLE_JUPITER,
	PARTICLE_SATURN,
	PARTICLE_URANUS,
	PARTICLE_NEPTUNE,
	PARTICLE_PLUTO,

	PARTICLE_ARBARO1,
	PARTICLE_ARBARO2,
	PARTICLE_ARBARO3,
	PARTICLE_ARBARO4,
	PARTICLE_ARBARO5,
	PARTICLE_ARBARO6,
	PARTICLE_ARBARO7,
	PARTICLE_ARBARO8,
	PARTICLE_ARBARO9,
	PARTICLE_ARBARO10,
	PARTICLE_ARBARO11,
	PARTICLE_ARBARO12,
	PARTICLE_ARBARO13,
	PARTICLE_ARBARO14,
	PARTICLE_ARBARO15,
	PARTICLE_ARBARO16,
	//PARTICLE_ARBARO17,
	//PARTICLE_ARBARO18,

	PARTICLE_SMOKE,

	PARTICLE_PEOPLE1,
	PARTICLE_PEOPLE2,
	NUM_PARTICLE_TYPES
};

struct CatalogElement
{
	char name[11];
	char varname[11];
//	char remark[128];
	float ra,dec,mag;
};
// Vertex structure for vertex-shader particles
#if ASM
typedef struct {
  D3DXFROMWINEVECTOR3 vecPos;	  // Coordinates of particle
  D3DXFROMWINEVECTOR2 vecOffset;  // Corner vertex offset coordinates
  D3DXFROMWINEVECTOR4       Diffuse;	  // Diffuse color of particle
  D3DXFROMWINEVECTOR2       uv;	  // Texture coordinates
} sShaderVertex;
#else
typedef struct {
  D3DXFROMWINEVECTOR3 vecPos;	  // Coordinates of particle
  D3DXFROMWINEVECTOR4       Diffuse;	  // Diffuse color of particle
  D3DXFROMWINEVECTOR2       uv;	  // Texture coordinates
  D3DXFROMWINEVECTOR2 vecOffset;  // Corner vertex offset coordinates
  D3DXFROMWINEVECTOR4 allowedRotDir;
} sShaderVertex;
#endif
// Particle container class
class cParticle
{
  public:
    __WORD__	m_Type; 	// Type of particle
    D3DXFROMWINEVECTOR3 m_vecPos;	// Position of particle
	D3DXFROMWINEVECTOR3 m_vecVelocity;	// Velocity of particle

    __WORD__	m_Life; 	// Life of particle in ms
    float	m_Size; 	// Size of particle

    __WORD__	m_Color;	// Diffuse color of particle
    D3DXFROMWINEVECTOR4 m_allowedRotDir;

    char	m_description[256];

    cParticle *m_Prev;		// Prev particle in linked list
    cParticle *m_Next;		// Next particle in linked list

  public:
    // Constructor and destructor to clear/release data
    cParticle()  { m_Prev = 0; m_Next = 0;		}
    ~cParticle() { delete m_Next; m_Next = 0; m_Prev = 0; }
};

// Particle emitter class
class cParticleEmitter
{
  public:
    sShaderVertex *Ptr;
    sShaderVertex *Ptr0;
	__WORD__ *Ptri;
	  __WORD__ start;
    Image3 atlas0;

    __WORD__		    m_EmitterType;   // Type of emitter

    D3DXFROMWINEVECTOR3	    m_vecPosition;   // Position of emitter
    __WORD__		    m_NumParticles;  // Max # particles in buffer
    cParticle		   *m_Particles;     // Particle list

    static __WORD__	    m_RefCount;      // Class reference count

    std::vector<sShaderVertex> m_VB;            // Particles' vertex buffer
    std::vector<__WORD__>  m_IB;            // Particles' index buffer
     static ShaderID	   m_pEffect;
    static ShaderID m_pShader;	 // Vertex shader
    static VertexFormatID m_pDecl; // Vertex decl
    TextureID m_pAtlasTexture;

    static bool initVegetation;
    static bool initVegetation2;
	static bool initSky;
#ifdef __VERTEXCACHE__
	static CVertexCache* m_vertexCache;
	sShaderVertex *pVertices;
	__WORD__* pIndices;
#endif
  public:
	void RenderStars();
    bool Create(
		D3DXFROMWINEVECTOR3 vecPosition=D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f),
		__WORD__ EmitterType=EMITTER_SKY,
		__WORD__ NumParticlesPerBuffer=9107){return false;}
    void Free();

    void Add(__WORD__ Type, D3DXFROMWINEVECTOR3& vecPos, float Size,
	     __WORD__ Color=0, __WORD__ Life=0,
	     D3DXFROMWINEVECTOR3 vecVelocity=D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f), D3DXFROMWINEVECTOR4 allowedRotDir=D3DXFROMWINEVECTOR4(0.0f,0.0f,0.0f,0.0f),char* decription=0)
{
  // Allocate a particle object and add to head of list
  cParticle *Particle = new cParticle();
  Particle->m_Prev = 0;
  if((Particle->m_Next = m_Particles))
    m_Particles->m_Prev = Particle;
  m_Particles = Particle;

  // Set particle data
  Particle->m_Type   = Type-start;

  Particle->m_vecPos = (vecPos);

  Particle->m_Size   = Size;
  Particle->m_Color  = Color;
  Particle->m_Life   = Life;
  Particle->m_vecVelocity = (vecVelocity);
  Particle->m_allowedRotDir=allowedRotDir;
  /* ??? if(description)
  strcpy(Particle->m_description,description);
  else */
  strcpy(Particle->m_description,"");
}
	     
    void ClearAll();

    // Functions to prepare for particle rendering, wrap up rendering,
    // and to render a batch of particles.
    bool Begin();//(D3DXFROMWINEMATRIX *matView, D3DXFROMWINEMATRIX *matProj,D3DXFROMWINEVECTOR4& xCamPos=D3DXFROMWINEVECTOR4(0.0f,0.0f,0.0f,1.0f));
    void End();
    void Render();

 D3DXFROMWINEVECTOR3 solarsystempositions[10];
D3DXFROMWINEVECTOR3 screenpositions[10];

//	CTerrain* m_pTerrain;
	D3DXFROMWINEVECTOR3 CENTER;
	D3DXFROMWINEVECTOR2 SIZE;
	int SEED;
	int atlasTextureCount;
	int iatlassize;//=1024;//768
	int isubtexturesize;//=256;
	int AddTextureToAtlas(Image3& atlas,Image3& img,float gamma = 1.0f);
	void MakeAtlas(Image3& atlas);
	void init();
#if 0
	cParticleEmitter(CTerrain* pTerrain,int numParticles=1000,D3DXFROMWINEVECTOR3 center=vO,D3DXFROMWINEVECTOR2 size=D3DXFROMWINEVECTOR2(1.0f,1.0f),int seed=0)
	{
		m_pTerrain=pTerrain;
		m_NumParticles = numParticles;
		CENTER=center;
		SIZE=size;
		SEED=seed;
		stx_srand(seed);


  // Clear object pointers
  m_VB		 = -1;
  m_IB		 = -1;

  // Clear particle data
  m_Particles	 = 0;
  if(numParticles>0)
  {
		AddVegetation(m_pTerrain,numParticles,center,size);
		//AddArbaro(m_pTerrain,numParticles,center,size);
		init("");
  }
	}
#endif
	cParticleEmitter(int seed=0)
	{
		m_NumParticles = 9107;
		CENTER=D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f);
		SIZE=D3DXFROMWINEVECTOR2(1.0f,1.0f);
		SEED=seed;
		stx_srand(seed);
        m_Particles	 = 0;
#if 0
    		Ptr=&m_VB[0];
    		Ptr0=&m_VB[0];
		Ptri=&m_IB[0];

  // Clear object pointers
  m_VB.clear();
  m_IB.clear();

  // Clear particle data
#endif
	  AddSky();
	}
	virtual ~cParticleEmitter()
	{
	  //Free();
	  }
	__WORD__ m_CountPolys;
	__WORD__ CountPolys()
	{
	      return m_CountPolys;
	}
	void applyGamma( unsigned char *buffer, size_t size, unsigned char bpp, float gamma = 1.0f );
	void applyGamma( Image3& image, float gamma = 1.0f );
	void AddSky()
	{
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);
		D3DXFROMWINEVECTOR3 vO(0.0f,0.0f,0.0f);
		Create(	vO, EMITTER_SKY);
	
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);
		ReadCatalog();
	
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);
	}
#if 0
	void AddVegetation(CTerrain* pTerrain,int numVegetation,D3DXFROMWINEVECTOR3 center=vO,D3DXFROMWINEVECTOR2 size=D3DXFROMWINEVECTOR2(1.0f,1.0f),int seed=0)
	{
		Create(	vO, EMITTER_VEGETATION,numVegetation);
	/*
		Distribute foliage over the terrain. To make the distribution fairly even a KD-tree is used
		to check whether an incoming location is too close to any existing location. This way all
		pieces of foliage are guaranteed to be at least maxDist distance apart.
	*/
	KDTree <float, 2> tree;
	float maxDist = 1.5f / sqrtf(float(numVegetation));

	stx_srand(seed);
	__WORD__ Type = PARTICLE_TREE1;
	for (int i = 0; i < numVegetation; i++){
		float tx, tz;

		do {
			tx = float(stx_rand()) / RAND_MAX * 2 - 1;
			tz = float(stx_rand()) / RAND_MAX * 2 - 1;
		} while (tree.hasPointWithin(float2(tx, tz), maxDist));
		tree.add(float2(tx, tz));

		D3DXFROMWINEVECTOR3 pos=center+D3DXFROMWINEVECTOR3(tx*size.x,pTerrain->m_map[0].GetExactHeightAt(center.x+tx*size.x,center.z+tz*size.y),tz*size.y);
		/*
		//LOG_PRINT("Particle i=%d\n",i);
		//LOG_PRINT("pos.x=%f\n",pos.x);
		//LOG_PRINT("pos.y=%f\n",pos.y);
		//LOG_PRINT("pos.z=%f\n",pos.z);
		*/
			    if(Type<PARTICLE_TREE5)
					  Add(Type++,
							&(pos+D3DXFROMWINEVECTOR3(0.0f,25.0f,0.0f)),
							50.0f, 0xFFFFFFFF, 0, &vO,D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f,0.0f));
			   else
					 Add(Type++,
			&(pos+D3DXFROMWINEVECTOR3(0.0f,12.5f,0.0f)),
				    25.0f, 0xFFFFFFFF, 0, &vO,D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f,0.0f));
	      if(Type > PARTICLE_TREE9)
						       Type = PARTICLE_TREE1;
	}
	}
	void AddArbaro(CTerrain* pTerrain,int numVegetation,D3DXFROMWINEVECTOR3 center=vO,D3DXFROMWINEVECTOR2 size=D3DXFROMWINEVECTOR2(1.0f,1.0f),int seed=0)
	{
		Create(	vO, EMITTER_ARBARO,numVegetation);
	/*
		Distribute foliage over the terrain. To make the distribution fairly even a KD-tree is used
		to check whether an incoming location is too close to any existing location. This way all
		pieces of foliage are guaranteed to be at least maxDist distance apart.
	*/
	KDTree <float, 2> tree;
	float maxDist = 1.5f / sqrtf(float(numVegetation));

	stx_srand(seed);
	__WORD__ Type = PARTICLE_TREE1;
	for (int i = 0; i < numVegetation; i++){
		float tx, tz;

		do {
			tx = float(stx_rand()) / RAND_MAX * 2 - 1;
			tz = float(stx_rand()) / RAND_MAX * 2 - 1;
		} while (tree.hasPointWithin(float2(tx, tz), maxDist));
		tree.add(float2(tx, tz));

		D3DXFROMWINEVECTOR3 pos=center+D3DXFROMWINEVECTOR3(tx*size.x,pTerrain->m_map[0].GetExactHeightAt(center.x+tx*size.x,center.z+tz*size.y),tz*size.y);
		/*
		//LOG_PRINT("Particle i=%d\n",i);
		//LOG_PRINT("pos.x=%f\n",pos.x);
		//LOG_PRINT("pos.y=%f\n",pos.y);
		//LOG_PRINT("pos.z=%f\n",pos.z);
		*/
			    if(Type<PARTICLE_ARBARO1)
					  Add(Type++,
							&(pos+D3DXFROMWINEVECTOR3(0.0f,25.0f,0.0f)),
							50.0f, 0xFFFFFFFF, 0, &vO,D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f,0.0f));
			   else
					 Add(Type++,
			&(pos+D3DXFROMWINEVECTOR3(0.0f,12.5f,0.0f)),
				    25.0f, 0xFFFFFFFF, 0, &vO,D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f,0.0f));
	      if(Type > PARTICLE_ARBARO2)
						       Type = PARTICLE_ARBARO1;
	}
	}
#endif
	void render();
	void print_date (char * title, struct ln_zonedate* date);
	void EqutoHoriz(char* name,double JD,struct ln_equ_posn object, struct ln_lnlat_posn observer);
	int sun (double JD,struct ln_lnlat_posn observer,struct ln_equ_posn equ,struct ln_hrz_posn hrz);
	int moon (double JD,struct ln_lnlat_posn observer,struct ln_equ_posn equ,struct ln_hrz_posn hrz);
	int planet (double JD,struct ln_lnlat_posn observer,PARTICLES body,struct ln_equ_posn equ,struct ln_hrz_posn hrz,double mag);
	//int main (int argc, char * argv[]);
	void ReadCatalog();
	D3DXFROMWINEVECTOR3 CalculatePosition(double aequra, double aequdec,double JD=0);
	int Pick();
	bool LineSphereIntersect(const D3DXFROMWINEVECTOR3 &p, const D3DXFROMWINEVECTOR3 &d,double r, double &i1, double &i2);
	bool LineSphereIntersectNN(const D3DXFROMWINEVECTOR3 &p, const D3DXFROMWINEVECTOR3 &d,double r, double &i1, double &i2);
	int initEffect();
	void ReadConstellationLines(const char* fn);
};
cParticleEmitter* m_pSkyEmitter=0;

int cParticleEmitter::sun (double JD,struct ln_lnlat_posn observer,struct ln_equ_posn equ,struct ln_hrz_posn hrz)
{
	struct ln_helio_posn pos;

	/* geometric coordinates */
	ln_get_solar_geom_coords (JD, &pos);

	/* ra, dec */
	ln_get_solar_equ_coords (JD, &equ);

	//EqutoHoriz("Sun",JD,equ, observer);
	ln_get_hrz_from_equ (&equ, &observer, JD, &hrz);
	solarsystempositions[0]=CalculatePosition(equ.ra,equ.dec,JD);
    D3DXFROMWINEVECTOR4 v(0.0f, 1.0f, 0.0f,0.0f);
	Add(PARTICLE_SUN,
							solarsystempositions[0],
							50.0f, 0xFFFFFFFF, 0, vO,v,"Sun");
	return 0;
}
int cParticleEmitter::moon (double JD,struct ln_lnlat_posn observer,struct ln_equ_posn equ,struct ln_hrz_posn hrz)
{

	struct ln_rect_posn moon;
	struct ln_lnlat_posn ecl;

	/* get the lunar geopcentric position in km, earth is at 0,0,0 */
	ln_get_lunar_geo_posn (JD, &moon, 0);
	//LOG_PRINT("lunar x %f  y %f  z %f\n",moon.X, moon.Y, moon.Z);

	/* Long Lat */
	ln_get_lunar_ecl_coords (JD, &ecl, 0);
	//LOG_PRINT("lunar int %f	lat %f\n",ecl.lng, ecl.lat);

	/* RA, DEC */
	ln_get_lunar_equ_coords (JD, &equ);

	//EqutoHoriz("Moon",JD,equ, observer);
	ln_get_hrz_from_equ (&equ, &observer, JD, &hrz);
	solarsystempositions[1]=CalculatePosition(equ.ra,equ.dec,JD);
	Add(PARTICLE_MOON,
							solarsystempositions[1],
							50.0f, 0xFFFFFFFF, 0, vO,D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f,0.0f),"Moon");
	return 0;
}
int cParticleEmitter::planet (double JD,struct ln_lnlat_posn observer,PARTICLES body,struct ln_equ_posn equ,struct ln_hrz_posn hrz,double mag)
{

	struct ln_helio_posn pos;
	struct lnh_equ_posn hequ;
	double au;

	char name[256];
	switch(body)
	{
		case PARTICLE_MERCURY:
			stx_snprintf(name, 256, "Mercury");
			break;
		case PARTICLE_VENUS:
			stx_snprintf(name, 256, "Venus");
			break;
		case PARTICLE_EARTH:
			stx_snprintf(name, 256, "Earth");
			break;
		case PARTICLE_MARS:
			stx_snprintf(name, 256, "Mars");
			break;
		case PARTICLE_JUPITER:
			stx_snprintf(name, 256, "Jupiter");
			break;
		case PARTICLE_SATURN:
			stx_snprintf(name, 256, "Saturn");
			break;
		case PARTICLE_URANUS:
			stx_snprintf(name, 256, "Uranus");
			break;
		case PARTICLE_NEPTUNE:
			stx_snprintf(name, 256, "Neptune");
			break;
		case PARTICLE_PLUTO:
			stx_snprintf(name, 256, "Pluto");
			break;
	}

	/* longitude, latitude and radius vector */
		switch(body)
		{
			case PARTICLE_MERCURY:
				ln_get_mercury_helio_coords(JD, &pos);
				break;
			case PARTICLE_VENUS:
				ln_get_venus_helio_coords(JD, &pos);
				break;
			case PARTICLE_EARTH:
				ln_get_earth_helio_coords(JD, &pos);
				break;
			case PARTICLE_MARS:
				ln_get_mars_helio_coords(JD, &pos);
				break;
			case PARTICLE_JUPITER:
				ln_get_jupiter_helio_coords(JD, &pos);
				break;
			case PARTICLE_SATURN:
				ln_get_saturn_helio_coords(JD, &pos);
				break;
			case PARTICLE_URANUS:
				ln_get_uranus_helio_coords(JD, &pos);
				break;
			case PARTICLE_NEPTUNE:
				ln_get_neptune_helio_coords(JD, &pos);
				break;
			case PARTICLE_PLUTO:
				ln_get_pluto_helio_coords(JD, &pos);
				break;
		}
	STX_PRINT("%s L %f B %f R %f\n",name, pos.L, pos.B, pos.R);

	/* RA, DEC */
		switch(body)
		{
			case PARTICLE_MERCURY:
				ln_get_mercury_equ_coords (JD, &equ);
				break;
			case PARTICLE_VENUS:
				ln_get_venus_equ_coords (JD, &equ);
				break;
			case PARTICLE_EARTH:
				break;
			case PARTICLE_MARS:
				ln_get_mars_equ_coords (JD, &equ);
				break;
			case PARTICLE_JUPITER:
				ln_get_jupiter_equ_coords (JD, &equ);
				break;
			case PARTICLE_SATURN:
				ln_get_saturn_equ_coords (JD, &equ);
				break;
			case PARTICLE_URANUS:
				ln_get_uranus_equ_coords (JD, &equ);
				break;
			case PARTICLE_NEPTUNE:
				ln_get_neptune_equ_coords (JD, &equ);
				break;
			case PARTICLE_PLUTO:
				ln_get_pluto_equ_coords (JD, &equ);
				break;
		}
	ln_equ_to_hequ (&equ, &hequ);
	STX_PRINT("%s RA %d:%d:%f Dec %d:%d:%f\n",name, hequ.ra.hours, hequ.ra.minutes, hequ.ra.seconds, hequ.dec.degrees, hequ.dec.minutes, hequ.dec.seconds);

	//EqutoHoriz(name,JD,equ, observer);
	ln_get_hrz_from_equ (&equ, &observer, JD, &hrz);
	int ii=0;
		switch(body)
		{
			case PARTICLE_MERCURY:
				au = ln_get_mercury_magnitude (JD);
				ii=2;
				break;
			case PARTICLE_VENUS:
				au = ln_get_venus_magnitude (JD);
				ii=3;
				break;
			case PARTICLE_EARTH:
				au = 0;
				ii=0;
				break;
			case PARTICLE_MARS:
				au = ln_get_mars_magnitude (JD);
				ii=4;
				break;
			case PARTICLE_JUPITER:
				au = ln_get_jupiter_magnitude (JD);
				ii=5;
				break;
			case PARTICLE_SATURN:
				au = ln_get_saturn_magnitude (JD);
				ii=6;
				break;
			case PARTICLE_URANUS:
				au = ln_get_uranus_magnitude (JD);
				ii=7;
				break;
			case PARTICLE_NEPTUNE:
				au = ln_get_neptune_magnitude (JD);
				ii=8;
				break;
			case PARTICLE_PLUTO:
				au = ln_get_pluto_magnitude (JD);
				ii=9;
				break;
		}
		mag=au;
	STX_PRINT("%s -> magnitude %f\n",name,au);

	solarsystempositions[ii]=CalculatePosition(equ.ra,equ.dec,JD);
	Add(body,
							solarsystempositions[ii],
							50.0f, 0xFFFFFFFF, 0, vO,D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f,0.0f),name);
	#if 0

	/* Earth - Mars dist AU */
		switch(body)
		{
			case MERCURY:
				au = ln_get_mercury_earth_dist (JD);
				break;
			case VENUS:
				au = ln_get_venus_earth_dist (JD);
				break;
			case EARTH:
				au = 0;
				break;
			case MARS:
				au = ln_get_mars_earth_dist (JD);
				break;
			case JUPITER:
				au = ln_get_jupiter_earth_dist (JD);
				break;
			case SATURN:
				au = ln_get_saturn_earth_dist (JD);
				break;
			case URANUS:
				au = ln_get_uranus_earth_dist (JD);
				break;
			case NEPTUNE:
				au = ln_get_neptune_earth_dist (JD);
				break;
			case PLUTO:
				au = ln_get_pluto_earth_dist (JD);
				break;
		}
	STX_PRINT("%s -> Earth dist (AU) %f\n",name,au);

	/* Sun - Mars Dist AU */
		switch(body)
		{
			case MERCURY:
				au = ln_get_mercury_solar_dist (JD);
				break;
			case VENUS:
				au = ln_get_venus_solar_dist (JD);
				break;
			case EARTH:
				au = ln_get_earth_solar_dist (JD);
				break;
			case MARS:
				au = ln_get_mars_solar_dist (JD);
				break;
			case JUPITER:
				au = ln_get_jupiter_solar_dist (JD);
				break;
			case SATURN:
				au = ln_get_saturn_solar_dist (JD);
				break;
			case URANUS:
				au = ln_get_uranus_solar_dist (JD);
				break;
			case NEPTUNE:
				au = ln_get_neptune_solar_dist (JD);
				break;
			case PLUTO:
				au = ln_get_pluto_solar_dist (JD);
				break;
		}
	STX_PRINT("%s -> Sun dist (AU) %f\n",name,au);

	/* Mars disk, magnitude and phase */
		switch(body)
		{
			case MERCURY:
				au = ln_get_mercury_disk (JD);
				break;
			case VENUS:
				au = ln_get_venus_disk (JD);
				break;
			case EARTH:
				au = 0;
				break;
			case MARS:
				au = ln_get_mars_disk (JD);
				break;
			case JUPITER:
				au = ln_get_jupiter_disk (JD);
				break;
			case SATURN:
				au = ln_get_saturn_disk (JD);
				break;
			case URANUS:
				au = ln_get_uranus_disk (JD);
				break;
			case NEPTUNE:
				au = ln_get_neptune_disk (JD);
				break;
			case PLUTO:
				au = ln_get_pluto_disk (JD);
				break;
		}
	STX_PRINT("%s -> illuminated disk %f\n",name,au);
		switch(body)
		{
			case MERCURY:
				au = ln_get_mercury_magnitude (JD);
				break;
			case VENUS:
				au = ln_get_venus_magnitude (JD);
				break;
			case EARTH:
				au = 0;
				break;
			case MARS:
				au = ln_get_mars_magnitude (JD);
				break;
			case JUPITER:
				au = ln_get_jupiter_magnitude (JD);
				break;
			case SATURN:
				au = ln_get_saturn_magnitude (JD);
				break;
			case URANUS:
				au = ln_get_uranus_magnitude (JD);
				break;
			case NEPTUNE:
				au = ln_get_neptune_magnitude (JD);
				break;
			case PLUTO:
				au = ln_get_pluto_magnitude (JD);
				break;
		}
	STX_PRINT("%s -> magnitude %f\n",name,au);
		switch(body)
		{
			case MERCURY:
				au = ln_get_mercury_phase (JD);
				break;
			case VENUS:
				au = ln_get_venus_phase (JD);
				break;
			case EARTH:
				au = 0;
				break;
			case MARS:
				au = ln_get_mars_phase (JD);
				break;
			case JUPITER:
				au = ln_get_jupiter_phase (JD);
				break;
			case SATURN:
				au = ln_get_saturn_phase (JD);
				break;
			case URANUS:
				au = ln_get_uranus_phase (JD);
				break;
			case NEPTUNE:
				au = ln_get_neptune_phase (JD);
				break;
			case PLUTO:
				au = ln_get_pluto_phase (JD);
				break;
		}
	STX_PRINT("%s -> phase %f\n",name,au);

	bool r=false;
	switch(body)
	{
		case MERCURY:
			r=ln_get_mercury_rst (JD, &observer, &rst);
			break;
		case VENUS:
			r=ln_get_venus_rst (JD, &observer, &rst);
			break;
		case EARTH:
			r=false;
			break;
		case MARS:
			r=ln_get_mars_rst (JD, &observer, &rst);
			break;
		case JUPITER:
			r=ln_get_jupiter_rst (JD, &observer, &rst);
			break;
		case SATURN:
			r=ln_get_saturn_rst (JD, &observer, &rst);
			break;
		case URANUS:
			r=ln_get_uranus_rst (JD, &observer, &rst);
			break;
		case NEPTUNE:
			r=ln_get_neptune_rst (JD, &observer, &rst);
			break;
		case PLUTO:
			r=ln_get_pluto_rst (JD, &observer, &rst);
			break;
	}
		/* rise, set and transit time */
	if (r == 1)
		STX_PRINT("%s is circumpolar\n",name);
	else {
		ln_get_local_date (rst.rise, &rise);
		ln_get_local_date (rst.transit, &transit);
		ln_get_local_date (rst.set, &set);
		print_date ("Rise", &rise);
		print_date ("Transit", &transit);
		print_date ("Set", &set);
	}
	#endif
	return 0;
}

D3DXFROMWINEVECTOR3 cParticleEmitter::CalculatePosition(double aequra, double aequdec, double JD)
{

	float f=65.017f*float(D3DXFROMWINE_PI)/180.0f;
	float r=1000.0f;
	D3DXFROMWINEVECTOR3 position;

	//aequdec=-1.0f*aequdec+90.0f;
	//aequra/=2.0f;//???

	float equdec=float(aequdec)*float(D3DXFROMWINE_PI)/180.0f;
	float equra=float(aequra)*float(D3DXFROMWINE_PI)/180.0f;

	float theta2=equdec;//???
	float varphi2=equra;//???
if(0)
{
	//double t=0;//*D3DXFROMWINE_PI/180.0;
	double t=ln_get_mean_sidereal_time (JD);
	//double t=ln_get_apparent_sidereal_time (JD);

	float H = float(t - equra);//Local Hour Angle
	float a = asinf(sinf(equdec)*sinf(f) + cosf(equdec)*cosf(f)*cosf(H));//altitude
	float A = asinf(- sinf(H)*cosf(equdec) / cosf(a));//azimuth
	theta2=a;//???
	varphi2=A;//???
}

	#if 0
	position.x=r*cosf(varphi)*sinf(theta)+g_CameraPosition.x;
    position.z=r*cosf(theta)+g_CameraPosition.z;
    position.y=r*sinf(varphi)*sinf(theta)+g_CameraPosition.y;
    #elif 0
	position.x=r*cosf(varphi)*sinf(theta)+g_CameraPosition.x;
    position.y=r*sinf(varphi)*sinf(theta)+g_CameraPosition.y;
    position.z=r*cosf(theta)+g_CameraPosition.z;
    #elif 0
	position.x=r*sinf(varphi2)*cosf(theta2)+g_CameraPosition.x;
	position.z=r*sinf(varphi2)*sinf(theta2)+g_CameraPosition.z;
	position.y=r*cosf(varphi2)+g_CameraPosition.y;
#elif 0
	position.x= r * cosf(varphi2) * cosf(theta2)+g_CameraPosition.x;
	position.y= r * cosf(varphi2) * sinf(theta2)+g_CameraPosition.y;
	position.z= r * sinf(varphi2)+g_CameraPosition.z;
#elif 1
	position.x = r * cosf(theta2) * cosf(varphi2)+g_CameraPosition.x;
	position.z = r * cosf(theta2) * sinf(varphi2)+g_CameraPosition.z;
	position.y = r * sinf(theta2)+g_CameraPosition.y;// = should be identical to starting cZ value
#elif 0
	x=cosf(theta)*sinf(phi);
	y=sinf(theta)*sinf(phi);
			  z=cos(phi);
#endif
	      /*
	//LOG_PRINT("position.x=%f\n",position.x);
	//LOG_PRINT("position.y=%f\n",position.y);
	//LOG_PRINT("position.z=%f\n",position.z);
	    */
	return position;

}

//cParticleEmitter::
CatalogElement catalog[NSTARS];
D3DXFROMWINEVECTOR3 catalogV3[NSTARS];
D3DXFROMWINEVECTOR3 catalogV3_[NSTARS];


D3DXFROMWINEVECTOR3 CalculatePosition(double aequra, double aequdec, double JD)
{

	float f=65.017f*float(D3DXFROMWINE_PI)/180.0f;
	float r=1000.0f;
	D3DXFROMWINEVECTOR3 position;

	//aequdec=-1.0f*aequdec+90.0f;
	//aequra/=2.0f;//???

	float equdec=float(aequdec)*float(D3DXFROMWINE_PI)/180.0f;
	float equra=float(aequra)*float(D3DXFROMWINE_PI)/180.0f;

	float theta2=equdec;//???
	float varphi2=equra;//???

	position.x = r * cosf(theta2) * cosf(varphi2)+g_CameraPosition.x;
	position.z = r * cosf(theta2) * sinf(varphi2)+g_CameraPosition.z;
	position.y = r * sinf(theta2)+g_CameraPosition.y;// = should be identical to starting cZ value

	return position;

}

D3DXFROMWINEVECTOR3 GetPosition3(float varphi,float theta,float r=SKY_RADIUS)
{
	D3DXFROMWINEVECTOR3 position;
#if 1
	double JD;
	/* get Julian day from local time */
	JD = ln_get_julian_from_sys();	
	position =  //m_pSkyEmitter->
                CalculatePosition(varphi, theta, JD);
#else
	float varphi2=varphi*D3DXFROMWINE_PI/180.0f;
	float theta2=theta*D3DXFROMWINE_PI/180.0f;
	position.x = r * cosf(theta2) * cosf(varphi2)+g_CameraPosition.x;
	position.z = r * cosf(theta2) * sinf(varphi2)+g_CameraPosition.z;
	position.y = r * sinf(theta2)+g_CameraPosition.y;// = should be identical to starting cZ value
#endif
	return position;
}

void DrawText(const D3DXFROMWINEVECTOR2& aScreenPosition, const char* aText)
{
	#if 0
	D3DFROMWINEVIEWPORT9 viewport = { 0, 0, IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight(), 0, 1 };
	Vec3ProjectArray(m_pSkyEmitter->screenpositions, stride,m_pSkyEmitter->solarsystempositions, stride, &viewport, &g_CameraProjMatrix, &g_CameraViewMatrix, &mvp, 10);
	#endif
		 char buf[256];
		 stx_snprintf(buf, 256, "%s", aText);
	  	 IRenderer::GetRendererInstance()->drawText(buf, aScreenPosition.x, aScreenPosition.y,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());
}

void LoadBorders()
{
//09 27 37.0404|-24.5425186|ANT
	int c=0;
	int constellationID=0;
	int borderNum=0;
	int N=0;
	for(unsigned int i=0;i<NCONSTELLATIONS;i++)
		     constellationpositions[i]=D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f);
	char str[2000];
	std::string pName=stx_convertpath("/Stars/IAUConstellationBorders.txt");
	fstream file_op(pName.c_str(), ios::in);
	while(!file_op.eof())
	{
	      file_op.getline(str,2000);

			if(strcmp(";",str)==0)
			{
				constellationBorderNum[constellationID]=borderNum;
				borderNum=0;
				continue;
			}
            		N++;
			  char buf[12];
			  unsigned int equinox=0;
			  buf[0]=str[equinox];
			  buf[1]=str[equinox+1];
			  buf[2]='\0';
			  int rah;
			  int r=sscanf (buf,"%d",&rah);
			  if(r!=1)continue;
			  buf[0]=str[equinox+3];
			  buf[1]=str[equinox+4];
			  buf[2]='\0';
			  int ram;
			  r=sscanf (buf,"%d",&ram);
			  if(r!=1)continue;
			  buf[0]=str[equinox+6];
			  buf[1]=str[equinox+7];
			  buf[2]=str[equinox+8];
			  buf[3]=str[equinox+9];
			  buf[4]=str[equinox+10];
			  buf[5]=str[equinox+11];
			  buf[6]=str[equinox+12];
			  buf[7]='\0';
			  float ras;
			  r=sscanf (buf,"%f",&ras);
			  if(r!=1)continue;
			  float x=(((ras/60.0f)+float(ram))/60.0f+float(rah))*15.0f;

			  buf[0]=str[equinox+14];
			  buf[1]=str[equinox+15];
			  buf[2]=str[equinox+16];
			  buf[3]=str[equinox+17];
			  buf[4]=str[equinox+18];
			  buf[5]=str[equinox+19];

			  buf[6]=str[equinox+20];
			  buf[7]=str[equinox+21];
			  buf[8]=str[equinox+22];
			  buf[9]=str[equinox+23];

			  buf[10]=str[equinox+24];
			  buf[11]='\0';
			  float dec;
			  r=sscanf (buf,"%f",&dec);
			  if(r!=1)continue;
			  float y=dec;

			  constellationBorders[c].ra=x;
			  constellationBorders[c].dec=y;

			constellationborders[c].x=x;
			constellationborders[c].y=y;
			constellationscreenborders[c].x=x;
			constellationscreenborders[c].y=y;

			  buf[0]=str[equinox+26];
			  buf[1]=str[equinox+27];
			  buf[2]=str[equinox+28];
			  buf[3]='\0';
			  char chr[4];
			  r=sscanf (buf,"%s",&chr);
		
			  static std::string con_="";
			  if(con_!=std::string(&chr[0]))
			  {
			  	con_=std::string(&chr[0]);
			  	constellationID++;
			  	N=0;
			  }

			  D3DXFROMWINEVECTOR3 position=GetPosition3(x,y);
			  constellationpositions[constellationID]+=position;
		constellationBorderNum[constellationID]=N;
	      c++;
	      borderNum++;
	}
	file_op.close();

	for(unsigned int i=0;i<NCONSTELLATIONS;i++)
	{
		unsigned int constellationID=i;
		constellationpositions[i]/=constellationBorderNum[i];
		constellationscreenpositions[i]=constellationpositions[i];
       }
}

void LoadFigures()
{
/*
/Constellations/ConstellationFigures/dhmenz11/DHMENZEL.TXT
0 8960
1 8975
;
*/
	int c=0;
	int constellationID=0;
	int figureNum=0;
	int figureStart=0;
	char str[2000];

	std::string pName=stx_convertpath("/Stars/ConstellationFigures.txt");
	fstream file_op(pName.c_str(), ios::in);

	while(!file_op.eof())
	{
	      file_op.getline(str,2000);

			if(str[0]=='/')
			{
				continue;
			}
			if(str[0]==';')
			{
				constellationFigureNum2[constellationID]=figureNum;
				constellationFigureStart[constellationID]=figureStart;
				figureStart+=figureNum;
				figureNum=0;
				constellationID++;
				continue;
			}
			  char buf[15];
			  buf[0]=str[0];
			  buf[1]='\0';
			  int cmd;
			  int r=sscanf (buf,"%d",&cmd);
			  if(r!=1)
			  {
			    //LOG_PRINT("x:r=-1\n");
			    continue;
			  }
			  int x=cmd;

			  buf[0]=str[2];
			  buf[1]=str[3];
			  buf[2]=str[4];
			  buf[3]=str[5];
			  buf[4]='\0';
			  int id;
			  r=sscanf (buf,"%d",&id);
			  if(r!=1)
			  {
			    //LOG_PRINT("y:r=-1\n");
			    continue;
			  }
			  int y=id;

			  constellationFigures[c].cmd=x;
			  constellationFigures[c].id=y;

	      c++;
	      figureNum++;
	}
	file_op.close();
}
void  cParticleEmitter::ReadCatalog()
{

	double JD;
	/* get Julian day from local time */
	JD = ln_get_julian_from_sys();

	static float magmin=1.0f;
	static float magmax=1.0f;
	int equinoxB1900=60;
	int equinoxJ2000=75;
	int equinox=equinoxJ2000;
	int c=0;
	int c2=0;
	char str[2000];
	

	std::string pName=stx_convertpath("/Stars/catalog");

	fstream file_op(pName.c_str(), ios::in);

	while(!file_op.eof())
	{
	      file_op.getline(str,2000);
			  c++;

			  char buf[11];
			  char name0[11];
			  char varname[11];

			  name0[0]='\0';
			  varname[0]='\0';

			  buf[0]=str[4];
			  buf[1]=str[5];
			  buf[2]=str[6];
			  buf[3]=str[7];
			  buf[4]=str[8];
			  buf[5]=str[9];
			  buf[6]=str[10];
			  buf[7]=str[11];
			  buf[8]=str[12];
			  buf[9]=str[13];
			  buf[10]='\0';

			  strcpy(name0,buf);

			  buf[0]=str[51];
			  buf[1]=str[52];
			  buf[2]=str[53];
			  buf[3]=str[54];
			  buf[4]=str[55];
			  buf[5]=str[56];
			  buf[6]=str[57];
			  buf[7]=str[58];
			  buf[8]=str[59];
			  buf[9]='\0';

			  strcpy(varname,buf);

			  buf[0]=str[equinox];
			  buf[1]=str[equinox+1];
			  buf[2]='\0';

			  int rah;
			  int r=sscanf (buf,"%d",&rah);
	
			  if(r!=1)continue;
	
			  buf[0]=str[equinox+2];
			  buf[1]=str[equinox+3];
			  buf[2]='\0';

			  int ram;
			  r=sscanf (buf,"%d",&ram);
	
			  if(r!=1)continue;
	
			  buf[0]=str[equinox+4];
			  buf[1]=str[equinox+5];
			  buf[2]=str[equinox+6];
			  buf[3]=str[equinox+7];
			  buf[4]='\0';

			  float ras;
			  r=sscanf (buf,"%f",&ras);
	
			  if(r!=1)continue;
	
			  float x=(((ras/60.0f)+float(ram))/60.0f+float(rah))*15.0f;

			  buf[0]=str[equinox+8];
			  buf[1]=str[equinox+9];
			  buf[2]=str[equinox+10];
			  buf[3]='\0';

			  int ded;
			  r=sscanf (buf,"%d",&ded);
	
			  if(r!=1)continue;
	
			  buf[0]=str[equinox+11];
			  buf[1]=str[equinox+12];
			  buf[2]='\0';

			  int dem;
			  r=sscanf (buf,"%d",&dem);
	
			  if(r!=1)continue;
	
			  buf[0]=str[equinox+13];
			  buf[1]=str[equinox+14];
			  buf[2]='\0';

			  int des;
			  r=sscanf (buf,"%d",&des);
	
			  if(r!=1)continue;
	
			  float y=((float(des)/60.0f)+float(dem))/60.0f*((ded > 0) ? 1 : ((ded < 0) ? -1 : 0))+float(ded);
			  buf[0]=str[103];
			  buf[1]=str[104];
			  buf[2]=str[105];
			  buf[3]=str[106];
			  buf[4]=str[107];
			  buf[5]='\0';
			  float mag;
			  r=sscanf (buf,"%f",&mag);
			  if(r!=1)continue;

			  //magmin=0.010000
			  //magmax=7.960000
			  float z=(1.0f-(mag-0.01f)/7.95f)*7.5f+2.5f;//*43.0f;
			  catalogV3[c-1].x=x;
			  catalogV3[c-1].y=y;
			catalog[c-1].ra=x;
			catalog[c-1].dec=y;
			catalog[c-1].mag=mag;
			catalog[c-1].name[0]='\0';
			catalog[c-1].varname[0]='\0';
			  strcpy(catalog[c-1].name,name0);
			  strcpy(catalog[c-1].varname,varname);

			  magmin=std::min(magmin,mag);
			   magmax=std::max(magmax,mag);
        D3DXFROMWINEVECTOR3 v1=CalculatePosition(x,y,JD);
        D3DXFROMWINEVECTOR4 v2(0.0f, 1.0f, 0.0f,0.0f);
        Add(PARTICLE_STAR,
							v1,
							z, 0xFFFFFFFF, 0, vO, v2, str);
	c2++;

	}
	
	file_op.close();
/*
   1	      BD+44 4550      3 36042	       46	    000001.1+444022000509.9+451345114.44-16.88 6.70  +0.07 +0.08	 A1Vn		    -0.012-0.018      -018	195  4.2  21.6AC   3
9110	      BD+60 2667 225289 10962		   V567 Cas 235955.9+604526000506.2+611851117.40-01.06 5.80  -0.09 -0.32	 B8IVpHgMn	    +0.015+0.005      +014V	 50		    *
*/
	struct ln_lnlat_posn observer;
		/* observers location (Edinburgh), used to calc rst */
	observer.lat = 65.017;
	observer.lng = 25.467;

	ln_equ_posn equ;
	ln_hrz_posn hrz;
	double mag=0;

	ZeroMemory(&equ,sizeof(ln_equ_posn));
	ZeroMemory(&hrz,sizeof(ln_hrz_posn));

	sun (JD,observer,equ,hrz);
	moon (JD,observer,equ,hrz);
	planet (JD,observer,PARTICLE_MERCURY,equ,hrz,mag);
	planet (JD,observer,PARTICLE_VENUS,equ,hrz,mag);
	//planet (JD,observer,PARTICLE_EARTH,equ,hrz,mag);
	planet (JD,observer,PARTICLE_MARS,equ,hrz,mag);
	planet (JD,observer,PARTICLE_JUPITER,equ,hrz,mag);
	planet (JD,observer,PARTICLE_SATURN,equ,hrz,mag);
	planet (JD,observer,PARTICLE_URANUS,equ,hrz,mag);
	planet (JD,observer,PARTICLE_NEPTUNE,equ,hrz,mag);
	planet (JD,observer,PARTICLE_PLUTO,equ,hrz,mag);
	return;
}

bool PointInFrustum(float x, float y, float z)
{
      D3DXFROMWINEMATRIX matWorld;D3DXFROMWINEMatrixIdentity(&matWorld);
      D3DXFROMWINEMATRIX matProjection=g_CameraProjMatrix;
      D3DXFROMWINEMATRIX matView=g_CameraViewMatrix;

      // I chaned this line, so you could change the projection view here, adjusting
      // the near and far plane...alternatively you can just use the GetTransform
      // DX call, and use the pre-set one.

      //D3DXFROMWINEMatrixPerspectiveFovLH( &matProjection, D3DXFROMWINE_PI/2, 1.0f, 1.0f, 30.0f );
      D3DXFROMWINEMATRIX Cull = matWorld * matView * matProjection;
      D3DXFROMWINEVECTOR4 vNode = D3DXFROMWINEVECTOR4(x,y,z,1);
      D3DXFROMWINEVECTOR4 vTestVert;
      D3DXFROMWINEVec4Transform(&vTestVert, &vNode, &Cull);

      if( abs(vTestVert.x) > abs(vTestVert.w) ) // Outside our X Frustum Plane
	    return false;

      if( abs(vTestVert.y) > abs(vTestVert.w) ) // Outside our Y Frustum Plane
	    return false;

      if( (vTestVert.z<0.0f) || (vTestVert.z>vTestVert.w)) // Outside our z Frustum Plane
	    return false;

      return true;

}

void RenderCircle(D3DXFROMWINEVECTOR2 aPos, float aCircleRadius=1.1f, D3DXFROMWINEVECTOR4 aColor=D3DXFROMWINEVECTOR4(1,1,1,1), float r=SKY_RADIUS)
{
	double JD;
	/* get Julian day from local time */
	JD = ln_get_julian_from_sys();	
	D3DXFROMWINEVECTOR3 p2;
	#if 1
	p2=m_pSkyEmitter->CalculatePosition(aPos.x, aPos.y, JD);
	#else
	float varphi2=aPos.x*D3DXFROMWINE_PI/180.0f;
	float theta2=aPos.y*D3DXFROMWINE_PI/180.0f;
	p2.x = r * cosf(theta2) * cosf(varphi2)+g_CameraPosition.x;
	p2.z = r * cosf(theta2) * sinf(varphi2)+g_CameraPosition.z;
	p2.y = r * sinf(theta2)+g_CameraPosition.y;// = should be identical to starting cZ value
	#endif
	IRenderer::GetRendererInstance()->drawCircle(p2.x, p2.y, aCircleRadius, D3DXFROMWINEVECTOR4(1,1,1,1));
}

void RenderCircle(float x, float y, float aCircleRadius=1.1f, D3DXFROMWINEVECTOR4 aColor=D3DXFROMWINEVECTOR4(1,1,1,1), float r=SKY_RADIUS)
{
	RenderCircle(D3DXFROMWINEVECTOR2(x,y), aCircleRadius, aColor, r);
}

void cParticleEmitter::RenderStars()
{
    //return; // ???
	//LOG_FNLN;
	double JD;
	/* get Julian day from local time */
	JD = ln_get_julian_from_sys();
	cParticle* particle=m_Particles->m_Next;
	while(particle)
	{
		D3DXFROMWINEVECTOR3 p1=particle->m_vecPos;
		#if 1
		RenderCircle(p1.x, p1.y, 1.1f);
		#else	
		D3DXFROMWINEVECTOR3 p2=CalculatePosition(p1.x, p1.y, JD);
	/*
	//LOG_FNLN;
	//LOG_PRINT("p1.x=%f\n", p1.x);
	//LOG_PRINT("p1.y=%f\n", p1.y);
	//LOG_PRINT("p2.x=%f\n", p2.x);
	//LOG_PRINT("p2.y=%f\n", p2.y);
	*/
		IRenderer::GetRendererInstance()->drawCircle(p2.x, p2.y, 1.1f, D3DXFROMWINEVECTOR4(1,1,1,1));
		#endif

      		// Go to next particle
      		particle = particle->m_Next;
	}
}
void DrawBorders()
{
	return; // ???
	D3DXFROMWINEVECTOR4 color(1.0f, 1.0f, 1.0f, 1.0f);
	const float lineWidth=1.0f;
	unsigned int NN=0;
	for(unsigned int figureFileID=0;figureFileID<NCONSTELLATIONS;figureFileID++)
	{
		int N=constellationBorderNum[figureFileID];
		for(unsigned int i=0;i<N;i++)
		{
			D3DXFROMWINEVECTOR3 pos1=GetPosition3(constellationBorders[NN+i].ra, constellationBorders[NN+i].dec, 0.0f);
			D3DXFROMWINEVECTOR3 pos2=GetPosition3(constellationBorders[NN+i+1].ra, constellationBorders[NN+i+1].dec, 0.0f);
			#if 0
			STX_PRINT("pos1.x=%f\npos1.y=%f\npos1.z=%f\n", pos1.x, pos1.y, pos1.z);
			STX_PRINT("pos2.x=%f\npos2.y=%f\npos2.z=%f\n", pos2.x, pos2.y, pos2.z);
			#endif
			IRenderer::GetRendererInstance()->drawLine(pos1.x, pos1.y, pos2.x, pos2.y, color,  lineWidth);
			pos1=pos2;
			NN+=N;
		}
	}
}

void DrawFigures()
{
	return; // ???
	line_vertex3 line[2];
	line[0].colour=D3DXFROMWINEVECTOR4(1.0f,1.0f,1.0f,1.0f);
	line[1].colour=D3DXFROMWINEVECTOR4(1.0f,1.0f,1.0f,1.0f);

	for(unsigned int figureFileID=0;figureFileID<NCONSTELLATIONS;figureFileID++)
	{
	int cmd0=constellationFigures[constellationFigureStart[figureFileID]+0].cmd;
	int id0=constellationFigures[constellationFigureStart[figureFileID]+0].id;

	if(cmd0==0)
	{
	line[0].pos=GetPosition3(catalog[id0].ra,catalog[id0].dec);
	}
	else
	for(unsigned int i=1;i<constellationFigureNum2[figureFileID];i++)
	{
		int cmd=constellationFigures[constellationFigureStart[figureFileID]+i].cmd;
		int id=constellationFigures[constellationFigureStart[figureFileID]+i].id;
		if(id>=NSTARS)
			continue;

		if(cmd==0)
		{
			line[0].pos=GetPosition3(catalog[id].ra,catalog[id].dec);
		}
		else
		{
			line[1].pos=GetPosition3(catalog[id].ra,catalog[id].dec);
#if 0
	IRenderer::GetRendererInstance()->setShader(shd3);IRenderer::GetRendererInstance()->setVertexFormat(vf3); 	
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
			IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_LINES, 1,line,line, sizeof(line_vertex3));
#else
	D3DXFROMWINEVECTOR4 color(1.0f, 1.0f, 1.0f, 1.0f);
	const float lineWidth=1.0f;
	IRenderer::GetRendererInstance()->drawLine(line[0].pos.x, line[0].pos.y, line[1].pos.x, line[1].pos.y, color, lineWidth);
#endif
			line[0].pos=line[1].pos;
		}
	}
	}
}

void DrawStars()
{
	//cParticleEmitter__DrawStars();
    if(m_pSkyEmitter) m_pSkyEmitter->RenderStars();
}
void DrawPlanets()
{	   
	for(unsigned int i=0;i<10;i++)
	{
		#if 0
		D3DXFROMWINEVECTOR3 pos(m_pSkyEmitter->screenpositions[i].x, m_pSkyEmitter->screenpositions[i].y, 0.0f);
		#else
		D3DXFROMWINEVECTOR3 pos(m_pSkyEmitter->solarsystempositions[i].x, m_pSkyEmitter->solarsystempositions[i].y, 0.0f);
		#endif
		#if 0
		IRenderer::GetRendererInstance()->drawCircle(pos.x,pos.y,5.0f,D3DXFROMWINEVECTOR4(1,1,1,1));
		#else
		RenderCircle(pos.x, pos.y, 5.0f);
		#endif
		DrawText(D3DXFROMWINEVECTOR2(pos.x+3.0f, pos.y), &names[i][0]);
	}
}
void DrawConstellationNames()
{
	 for(unsigned int i=0;i<NCONSTELLATIONS;i++)
	 {
        bool visible=PointInFrustum(constellationpositions[i].x, constellationpositions[i].y, constellationpositions[i].z);
		//if(visible)
		 {
		 char buf[256];
		 stx_snprintf(buf, 256, "%s", &constellationnames[i][0]);
		IRenderer::GetRendererInstance()->drawText(buf, constellationscreenpositions[i].x, constellationscreenpositions[i].y,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());
		}
	}
}

int init(const char* aTitle)
{


#if defined(USE_GLES_SHADERS) && (defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR))
const char* SHD3="[Vertex shader]\n"
"attribute mediump vec3 aPosition;\n"
"attribute mediump vec4 aTexCoord;\n"
"varying mediump vec4 vTexCoord;\n"
"uniform highp mat4 mvp;\n"
"void main(){\n"
"	gl_Position = vec4(aPosition.x, aPosition.y, aPosition.z, 1)*mvp;\n"
"	vTexCoord=aTexCoord;\n"
"}\n"
"[Fragment shader]\n"
"varying mediump vec4 vTexCoord;\n"
"void main()\n"
"{\n"
"gl_FragColor = vTexCoord;\n"
"}\n";
#else
const char* SHD3="[Vertex shader]\n"
"struct VsIn {\n"
"	float3 position: POSITION;\n"
"	float4 texCoord: TEXCOORD0;\n"
"};\n"
"struct VsOut {\n"
"	float4 position: POSITION;\n"
"	float4 texCoord: TEXCOORD0;\n"
"};\n"
"ROW_MAJOR float4x4 mvp MVPSEMANTIC;\n"
"VsOut main(VsIn In){\n"
"	VsOut Out=(VsOut)0;\n"
"	Out.position = mul(float4(In.position.x, In.position.y, In.position.z, 1.0), mvp);\n"
"	//Out.position.z = 0.0;\n"
"	//Out.position.w = 1.0;\n"
"	Out.texCoord = In.texCoord;\n"
"	return Out;\n"
"}\n"
"[Fragment shader]\n"
"struct VsOut {\n"
"	float4 position: POSITION;\n"
"	float4 texCoord: TEXCOORD0;\n"
"};\n"
"float4 main(VsOut IN): COLOR {\n"
"	return IN.texCoord;\n"
"}\n";
#endif

	shd3 = IRenderer::GetRendererInstance()->addHLSLShader(SHD3, "main", "main");

	FormatDesc format3[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 4
	};
	vf3 = IRenderer::GetRendererInstance()->addVertexFormat(format3, elementsOf(format3), shd3);

#if defined(USE_GLES_SHADERS) && (defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR))
const char* SHD4="[Vertex shader]\n"
"attribute mediump vec4 aPosition;\n"
"attribute mediump vec4 aTexCoord;\n"
"varying mediump vec4 vTexCoord;\n"
"uniform highp mat4 mvp;\n"
"void main(){\n"
"	gl_Position = mvp*aPosition;\n"
"	vTexCoord=aTexCoord;\n"
"}\n"
"[Fragment shader]\n"
"varying mediump vec4 vTexCoord;\n"
"void main()\n"
"{\n"
"gl_FragColor = vTexCoord;\n"
"}\n";
#else
const char* SHD4="[Vertex shader]\n"
"struct VsIn {\n"
"	float4 position: POSITION;\n"
"	float4 texCoord: TEXCOORD0;\n"
"};\n"
"struct VsOut {\n"
"	float4 position: POSITION;\n"
"	float4 texCoord: TEXCOORD0;\n"
"};\n"
"ROW_MAJOR float4x4 mvp MVPSEMANTIC;\n"
"VsOut main(VsIn In){\n"
"	VsOut Out=(VsOut)0;\n"
"	Out.position = mul(mvp, In.position);\n"
"	Out.texCoord = In.texCoord;\n"
"	return Out;\n"
"}\n"
"[Fragment shader]\n"
"struct VsOut {\n"
"	float4 position: POSITION;\n"
"	float4 texCoord: TEXCOORD0;\n"
"};\n"
"float4 main(VsOut IN): COLOR {\n"
"	return IN.texCoord;\n"
"}\n";
#endif
	shd4 = IRenderer::GetRendererInstance()->addHLSLShader(SHD4, "main", "main");

	FormatDesc format4[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 4
	};
	vf4 = IRenderer::GetRendererInstance()->addVertexFormat(format4, elementsOf(format4), shd4);
	figureFileID=0;//???
	//LOG_PRINT("w/h=%f\n",width/float(height));
	D3DXFROMWINEMatrixPerspectiveFovLH(&g_CameraProjMatrix, D3DXFROMWINE_PI / 2.0f, float(IRenderer::GetRendererInstance()->GetViewportWidth())/float(IRenderer::GetRendererInstance()->GetViewportHeight()), 0.001f, 1200000.0f);

	D3DXFROMWINEVECTOR3 pvEyePt=D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f);
	D3DXFROMWINEVECTOR3 pvLookatPt=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXFROMWINEVECTOR3 up(0.0f,1.0f,0.0f);

	D3DXFROMWINEMatrixLookAtLH(&g_CameraViewMatrix, &pvEyePt, &pvLookatPt, &up);
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	#if 0
	D3DXFROMWINEMatrixIdentity(&g_CameraViewMatrix);
	D3DXFROMWINEMatrixIdentity(&g_CameraProjMatrix);
	#endif
      IRenderer::GetRendererInstance()->SetTransform(TS_WORLD, I);
      IRenderer::GetRendererInstance()->SetTransform(TS_VIEW,		g_CameraViewMatrix);
      IRenderer::GetRendererInstance()->SetTransform(TS_PROJECTION, 	g_CameraProjMatrix);      

	g_CameraSpeed=0.001f;
    m_pSkyEmitter=new cParticleEmitter(0);
    LoadBorders();
    LoadFigures();
	return 0;
}

void render()
{
	float f=0.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
    DrawBorders();
    DrawFigures();
    DrawStars();
    DrawPlanets();
    DrawConstellationNames();
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("Stars");
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

