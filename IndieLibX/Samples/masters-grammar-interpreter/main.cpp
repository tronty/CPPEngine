#if 1
/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <sstream>
#include <string>

#include <Framework3/IRenderer.h>
#include <GUI/GUIUtils.h>
#include <FW3.h>

#define defaultshader ePhongLighting2
//define alternativeshader eSimpleShader
//define alternativeshader eAmbientLighting
#define alternativeshader ePhongLighting2
//define alternativeshader ePhongLighting1
//define alternativeshader ePhongLighting2

const char* filenames[] = {	"/masters-grammar-interpreter/testing.txt",
				"/masters-grammar-interpreter/borg_cube.txt",
				"/masters-grammar-interpreter/c_station.txt",
				"/masters-grammar-interpreter/enterprise.txt",
				"/masters-grammar-interpreter/klingon.txt",
				"/masters-grammar-interpreter/rotational_ship.txt",
				"/masters-grammar-interpreter/space_station.txt",
				"/masters-grammar-interpreter/spike_ship.txt",
				"/masters-grammar-interpreter/squid_ship.txt",
				"/masters-grammar-interpreter/tie_fighter.txt",
				"/masters-grammar-interpreter/wing_ship.txt"
				};
unsigned int m_i=9;

IRenderer* r=0;
IInput*    i=0;
float mAngleX=180.0f;
float mAngleY=0.0f;
D3DXFROMWINEMATRIX R_,S_,T_;

struct line;
std::vector<std::vector<line>> v;
struct Shape3D_
{
	MeshRenderer2 shape3D;
	D3DXFROMWINEVECTOR3 min;
	D3DXFROMWINEVECTOR3 max;
	Shape3D_()
	{
		min=D3DXFROMWINEVECTOR3( 1e10f, 1e10f, 1e10f);
		max=D3DXFROMWINEVECTOR3( -1e10f, -1e10f, -1e10f);
	}
	virtual ~Shape3D_(){}

	void draw(D3DXFROMWINEMATRIX& mat, D3DXFROMWINEVECTOR4 AmbientColor, D3DXFROMWINEVECTOR4 DiffuseColor,
			D3DXFROMWINEVECTOR4 LightDir=D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 EyeDir=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 1.0f, 1.0f))
	{
		shape3D.SetAmbientColor(AmbientColor);
		shape3D.SetDiffuseColor(DiffuseColor);
		shape3D.SetLightDir(LightDir);
		shape3D.SetEyeDir(EyeDir);
		shape3D.BeginDraw(&mat, -1, -1, -1);
		shape3D.EndDraw();
	}
};
Shape3D_ shape3D[4];
struct line
{
	line()
	{
		name="";
		shape=eShapeNone;
/*
		eCylinder,
		eEllipsoid
		eBox,
		eTexturedBox,
*/
		active=false;
		additive=false;
		position=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
	 	extents=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
	 	angle=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
		priority=0;
		tags.clear();
	}
	line(const line& rhs)
	{
		name=rhs.name;
		shape=rhs.shape;
		active=rhs.active;
		additive=rhs.additive;
		position=rhs.position;
	 	extents=rhs.extents;
		angle=rhs.angle;
		priority=rhs.priority;
	}
	virtual ~line()
	{
		tags.clear();
	}
	void render()
	{
		D3DXFROMWINEVECTOR4 AmbientColor=D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		D3DXFROMWINEVECTOR4 DiffuseColor=D3DXFROMWINEVECTOR4(0.0f, 0.1f, 0.0f, 1.0f);
		unsigned int i=0;
		switch(shape)
		{
			case eCylinder:
				i=0;
				AmbientColor=D3DXFROMWINEVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
				DiffuseColor=D3DXFROMWINEVECTOR4(0.1f, 0.0f, 0.0f, 1.0f);
				break;
			case eEllipsoid:
				i=1;
				AmbientColor=D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
				DiffuseColor=D3DXFROMWINEVECTOR4(0.0f, 0.1f, 0.0f, 1.0f);
				break;
			case eBox:
				i=2;
				AmbientColor=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
				DiffuseColor=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.1f, 1.0f);
				break;
			case eTexturedBox:
				i=3;
				AmbientColor=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
				DiffuseColor=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.1f, 1.0f);
				break;
		};
		D3DXFROMWINEVECTOR4 LightDir(0.0f, 0.0f, -1.0f, 1.0f);
		D3DXFROMWINEVECTOR4 EyeDir(0.0f, 0.0f, 1.0f, 1.0f);

		D3DXFROMWINEMATRIX mvp_, R, S, T, I;
		D3DXFROMWINEMatrixTranslation(&T, position.x, position.y, position.z);
		D3DXFROMWINEMatrixScaling(&S, extents.x, extents.y, extents.z);
		D3DXFROMWINEMatrixRotationYawPitchRoll(&R, angle.x, angle.y, angle.z);
		D3DXFROMWINEMATRIX scale=ScaleAsset(elementsOf(shape3D), shape3D, v[m_i]);
	
		D3DXFROMWINEMatrixIdentity(&I);	
		//mvp_=R*S*scale*T*R_*S_*T_;
        MeshRenderer2 m=shape3D[i].shape3D;
        D3DXFROMWINEMATRIX mm;
	mm=R*S*T;
        m.TransformCoord(mm);
        mm=R_*S_*T_;
        m.TransformCoord(mm);
        m.SetAmbientColor(AmbientColor);
        m.SetDiffuseColor(DiffuseColor);
        m.Draw(&I, -1, -1, -1, AmbientColor, DiffuseColor, LightDir, EyeDir);
	}
D3DXFROMWINEMATRIX ScaleAsset(const unsigned int aN, Shape3D_* aShape3D, const std::vector<line>& av)
{
	D3DXFROMWINEVECTOR3 _min( 1e10f, 1e10f, 1e10f);
	D3DXFROMWINEVECTOR3 _max( -1e10f, -1e10f, -1e10f);
	D3DXFROMWINEVECTOR3 __min, __max;
	for(int i = 0; i<av.size(); i++)
	{
		switch(shape)
		{
			case eCylinder:
				__min=aShape3D[0].min;
				__max=aShape3D[0].max;
			break;
			case eEllipsoid:
				__min=aShape3D[1].min;
				__max=aShape3D[1].max;
			break;
			case eBox:
				__min=aShape3D[2].min;
				__max=aShape3D[2].max;
			break;
			case eTexturedBox:
				__min=aShape3D[3].min;
				__max=aShape3D[3].max;
			break;
		};
	
#if 1
		__min+=av[i].position;
		__max+=av[i].position;
#endif
		_min.x = std::min( _min.x, __min.x);
		_min.y = std::min( _min.y, __min.y);
		_min.z = std::min( _min.z, __min.z);
		_max.x = std::max( _max.x, __max.x);
		_max.y = std::max( _max.y, __max.y);
		_max.z = std::max( _max.z, __max.z);
	}
	D3DXFROMWINEVECTOR3 vDelta = _max-_min;
	D3DXFROMWINEVECTOR3 vHalf =  _min + (vDelta / 2.0f);
	float fScale = 10.0f / D3DXFROMWINEVec3Length(&vDelta);
	return D3DXFROMWINEMATRIX(
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		-vHalf.x,-vHalf.y,-vHalf.z,1.0f) *
		D3DXFROMWINEMATRIX(
		fScale,0.0f,0.0f,0.0f,
		0.0f,fScale,0.0f,0.0f,
		0.0f,0.0f,fScale,0.0f,
		0.0f,0.0f,0.0f,1.0f);
}
	std::string name;
	tShape shape;
	bool active;
	bool additive;
	D3DXFROMWINEVECTOR3 position;
	D3DXFROMWINEVECTOR3 extents;
	D3DXFROMWINEVECTOR3 angle;
	unsigned int priority;
	std::vector<std::string> tags;
};
	
std::string str;
ifstream myfile;
bool readline(unsigned int k)
{
	line ln;
    	myfile >> ln.name;
    	myfile >> ln.name;
		ln.shape=eShapeNone;
	if(stx_strcmp(ln.name.c_str(),"cylinder")==0)
	{
		ln.shape=eCylinder;
	}
	else if(stx_strcmp(ln.name.c_str(),"ellipsoid")==0)
	{
		ln.shape=eEllipsoid;
	}
	else if(stx_strcmp(ln.name.c_str(),"rectangle")==0)
	{
		ln.shape=eBox;//???eTexturedBox
	}
    	myfile >> str;
   	myfile >> str;
	if(stx_strcmp(str.c_str(),"True")==0)
		ln.active=true;
    	myfile >> str;
   	myfile >> str;
	if(stx_strcmp(str.c_str(),"True")==0)
		ln.additive=true;
    	myfile >> str;
	myfile >> ln.position.x;
	myfile >> ln.position.y;
	myfile >> ln.position.z;
	//D3DXFROMWINEMatrixTranslation(&ln.translation,ln.position.x,ln.position.y,ln.position.z);
    	myfile >> str;
	myfile >> ln.extents.x;
	myfile >> ln.extents.y;
	myfile >> ln.extents.z;
	//D3DXFROMWINEMatrixScaling(&ln.scaling,ln.extents.x,ln.extents.y,ln.extents.z);
	D3DXFROMWINEMATRIX orientation;
	D3DXFROMWINEMatrixIdentity(&orientation);
    	myfile >> str;
	myfile >> orientation.m[0][0];
	myfile >> orientation.m[0][1];
	myfile >> orientation.m[0][2];
	myfile >> orientation.m[1][0];
	myfile >> orientation.m[1][1];
	myfile >> orientation.m[1][2];
	myfile >> orientation.m[2][0];
	myfile >> orientation.m[2][1];
	myfile >> orientation.m[2][2];
	
	D3DXFROMWINEVECTOR3 rrr;
	D3DXFROMWINEGetRotation(orientation, rrr);
	ln.angle=rrr;
	//LOG_PRINT("ln.position.x:%f\n", ln.position.x);
	//LOG_PRINT("ln.position.y:%f\n", ln.position.y);
	//LOG_PRINT("ln.position.z:%f\n", ln.position.z);
	//LOG_PRINT("ln.extents.x:%f\n", ln.extents.x);
	//LOG_PRINT("ln.extents.y:%f\n", ln.extents.y);
	//LOG_PRINT("ln.extents.z:%f\n", ln.extents.z);
	//LOG_PRINT("ln.orientation.x:%f\n", rrr.x);
	//LOG_PRINT("ln.orientation.y:%f\n", rrr.y);
	//LOG_PRINT("ln.orientation.z:%f\n", rrr.z);
#if 0
	D3DXFROMWINEMATRIX mvp;
	mvp=stx_GetPivotMatrix(rrr, ln.position);
	ln.mvp*=mvp;
#elif 0
	ln.mvp=ln.orientation*ln.scaling*ln.translation;
#endif
#if 0
	D3DXFROMWINEGetTranslation(ln.mvp, rrr);
	LOG_PRINT("x:%f\n", rrr.x);
	LOG_PRINT("y:%f\n", rrr.y);
	LOG_PRINT("z:%f\n", rrr.z);
	D3DXFROMWINEGetScaling(ln.mvp, rrr);
	LOG_PRINT("sx:%f\n", rrr.x);
	LOG_PRINT("sy:%f\n", rrr.y);
	LOG_PRINT("sz:%f\n", rrr.z);
	D3DXFROMWINEGetRotation(ln.mvp, rrr);
	LOG_PRINT("Yaw:%f\n", rrr.x);
	LOG_PRINT("Pitch:%f\n", rrr.y);
	LOG_PRINT("Roll:%f\n", rrr.z);
#endif
	/*
	position.toString();
	extents.toString();
	orientation.toString();
	*/
	myfile >> str;
	myfile >> ln.priority;

	// eg. tags_begin  thruster wing thruster_bumps tags_end
	myfile >> str;// tags_begin 
	while(stx_strcmp(str.c_str(),"tags_end")!=0)
	{
		myfile >> str;
		ln.tags.push_back(str);
	}

	myfile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	LOG_FNLN;
	LOG_PRINT("v.size()=%d\n", v.size());
	LOG_PRINT("v[%d].size()=%d\n", k, v.size());
	v[k].push_back(ln);
	return myfile.eof();
}

class masters_grammar_interpreter : public STXGUI {
public:
	masters_grammar_interpreter(){}
	virtual ~masters_grammar_interpreter(){}

	virtual void actionPerformed(GUIEvent &evt)
{
  const std::string &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == RADIO_BUTTON)
  {
	GUIRadioButton   *radioButton = (GUIRadioButton*)sourceRectangle;
	if(radioButton->isPressed() && radioButton->isChecked())
	{
        mAngleX=180.0f;
        mAngleY=0.0f;
		//LOG_PRINT("callbackString=%s\n", callbackString.data);
      	if(callbackString == "testing") 
		{
			m_i=0;
		}
		else if(callbackString == "borg_cube") 
		{
			m_i=1;
		}
		else if(callbackString == "c_station") 
		{
			m_i=2;
		}
		else if(callbackString == "enterprise") 
		{
			m_i=3;
		}
		else if(callbackString == "klingon") 
		{
			m_i=4;
		}
		else if(callbackString == "rotational_ship") 
		{
			m_i=5;
		}
		else if(callbackString == "space_station") 
		{
			m_i=6;
		}
		else if(callbackString == "spike_ship") 
		{
			m_i=7;
		}
		else if(callbackString == "squid_ship") 
		{
			m_i=8;
		}
		else if(callbackString == "tie_fighter") 
		{
			m_i=9;
		}
		else if(callbackString == "wing_ship") 
		{
			m_i=10;
		}
	}
	}
}

int init(const char* aTitle)
{

	//LOG_PRINT("masters-grammar-interpreter:\n");
	//"cylinder", "ellipsoid", "rectangle"
	shape3D[0].shape3D.CreateCylinder(1.0f, 1.0f, 1.0f, defaultshader);
	shape3D[0].shape3D.GetBounds(shape3D[0].min, shape3D[0].max);
        shape3D[1].shape3D.CreateEllipsoid(1.0f, 1.0f, 1.0f, alternativeshader);
	shape3D[1].shape3D.GetBounds(shape3D[1].min, shape3D[1].max);
        shape3D[2].shape3D.CreateBox(1.0f, 1.0f, 1.0f, alternativeshader);
	shape3D[2].shape3D.GetBounds(shape3D[2].min, shape3D[2].max);
        shape3D[3].shape3D.CreateTexturedBox(1.0f, 1.0f, 1.0f, defaultshader);
	shape3D[3].shape3D.GetBounds(shape3D[3].min, shape3D[3].max);

	// name ellipsoid active True additive True position  0.0 0.0 0.0 extents  10.0 15.0 10.0 orientation  1.0 0.0 0.0 0.0 1.0 0.0 0.0 0.0 1.0 priority  0 tags_begin  tags_end # 

	for(unsigned int k=0;k<elementsOf(filenames);k++)
	{
		v.resize(v.size()+1);
		std::string filename=filenames[k];
		std::string file=stx_convertpath(filename.c_str());

		std::ifstream f(file.c_str());
		std::string line;
		int n = 0;
		for (n = 0; std::getline(f, line); ++n);
		f.close();

    		myfile.open(file.c_str());
		for (int i = 0; i<n; i++)
			readline(k);
    		myfile.close();
		LOG_PRINT("v.size()=%d\n", v.size());
		LOG_PRINT("v[%d].size()=%d\n", k, v[k].size());
		for (int i = 0; i<v[k].size(); i++)
			LOG_PRINT("%s\n", v[k][i].name.c_str());
	}

	D3DXFROMWINEMatrixIdentity(&R_);
#if 0
	D3DXFROMWINEMatrixScaling(&S_, 0.025f, 0.025f, 0.025f);
	D3DXFROMWINEMatrixTranslation(&T_, 0.0f, 0.0f, 2.0f);
#elif 1
	D3DXFROMWINEMatrixScaling(&S_, 0.05f, 0.05f, 0.05f);
	D3DXFROMWINEMatrixTranslation(&T_, 0.0f, 0.0f, -2.0f);
#else
	D3DXFROMWINEMatrixScaling(&S_, 0.03f, 0.03f, 0.03f);
	D3DXFROMWINEMatrixTranslation(&T_, 0.0f, 0.0f, -2.0f);
#endif		
	STXGUI::init("/masters-grammar-interpreter/GUILayout.xml");

	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
		
		stx_GetAngles(mAngleX, mAngleY);
		D3DXFROMWINEMatrixRotationYawPitchRoll(&R_, mAngleX, mAngleY, 0.0f);

	for(unsigned int i=0;i<v[m_i].size();i++)
		v[m_i][i].render();
		
  		STXGUI::update();

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

		char txt2[128];
		stx_snprintf(txt2, 128, "m_i=%d\n", m_i);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 50, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		stx_snprintf(txt2, 128, "AngleX=%f\n", mAngleX);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 70, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		stx_snprintf(txt2, 128, "AngleY=%f\n", mAngleY);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 90, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}
};

int ApplicationLogic()
{
	r=IRenderer::GetRendererInstance("masters-grammar-interpreter");	
	i=STX_Service::GetInputInstance();
	masters_grammar_interpreter app;
	app.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		app.render();
		//break;
	}
	return 0;
}
#endif

