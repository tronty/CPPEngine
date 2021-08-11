/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

        int waterWidth = 128;
        int waterHeight = 128;
        std::vector<VertexPositionTexture> waterVertexBuffer;
        std::vector<__WORD__> waterIndexBuffer;
        VertexFormatID myVertexDeclaration=-1;
        TextureID waterBumps=-1; 
        TextureID skyboxTexture=-1;
	ShaderID expEffect=-1;

        D3DXFROMWINEMATRIX I, ProjectionMatrix, ViewMatrix;
        D3DXFROMWINEVECTOR3 Position;

	std::vector<VertexPositionTexture> vertices;
	std::vector<int> indices;

        std::vector<VertexPositionTexture> CreateWaterVertices()
        {
            std::vector<VertexPositionTexture> waterVertices(waterWidth * waterHeight);

            int i = 0;
            for (int z = 0; z < waterHeight; z++)
            {
                for (int x = 0; x < waterWidth; x++)
                {
                    D3DXFROMWINEVECTOR3 position(x, 0, -z);
                    D3DXFROMWINEVECTOR2 texCoord((float)x / 30.0f, (float)z / 30.0f);

                    waterVertices[i++] = VertexPositionTexture(position, texCoord);
                }
            }
            return waterVertices;
        }

        std::vector<int> CreateWaterIndices()
        {
            std::vector<int> waterIndices((waterWidth) * 2 * (waterHeight - 1));

            int i = 0;
            int z = 0;
            while (z < waterHeight - 1)
            {
                for (int x = 0; x < waterWidth; x++)
                {
                    waterIndices[i++] = x + z * waterWidth;
                    waterIndices[i++] = x + (z + 1) * waterWidth;
                }
                z++;

                if (z < waterHeight - 1)
                {
                    for (int x = waterWidth - 1; x >= 0; x--)
                    {
                        waterIndices[i++] = x + (z + 1) * waterWidth;
                        waterIndices[i++] = x + z * waterWidth;
                    }
                }
                z++;
            }

            return waterIndices;
        }

        void CreateBuffers(std::vector<VertexPositionTexture> avertices, std::vector<int> aindices)
        {
            waterVertexBuffer = IRenderer::GetRendererInstance()->addVertexBuffer(avertices.size() * sizeof(VertexPositionTexture),STATIC,&avertices[0]);
            waterIndexBuffer = IRenderer::GetRendererInstance()->addIndexBuffer(aindices.size() , sizeof(int),STATIC,&aindices[0]);
        }
/*    
void UpdateViewMatrix()
        {
            Matrix cameraRotation = Matrix.CreateRotationX(updownRot) * Matrix.CreateRotationY(leftrightRot);

            D3DXFROMWINEVECTOR3 cameraOriginalTarget(0, 0, -1);
            D3DXFROMWINEVECTOR3 cameraOriginalUpVector(0, 1, 0);

            D3DXFROMWINEVECTOR3 cameraRotatedTarget = D3DXFROMWINEVECTOR3.Transform(cameraOriginalTarget, cameraRotation);
            D3DXFROMWINEVECTOR3 cameraFinalTarget = cameraPosition + cameraRotatedTarget;

            D3DXFROMWINEVECTOR3 cameraRotatedUpVector = D3DXFROMWINEVECTOR3.Transform(cameraOriginalUpVector, cameraRotation);
            D3DXFROMWINEVECTOR3 cameraFinalUpVector = cameraPosition + cameraRotatedUpVector;

            viewMatrix = Matrix.CreateLookAt(cameraPosition, cameraFinalTarget, cameraRotatedUpVector);
        }
*/
int init(const char* aTitle)
{
        expEffect = IRenderer::GetRendererInstance()->addShaderFromFile("/15_Ocean_Water/oceanwater.fx", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	myVertexDeclaration = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), expEffect);
            SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	    waterBumps = IRenderer::GetRendererInstance()->addImageLibTexture("/15_Ocean_Water/waterbumps.bmp", false, ss);
	skyboxTexture = IRenderer::GetRendererInstance()->addImageLibTexture("/15_Ocean_Water/skybox02.dds", false, ss);

            std::vector<VertexPositionTexture> waterVertices = CreateWaterVertices();
            std::vector<int> waterIndices = CreateWaterIndices();
            CreateBuffers(waterVertices, waterIndices);

	    D3DXFROMWINEMatrixPerspectiveFovLH(&ProjectionMatrix, D3DXFROMWINE_PI/4.0f, float(IRenderer::GetRendererInstance()->GetViewportWidth())/IRenderer::GetRendererInstance()->GetViewportHeight(), 0.5f, 1000.0f);

	D3DXFROMWINEMatrixTranslation(&ViewMatrix, 110.0f, 1.1f, -110.0f);

	//UpdateViewMatrix()

	D3DXFROMWINEMatrixIdentity(&I);
	//D3DXFROMWINEMatrixIdentity(&ViewMatrix);
        //Position=D3DXFROMWINEVECTOR3(110.0f, 1.1f, -110.0f);
	//Position=D3DXFROMWINEVECTOR3(0.0f, 1.0f, 15.0f);
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();           

            //draw water                        
            float time = (float)timeGetTime() / 1000.0f;
            D3DXFROMWINEVECTOR4 waveFreqs(1, 2, 0.5f, 1.5f);
            D3DXFROMWINEVECTOR4 waveHeights(0.3f, 0.4f, 0.2f, 0.3f);
            //waveHeights = D3DXFROMWINEVECTOR4(0, 0, 0, 0);
            D3DXFROMWINEVECTOR4 waveLengths(10, 5, 15, 7);

            D3DXFROMWINEVECTOR2 waveDirs[4];
            waveDirs[0] = D3DXFROMWINEVECTOR2(-1, 0);
            waveDirs[1] = D3DXFROMWINEVECTOR2(-1, 0.5f);
            waveDirs[2] = D3DXFROMWINEVECTOR2(-1, 0.7f);
            waveDirs[3] = D3DXFROMWINEVECTOR2(-1, -0.5f);

            for (int i = 0; i < 4; i++)
                D3DXFROMWINEVec2Normalize(&waveDirs[i],&waveDirs[i]);

	IRenderer::GetRendererInstance()->setShader(expEffect);
                IRenderer::GetRendererInstance()->setVertexFormat(myVertexDeclaration);

            IRenderer::GetRendererInstance()->setShaderConstant4x4f("xWorld", I);
            IRenderer::GetRendererInstance()->setShaderConstant4x4f("xView", ViewMatrix);
            IRenderer::GetRendererInstance()->setTexture("BumpMapSampler", waterBumps);
            IRenderer::GetRendererInstance()->setShaderConstant4x4f("xProjection", ProjectionMatrix);
            IRenderer::GetRendererInstance()->setShaderConstant1f("xBumpStrength", 0.5f);

            IRenderer::GetRendererInstance()->setTexture("CubeMapSampler", skyboxTexture);
            IRenderer::GetRendererInstance()->setShaderConstant1f("xTexStretch", 4.0f);
            IRenderer::GetRendererInstance()->setShaderConstant3f("xCameraPos", Position);
            IRenderer::GetRendererInstance()->setShaderConstant1f("xTime", time);

            IRenderer::GetRendererInstance()->setShaderConstant4f("xWaveSpeeds", waveFreqs);
            IRenderer::GetRendererInstance()->setShaderConstant4f("xWaveHeights", waveHeights);
            IRenderer::GetRendererInstance()->setShaderConstant4f("xWaveLengths", waveLengths);
            IRenderer::GetRendererInstance()->setShaderConstant2f("xWaveDir0", waveDirs[0]);
            IRenderer::GetRendererInstance()->setShaderConstant2f("xWaveDir1", waveDirs[1]);
            IRenderer::GetRendererInstance()->setShaderConstant2f("xWaveDir2", waveDirs[2]);
            IRenderer::GetRendererInstance()->setShaderConstant2f("xWaveDir3", waveDirs[3]);

                IRenderer::GetRendererInstance()->setVertexBuffer(0, waterVertexBuffer);
                IRenderer::GetRendererInstance()->setIndexBuffer(waterIndexBuffer);

		IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLE_STRIP, 0, 0, waterWidth * waterHeight, 0, waterWidth * 2 * (waterHeight - 1) - 2);

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("15_Ocean_Water");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

