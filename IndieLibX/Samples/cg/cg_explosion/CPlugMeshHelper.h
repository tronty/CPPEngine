#ifndef __PLUGMESHHELPER__
#define __PLUGMESHHELPER__
#include "COglMeshHelper.h"

#include "nv_plug/plug.h"

using namespace plug;

class PlugMeshHelper : public OglMeshHelper, public PlugContainer
{
protected:
	IPlug *m_plugMesh;			///< original mesh plug
		IPlug	*m_plugVtx;			///< original mesh plug
		IPlug	*m_plugColor;			///< original mesh plug
		IPlug	*m_plugNormal;			///< original mesh plug
		IPlug	*m_plugUV[4];			
public:
	PlugMeshHelper();
	~PlugMeshHelper();
	/// \name From PlugContainer
	//@{
	virtual		bool plug_IncomingData(IPlug *lpSrc, IPlug *lpDst, bool bSendEvent);
	virtual		void *plug_DataReadEvent(IPlug *lpPlug, int &idx, int *arraysize);
	//@}
};

#define PLUG_GLPLUGMESH	1
#define PLUG_GLPLUGMESHVTX	2
#define PLUG_GLPLUGMESHCOLOR	3
#define PLUG_GLPLUGMESHNORMAL	4
#define PLUG_GLPLUGMESHUV0	5
#define PLUG_GLPLUGMESHUV1	6
#define PLUG_GLPLUGMESHUV2	7
#define PLUG_GLPLUGMESHUV3	8

#endif
