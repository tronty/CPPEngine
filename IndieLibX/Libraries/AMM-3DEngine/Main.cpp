/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include "MeshGenerator.h"
#include "Model.h"

D3DXFROMWINEVECTOR3 cameraPosition=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 15.0f);
std::vector<Model*> m_objects;
D3DXFROMWINEMATRIX toWorld;
D3DXFROMWINEMATRIX view;
D3DXFROMWINEMATRIX persp;

int init(unsigned int width, unsigned int height)
{
	D3DXFROMWINEMatrixIdentity(&toWorld);
	D3DXFROMWINEVECTOR3 peye(0.0f, 0.0f, 0.0f);
	D3DXFROMWINEVECTOR3 pat(0.0f, 0.0f, 5.0f);//(0.0f, 5.0f, 200.0f);
	D3DXFROMWINEVECTOR3 pup(0.0f, 1.0f, 0.0f);
	D3DXFROMWINEMatrixLookAtLH(&view, &peye, &pat, &pup);
	D3DXFROMWINEMatrixPerspectiveFovLH(&persp, 45.0f, float(width) / height, 0.001f, 1000.0f);

	//GameManager::GetInstance()->m_collisionType = BRUTE_FORCE;

	MeshGenerator* mesh = new MeshGenerator();

	LOG_PRINT("Generating Tree1...\n");
	mesh->rotationOffset = 0.3926991f;
	mesh->translationOffset = 5.0f;
	mesh->m_system.AddAxiom('F', "F[Fz[zFZXFZYF]Z[ZFxzFyzF]C+]");
	mesh->GenerateModel("+TT+F", 4, "Tree1", D3DXFROMWINEVECTOR3(0, 0, 0), 1.0f, 6);

	LOG_PRINT("Generating Tree2...\n");
	mesh->m_system.ClearAxioms();
	mesh->rotationOffset = 0.4485496f;
	mesh->translationOffset = 1.0f;
	mesh->m_system.AddAxiom('R', "F[ZxR][zR]FR");
	mesh->m_system.AddAxiom('F', "FF");
	//mesh->GenerateModel("R", 7, "Tree2", D3DXFROMWINEVECTOR3(0, 0, 0), .1f, 5);

	LOG_PRINT("Generating Tree3...\n");
	mesh->m_system.ClearAxioms();
	mesh->rotationOffset = 0.3f;
	mesh->translationOffset = 4.0f;
	mesh->m_system.AddAxiom('R', "FFF[FXYZ[FxRxF[zFRzXFC]R[ZFZyFC]]yFRyF]");
	mesh->GenerateModel("+TT+R", 5, "Tree3", D3DXFROMWINEVECTOR3(0, 0, 0), 1.0f, 5);

	LOG_PRINT("Generating Tree4...\n");
	mesh->m_system.ClearAxioms();
	mesh->rotationOffset = 0.1f;
	mesh->translationOffset = 1.0f;
	mesh->m_system.AddAxiom('R', "F[[yyBBzB]XB]");
	mesh->m_system.AddAxiom('B', "XXYYYYYYYYFRFzzFRRC");
	mesh->GenerateModel("+TT+R", 7, "Tree4", D3DXFROMWINEVECTOR3(0, 0, 0), 1.0f, 6);

	LOG_PRINT("Generating Tree5...\n");
	mesh->m_system.ClearAxioms();
	mesh->rotationOffset = .4f;
	mesh->translationOffset = .2f;
	mesh->m_system.AddAxiom('R', "YYTF[xFR]C[XFRFR]");
	mesh->GenerateModel("+TT+R", 7, "Tree5", D3DXFROMWINEVECTOR3(0, 0, 0), .2f, 3);

	LOG_PRINT("All trees generated!\n");
	LOG_PRINT("Building a forest...\n");
	for (int x = -5; x < 5; x++)
	{
		for (int z = -5; z < 5; z++)
		{
			Model* temp=0;
			int treeType = stx_rand() % 4;
			//int treeType = 4;
			switch (treeType)
			{
			case 0:
				temp=(new Model("Tree1"));
				temp->m_scales = D3DXFROMWINEVECTOR3(.1f, .1f, .1f);
				break;
			case 1:
				temp=(new Model("Tree3"));
				temp->m_scales = D3DXFROMWINEVECTOR3(.3f, .3f, .3f);
				break;
			case 2:
				temp=(new Model("Tree4"));
				temp->m_scales = D3DXFROMWINEVECTOR3(.5f, .5f, .5f);
				break;
			case 3:
				temp=(new Model("Tree5"));
				temp->m_scales = D3DXFROMWINEVECTOR3(.5f, .5f, .5f);
			default:
				break;
			}
			temp->m_translations = D3DXFROMWINEVECTOR3(x * (stx_rand() % 30 + 5), 0, z * (stx_rand() % 25 + 20));
			D3DXFROMWINEVECTOR3 color(stx_rand() % 80 / 255.0f,
										stx_rand() % 100 / 255.0f + .3f,
										stx_rand() % 10 / 255.0f);
			D3DXFROMWINEVECTOR3 pv(0, 1, 0);
			D3DXFROMWINEMatrixRotationAxis(&temp->m_rotations, &pv, (float)(stx_rand() % 180));

			temp->defaultColor = color;
			temp->m_scales *= (float)(stx_rand() % 10 + 10) / 10.0f;
			//temp->m_collisionData->m_collisionMask = 0;
			m_objects.push_back(temp);
	#if 0
		LOG_PRINT("n: %x\n", m_objects.size()-1);
		LOG_PRINT("m_shd: %x\n", m_objects[m_objects.size()-1]->m_shd);
		LOG_PRINT("m_vf: %x\n", m_objects[m_objects.size()-1]->m_vf);
		LOG_PRINT("m_vertices(): %x\n", m_objects[m_objects.size()-1]->m_vertices());
		LOG_PRINT("m_indices.size(): %x\n", m_objects[m_objects.size()-1]->m_indices.size());
		LOG_PRINT("&m_indices[0]: %x\n", &m_objects[m_objects.size()-1]->m_indices[0]);
		LOG_PRINT("&m_lVertex[0]: %x\n", &m_objects[m_objects.size()-1]->m_lVertex[0]);
	#endif
		}
	}
	LOG_PRINT("m_objects.size(): %x\n", m_objects.size());
	return 0;
}

int render()
{
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (0.529f, 0.807f, 0.9803f, 0.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	for(unsigned int i=0;i<m_objects.size();i++)
		m_objects[i]->Render(toWorld,view,persp);

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
	return 0;
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance(640, 512, "3D L-System");	
	IInput*    i=STX_Service::GetInputInstance();
	init(640, 512);
	while (!i->OnKeyPress (KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}
