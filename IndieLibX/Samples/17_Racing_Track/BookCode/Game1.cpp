/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

        ShaderID basicEffect-1;

        TextureID road=-1;
        std::vector<stx_VertexPositionNormalTexture> trackVertices;
        VertexFormatID myVertexDeclaration=-1;

void Initialize()
        {

            std::list<D3DXFROMWINEVECTOR3> basePoints();
            basePoints.Add(D3DXFROMWINEVECTOR3(2, 0, 4));
            basePoints.Add(D3DXFROMWINEVECTOR3(0, 0, 0));
            basePoints.Add(D3DXFROMWINEVECTOR3(-2, 0, -4));
            basePoints.Add(D3DXFROMWINEVECTOR3(2, 0, -4));
            basePoints.Add(D3DXFROMWINEVECTOR3(0, 1, 0));
            basePoints.Add(D3DXFROMWINEVECTOR3(-2, 0, 4));

            std::list<D3DXFROMWINEVECTOR3> extendedTrackPoints = GenerateTrackPoints(basePoints);
            trackVertices = GenerateTrackVertices(extendedTrackPoints);
        }

        D3DXFROMWINEVECTOR3 CR3D(D3DXFROMWINEVECTOR3 v1, D3DXFROMWINEVECTOR3 v2, D3DXFROMWINEVECTOR3 v3, D3DXFROMWINEVECTOR3 v4, float amount)
        {
            D3DXFROMWINEVECTOR3 result();

            result.X = MathHelper.CatmullRom(v1.X, v2.X, v3.X, v4.X, amount);
            result.Y = MathHelper.CatmullRom(v1.Y, v2.Y, v3.Y, v4.Y, amount);
            result.Z = MathHelper.CatmullRom(v1.Z, v2.Z, v3.Z, v4.Z, amount);

            return result;
        }

        std::list<D3DXFROMWINEVECTOR3> InterpolateCR(D3DXFROMWINEVECTOR3 v1, D3DXFROMWINEVECTOR3 v2, D3DXFROMWINEVECTOR3 v3, D3DXFROMWINEVECTOR3 v4)
        {
            std::list<D3DXFROMWINEVECTOR3> list();
            int detail = 20;
            for (int i = 0; i < detail; i++)
            {
                D3DXFROMWINEVECTOR3 newPoint = CR3D(v1, v2, v3, v4, (float)i / (float)detail);
                list.Add(newPoint);
            }
            return list;
        }

        std::list<D3DXFROMWINEVECTOR3> GenerateTrackPoints(std::list<D3DXFROMWINEVECTOR3> basePoints)
        {
            basePoints.Add(basePoints[0]);
            basePoints.Add(basePoints[1]);
            basePoints.Add(basePoints[2]);

            std::list<D3DXFROMWINEVECTOR3> allPoints();

            for (int i = 1; i < basePoints.Count - 2; i++)
            {
                std::list<D3DXFROMWINEVECTOR3> part = InterpolateCR(basePoints[i - 1], basePoints[i], basePoints[i + 1], basePoints[i + 2]);
                allPoints.AddRange(part);
            }

            return allPoints;
        }

        std::vector<stx_VertexPositionNormalTexture> GenerateTrackVertices(std::list<D3DXFROMWINEVECTOR3> basePoints)
        {
            float halfTrackWidth = 0.2f;
            float textureLenght = 0.5f;

            float distance = 0;
            std::list<stx_VertexPositionNormalTexture> verticesList();

            for (int i = 1; i < basePoints.Count - 1; i++)
            {
                D3DXFROMWINEVECTOR3 carDir = basePoints[i + 1] - basePoints[i];
                D3DXFROMWINEVECTOR3 sideDir = D3DXFROMWINEVECTOR3.Cross(D3DXFROMWINEVECTOR3(0, 1, 0), carDir);
                sideDir.Normalize();

                D3DXFROMWINEVECTOR3 outerPoint = basePoints[i] + sideDir * halfTrackWidth;
                D3DXFROMWINEVECTOR3 innerPoint = basePoints[i] - sideDir * halfTrackWidth;

                stx_VertexPositionNormalTexture vertex;
                vertex = stx_VertexPositionNormalTexture(innerPoint, D3DXFROMWINEVECTOR3(0, 1, 0), D3DXFROMWINEVECTOR2(0, distance / textureLenght));
                verticesList.Add(vertex);
                vertex = stx_VertexPositionNormalTexture(outerPoint, D3DXFROMWINEVECTOR3(0, 1, 0), D3DXFROMWINEVECTOR2(1, distance / textureLenght));
                verticesList.Add(vertex);
                distance += carDir.size()();
            }

            stx_VertexPositionNormalTexture extraVert = verticesList[0];
            extraVert.TextureCoordinate.Y = distance / textureLenght;
            verticesList.Add(extraVert);

            extraVert = verticesList[1];
            extraVert.TextureCoordinate.Y = distance / textureLenght;
            verticesList.Add(extraVert);

            return verticesList.ToArray();
        }        

        void LoadContent()
        {
        basicEffect = IRenderer::GetRendererInstance()->addShaderFromFile("/17_Racing_Track/Effect.fx", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	myVertexDeclaration = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), basicEffect);

	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	road=IRenderer::GetRendererInstance()->addImageLibTexture("/17_Racing_Track/road.png", false, ss);
            //myVertexDeclaration = new VertexFormatID(device, stx_VertexPositionNormalTexture.VertexElements);

	}

int init(const char* aTitle)
{
	LoadContent();
	Initialize();
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	IRenderer::GetRendererInstance()->setShader(basicEffect);
	IRenderer::GetRendererInstance()->setVertexFormat(myVertexDeclaration);

	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, trackVertices.size()-2, &trackVertices[0], &trackVertices[0], sizeof(stx_VertexPositionNormalTexture));

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("17_Racing_Track BookCode");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}
