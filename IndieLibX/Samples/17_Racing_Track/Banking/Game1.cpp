/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

        ShaderID basicEffect-1;

        TextureID road=-1;
        std::vector<stx_VertexPositionNormalTexture> trackVertices;
        VertexFormatID trackVertexDeclaration=-1;
        std::vector<VertexPositionColor> supportVertices;
        VertexFormatID supportVertexDeclaration=-1;

void Initialize()
        {

            std::list<D3DXFROMWINEVECTOR3> trackPoints();
            trackPoints.Add(D3DXFROMWINEVECTOR3(0, 0, -2));

            trackPoints.Add(D3DXFROMWINEVECTOR3(7, 0, 1));
            trackPoints.Add(D3DXFROMWINEVECTOR3(7, 3, 1));
            trackPoints.Add(D3DXFROMWINEVECTOR3(4, 3, 0));
            trackPoints.Add(D3DXFROMWINEVECTOR3(4, 0, 0));

            trackPoints.Add(D3DXFROMWINEVECTOR3(10, 0, 0));
            trackPoints.Add(D3DXFROMWINEVECTOR3(8, 0, -4));
            trackPoints.Add(D3DXFROMWINEVECTOR3(12, 0, -7));
            trackPoints.Add(D3DXFROMWINEVECTOR3(8, 1, -10));
            trackPoints.Add(D3DXFROMWINEVECTOR3(12, 0, -14));
            trackPoints.Add(D3DXFROMWINEVECTOR3(6, 0, -17));

            trackPoints.Add(D3DXFROMWINEVECTOR3(2, 1, -13));
            trackPoints.Add(D3DXFROMWINEVECTOR3(9, 2, -6));
            trackPoints.Add(D3DXFROMWINEVECTOR3(4, 1, 2));
            trackPoints.Add(D3DXFROMWINEVECTOR3(1, 1, 2));

            trackPoints.Add(D3DXFROMWINEVECTOR3(3, 0, -3));
            trackPoints.Add(D3DXFROMWINEVECTOR3(9, 0, -10));
            trackPoints.Add(D3DXFROMWINEVECTOR3(6, 0, -11));

            std::list<D3DXFROMWINEVECTOR3> extendedTrackPoints = GenerateTrackPoints(trackPoints);
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

        std::vector<stx_VertexPositionNormalTexture> GenerateTrackVertices(std::list<D3DXFROMWINEVECTOR3> trackPoints)
        {
            float halfTrackWidth = 0.2f;
            std::list<stx_VertexPositionNormalTexture> verticesList();
            std::list<D3DXFROMWINEVECTOR3> supportList();

            float distance = 0;
            float textureLenght = 0.5f;

            D3DXFROMWINEVECTOR3 currentNormal = D3DXFROMWINEVECTOR3.Up;
            for (int i = 1; i < trackPoints.Count - 1; i++)
            {
                D3DXFROMWINEVECTOR3 next2 = trackPoints[i + 1] - trackPoints[i];
                next2.Normalize();
                D3DXFROMWINEVECTOR3 previous = trackPoints[i] - trackPoints[i - 1];
                previous.Normalize();

                D3DXFROMWINEVECTOR3 split = D3DXFROMWINEVECTOR3.Cross(next2, previous);
                D3DXFROMWINEVECTOR3 mySide = D3DXFROMWINEVECTOR3.Cross(next2, split);

                currentNormal = currentNormal + 0.2f * D3DXFROMWINEVECTOR3.Up + mySide * 2.0f;

                supportList.Add(D3DXFROMWINEVECTOR3(trackPoints[i].X, -0.5f, trackPoints[i].Z));
                supportList.Add(trackPoints[i]);

                D3DXFROMWINEVECTOR3 side = D3DXFROMWINEVECTOR3.Cross(currentNormal, next2);
                side.Normalize();
                currentNormal = D3DXFROMWINEVECTOR3.Cross(next2, side);

                D3DXFROMWINEVECTOR3 outerPoint = trackPoints[i] + side * halfTrackWidth;
                D3DXFROMWINEVECTOR3 innerPoint = trackPoints[i] - side * halfTrackWidth;

                distance += next2.size()();
                stx_VertexPositionNormalTexture vertex;
                vertex = stx_VertexPositionNormalTexture(innerPoint, currentNormal, D3DXFROMWINEVECTOR2(0, distance / textureLenght));
                verticesList.Add(vertex);
                vertex = stx_VertexPositionNormalTexture(outerPoint, currentNormal, D3DXFROMWINEVECTOR2(1, distance / textureLenght));
                verticesList.Add(vertex);
            }
            verticesList.Add(verticesList[0]);
            verticesList.Add(verticesList[1]);

            supportList.Add(D3DXFROMWINEVECTOR3.Zero);
            supportList.Add(D3DXFROMWINEVECTOR3.Zero);
            supportVertices = XNAUtils.VerticesFromVector3List(supportList, Color.Yellow);

            std::vector<stx_VertexPositionNormalTexture> trackVertices = verticesList.ToArray();

            return trackVertices;
        }        

        void LoadContent()
        {

	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	road=IRenderer::GetRendererInstance()->addImageLibTexture("/17_Racing_Track/road.png", false, ss);

trackVertexEffect = IRenderer::GetRendererInstance()->addShaderFromFile("/17_Racing_Track/trackEffect.fx", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	trackVertexFormat = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), trackVertexEffect);
VertexEffect = IRenderer::GetRendererInstance()->addShaderFromFile("/17_Racing_Track/Effect.fx", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 4
	};
	VertexFormat = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), VertexEffect);
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

	IRenderer::GetRendererInstance()->setShader(expEffect);
	IRenderer::GetRendererInstance()->setVertexFormat(trackVertexFormat);
                //device.DrawUserPrimitives<stx_VertexPositionNormalTexture>(PrimitiveType.TriangleStrip, trackVertices, 0, trackVertices.size() - 2);

	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, trackVertices.size()-2, &trackVertices[0], &trackVertices[0], sizeof(stx_VertexPositionNormalTexture));

            //render supports
            IRenderer::GetRendererInstance()->setVertexFormat(basicEffect);

	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_LINES, supportVertices.size()/2, &supportVertices[0], &supportVertices[0], sizeof(VertexPositionColor));

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("17_Racing_Track Banking");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}
