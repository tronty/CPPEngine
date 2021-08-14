/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("SimpleTexture");		
	IInput*    i=STX_Service::GetInputInstance();
	MeshRenderer2 shape3D;
        shape3D.CreateSphere(1.0f, eSimpleShader);
	MeshRenderer2 m_Mesh;
#if 0
	FW3::GetInstance()->GetMesh(m_Mesh. "/skeleton/skeleton.x", eSimpleShader);
#else
	m_Mesh.Load("/skeleton/skeleton.x", eSimpleShader);
#endif
	InitNSGUI("/skeleton/GUILayout.xml");
	return 0;
}
