/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#if 0
	void MeshRenderer2::RemoveHiddenTriangles2()
	{
		std::vector<__WORD__> indicestmp=indices;	
		// ??? for(int i=indices.size()-1;i>=0;3*i--)
            	{
			D3DXFROMWINEVECTOR3 p[3],v[3],d[3];
			for(unsigned int j=0;j<indices.size()/3;j++)
            		{
				if(i==j)
					continue;
				p[0]=vertices[indices[3*j+0]].Position;
				p[1]=vertices[indices[3*j+1]].Position;
				p[2]=vertices[indices[3*j+2]].Position;
				v[0]=vertices[indices[3*i+0]].Position;
				v[1]=vertices[indices[3*i+1]].Position;
				v[2]=vertices[indices[3*i+2]].Position;
		
				for (unsigned int n=0;n<3;n++)
					for (unsigned int m=0;m<3;m++)
					{
						d[m]=v[n]-p[m];
						d[m]/=D3DXFROMWINEVec3Length(&d[m]);
					}
				for (unsigned int m=0;m<3;m++)
				{
					boor r=stx_RayIntersectsTriangle(p[m], d[m], v[0], v[1], v[2]);
					if(r) indicestmp.erase(indicestmp.begin() + 3*i, indicestmp.begin() + 3*i + 3);
				}̈́
			}
		}
		indices=indicestmp;	
	}
#endif
	void MeshRenderer2::RemoveHiddenTriangles(const std::vector<D3DXFROMWINEVECTOR3>& minmax)
	{
		for (unsigned int k=0;k<minmax.size()/2;k++)
            	{
		for (unsigned int i=0;i<indices.size()/3;i++)
            	{
			int iRemoveTriangle=0;
			{D3DXFROMWINEVECTOR3 ap=vertices[indices[3*i+0]].Position;
			 D3DXFROMWINEVECTOR3 amin=minmax[2*k+0];
			 D3DXFROMWINEVECTOR3 amax=minmax[2*k+1];		
		if((ap.x>amin.x)&&(ap.y>amin.y)&&(ap.z>amin.z)&&(ap.x<amax.x)&&(ap.y<amax.y)&&(ap.z<amax.z))
			{
				vertices.erase(vertices.begin() + indices[3*i+0]);
				iRemoveTriangle++;
			}}
			{D3DXFROMWINEVECTOR3 ap=vertices[indices[3*i+1]].Position;
			 D3DXFROMWINEVECTOR3 amin=minmax[2*k+0];
			 D3DXFROMWINEVECTOR3 amax=minmax[2*k+1];		
		if((ap.x>amin.x)&&(ap.y>amin.y)&&(ap.z>amin.z)&&(ap.x<amax.x)&&(ap.y<amax.y)&&(ap.z<amax.z))
			{
				vertices.erase(vertices.begin() + indices[3*i+1]);
				iRemoveTriangle++;
			}}
			{D3DXFROMWINEVECTOR3 ap=vertices[indices[3*i+2]].Position;
			 D3DXFROMWINEVECTOR3 amin=minmax[2*k+0];
			 D3DXFROMWINEVECTOR3 amax=minmax[2*k+1];		
		if((ap.x>amin.x)&&(ap.y>amin.y)&&(ap.z>amin.z)&&(ap.x<amax.x)&&(ap.y<amax.y)&&(ap.z<amax.z))
			{
				vertices.erase(vertices.begin() + indices[3*i+2]);
				iRemoveTriangle++;
			}}
			if(iRemoveTriangle==3)
				indices.erase(indices.begin() + 3*i, indices.begin() + 3*i + 3);
				
		}
	}}
	void MeshRenderer2::CreateCombinedShape(const std::vector<MeshRenderer2>& av, const std::vector<D3DXFROMWINEMATRIX>& aW)
	{
		vertices.clear();
		indices.clear();
		std::vector<D3DXFROMWINEVECTOR3> minmax;
            	unsigned int i=0;
            	unsigned int ind0=0;
            	while(1)
		{
		if(i>=av.size())
			break;
		if(i>=aW.size())
			break;
		//LOG_PRINT("i=%d\n", i);
		for (unsigned int j=0;j<av[i].indices.size();j++)
            	{
		//LOG_PRINT("j=%d\n", j);
			D3DXFROMWINEVECTOR3 v1=av[i].vertices[av[i].indices[j]].Position;
			stx_VertexPositionNormalTexture v2;
			D3DXFROMWINEVec3TransformCoord(&v2.Position, &v1, &aW[i]);
			vertices.push_back(v2);
			indices.push_back(av[i].indices[j]+ind0);
		}
		minmax.push_back(_min);
		minmax.push_back(_max);
		ind0+=av[i].indices.size();
		i++;
		}
		RemoveHiddenTriangles(minmax);
		for(unsigned int i=0;i<vertices.size();i++)
			stx_CalculatePositionalSphericalMapping(vertices[i]);
	    computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertices.size());

	}
	unsigned int MeshRenderer2::getVertexCount(){return vertices.size();}

	unsigned int MeshRenderer2::getTriangleCount(){return indices.size()/3;}

	VertexFormatID MeshRenderer2::getVertexDeclaration(){return VertexDeclaration;}
	void MeshRenderer2::setVertexDeclaration(VertexFormatID aVertexDeclaration){VertexDeclaration=aVertexDeclaration;}

        /// <summary>
        /// The byte size of each vertex making up the
        /// </summary>

        /// <summary>
        /// Initializes a new instance of a <see cref="MeshRenderer2"/>.
        /// Private so that a shape can only be instantiated from the Create methods.
        /// </summary>
        MeshRenderer2::MeshRenderer2()
        {
		_max=D3DXFROMWINEVECTOR3( -1e10f, -1e10f, -1e10f);
		_min=D3DXFROMWINEVECTOR3( 1e10f, 1e10f, 1e10f);
		texID = -1;
		Shader = -1;
            VertexDeclaration = -1;
        }

        /// <summary>
        /// Draws the
        /// </summary>
        /// <param name="device">The device to use to draw the </param>
        unsigned int MeshRenderer2::Draw(D3DXFROMWINEMATRIX* amat, TextureID id, ShaderID shdid, VertexFormatID vfid, D3DXFROMWINEVECTOR4 AmbientColor, D3DXFROMWINEVECTOR4 DiffuseColor, D3DXFROMWINEVECTOR4 LightDir, D3DXFROMWINEVECTOR4 EyeDir)
        {

	D3DXFROMWINEMATRIX mat;
	if(amat)
		mat=*amat;
	else
		D3DXFROMWINEMatrixIdentity(&mat);

	if(shdid > -1)
           IRenderer::GetRendererInstance()->setShader(shdid);
	else if(Shader > -1)
	{
           IRenderer::GetRendererInstance()->setShader(Shader);

	IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", mat);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("World", mat);
	IRenderer::GetRendererInstance()->setShaderConstant4f("AmbientColor", AmbientColor);
	IRenderer::GetRendererInstance()->setShaderConstant4f("DiffuseColor", DiffuseColor);
	IRenderer::GetRendererInstance()->setShaderConstant4f("vecLightDir", LightDir);
	IRenderer::GetRendererInstance()->setShaderConstant4f("vecEye", EyeDir);

		if(id > -1) IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", id);

		else if(texID > -1) IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", texID);
	}
	if(vfid > -1)
           IRenderer::GetRendererInstance()->setVertexFormat(vfid);
	else if(VertexDeclaration > -1)
           IRenderer::GetRendererInstance()->setVertexFormat(VertexDeclaration);
	unsigned int ret=0;
	if(indices.size()){
		ret=IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, vertices.size(), indices.size()/3, &indices[0], &indices[0], sizeof(__WORD__), &vertices[0], &vertices[0], sizeof(stx_VertexPositionNormalTexture));if(ret){LOG_PRINT("%s:%s:%d:ret=%x\n",__FILE__,__FUNCTION__,__LINE__,ret);}}
	else{
		ret=IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, vertices.size()/3, &vertices[0], &vertices[0], sizeof(stx_VertexPositionNormalTexture));if(ret){LOG_PRINT("%s:%s:%d:ret=%x\n",__FILE__,__FUNCTION__,__LINE__,ret);}}

#if 0

		//LOG_PRINT("Shader=%x\n", Shader);
		//LOG_PRINT("vf=%x\n", VertexDeclaration);
		//LOG_PRINT("texID=%x\n", texID);
		//LOG_PRINT("vertices.size()=%x\n", vertices.size());
		//LOG_PRINT("indices.size()=%x\n", indices.size());
#endif
		return ret;
        }
        void MeshRenderer2::BeginDraw(D3DXFROMWINEMATRIX* amat, TextureID id, ShaderID shdid, VertexFormatID vfid, D3DXFROMWINEVECTOR4 AmbientColor, D3DXFROMWINEVECTOR4 DiffuseColor, D3DXFROMWINEVECTOR4 LightDir, D3DXFROMWINEVECTOR4 EyeDir)
        {

	D3DXFROMWINEMATRIX mat;
	if(amat)
		mat=*amat;
	else
		D3DXFROMWINEMatrixIdentity(&mat);

	if(shdid > -1)
           IRenderer::GetRendererInstance()->setShader(shdid);
	else if(Shader > -1)
	{
        	IRenderer::GetRendererInstance()->setShader(Shader);
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", mat);
		IRenderer::GetRendererInstance()->setShaderConstant4f("AmbientColor", AmbientColor);
		IRenderer::GetRendererInstance()->setShaderConstant4f("DiffuseColor", DiffuseColor);
		IRenderer::GetRendererInstance()->setShaderConstant4f("vecLightDir", LightDir);
		IRenderer::GetRendererInstance()->setShaderConstant4f("vecEye", EyeDir);

		if(id > -1) IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", id);
		else if(texID > -1) IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", texID);
	}

	if (vfid > -1)
		IRenderer::GetRendererInstance()->setVertexFormat(vfid);
	else if (VertexDeclaration > -1)
		IRenderer::GetRendererInstance()->setVertexFormat(VertexDeclaration);
        }
	unsigned int MeshRenderer2::EndDraw(const unsigned int flags)
        {
		unsigned int ret=0;
	if(indices.size()){
		ret=IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, vertices.size(), indices.size()/3, &indices[0], &indices[0], sizeof(__WORD__), &vertices[0], &vertices[0], sizeof(stx_VertexPositionNormalTexture), flags);
		if(ret){LOG_PRINT("%s:%s:%d:ret=%x\n",__FILE__,__FUNCTION__,__LINE__,ret);}}
	else{
		ret=IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, vertices.size()/3, &vertices[0], &vertices[0], sizeof(stx_VertexPositionNormalTexture), flags);
		if(ret){LOG_PRINT("%s:%s:%d:ret=%x\n",__FILE__,__FUNCTION__,__LINE__,ret);}}

#if 0


		//LOG_PRINT("Shader=%x\n", Shader);

		//LOG_PRINT("vf=%x\n", VertexDeclaration);

		//LOG_PRINT("texID=%x\n", texID);
		//LOG_PRINT("vertices.size()=%x\n", vertices.size());
		//LOG_PRINT("indices.size()=%x\n", indices.size());
#endif
		return ret;
        }

        /// <summary>
        /// Fills vertices std::vector  with <see cref="stx_VertexPositionNormalTexture"/> vertices forming a box.
        /// </summary>
        /// <remarks>Always draw using a non-indexed TriangleList of 12 primitives.</remarks>
        /// <param name="device">The <see cref="GraphicsDevice"/> that is associated with the created box.</param>
        /// <param name="width">Width of the box, along the x-axis.</param>
        /// <param name="height">Height of the box, along the y-axis.</param>
        /// <param name="depth">Depth of the box, along the z-axis.</param>
        void MeshRenderer2::CreateTexturedBox(float width, float height, float depth, tShader aShader_)
        {
		m_Shape=eTexturedBox;
            // Because the box is centered at the origin, need to divide by two to find the + and - offsets
            width = width / 2.0f;
            height = height / 2.0f;
            depth = depth / 2.0f;

            D3DXFROMWINEVECTOR3 topLeftFront(-width, height, depth);
            D3DXFROMWINEVECTOR3 bottomLeftFront(-width, -height, depth);
            D3DXFROMWINEVECTOR3 topRightFront(width, height, depth);
            D3DXFROMWINEVECTOR3 bottomRightFront(width, -height, depth);
            D3DXFROMWINEVECTOR3 topLeftBack(-width, height, -depth);
            D3DXFROMWINEVECTOR3 topRightBack(width, height, -depth);
            D3DXFROMWINEVECTOR3 bottomLeftBack(-width, -height, -depth);
            D3DXFROMWINEVECTOR3 bottomRightBack(width, -height, -depth);

            D3DXFROMWINEVECTOR2 textureTopLeft(0.0f, 0.0f);
            D3DXFROMWINEVECTOR2 textureBottomLeft(0.0f, 1.0f);
            D3DXFROMWINEVECTOR2 textureTopRight(1.0f, 0.0f);
            D3DXFROMWINEVECTOR2 textureBottomRight(1.0f, 1.0f);

            D3DXFROMWINEVECTOR3 frontNormal(0.0f, 0.0f, 1.0f);
            D3DXFROMWINEVECTOR3 backNormal(0.0f, 0.0f, -1.0f);
            D3DXFROMWINEVECTOR3 topNormal(0.0f, 1.0f, 0.0f);
            D3DXFROMWINEVECTOR3 bottomNormal(0.0f, -1.0f, 0.0f);
            D3DXFROMWINEVECTOR3 leftNormal(-1.0f, 0.0f, 0.0f);
            D3DXFROMWINEVECTOR3 rightNormal(1.0f, 0.0f, 0.0f);
		
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftFront, frontNormal, textureBottomLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightFront, frontNormal, textureBottomRight));
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftFront, frontNormal, textureTopLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(topRightFront, frontNormal, textureTopRight));
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);
		
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftBack, backNormal, textureBottomLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightBack, backNormal, textureBottomRight));
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftBack, backNormal, textureTopLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(topRightBack, backNormal, textureTopRight));
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);
	
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftFront, frontNormal, textureBottomLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(topRightFront, frontNormal, textureBottomRight));
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftBack, backNormal, textureTopLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(topRightBack, backNormal, textureTopRight));
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);
	
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftFront, frontNormal, textureBottomLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightFront, frontNormal, textureBottomRight));
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftBack, backNormal, textureTopLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightBack, rightNormal, textureTopRight));
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);
	
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightBack, rightNormal, textureBottomLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightFront, frontNormal, textureBottomRight));
	vertices.push_back(stx_VertexPositionNormalTexture(topRightBack, backNormal, textureTopLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(topRightFront, frontNormal, textureTopRight));
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);

	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftBack, backNormal, textureBottomLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftFront, frontNormal, textureBottomRight));
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftBack, backNormal, textureTopLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftFront, frontNormal, textureTopRight));
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);

		D3DXFROMWINEVECTOR3 aiVecs[2];D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);

            // Create the actual vertex buffer

	    computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertices.size());
            

            
            

	if(aShader_!=eShaderNone)
	{
        	const char* aShaderName=GetShaderName(aShader_);
		Shader = FW3ShadersFactory::GetShader(aShaderName, "main", "main");
		VertexDeclaration = FW3ShadersFactory::GetVertexFormat(eTexturedBox, Shader);
	}
	else
	{
        	Shader = -1;
		VertexDeclaration = -1;
	}
	
            
	texID=-1;
	
        }
        void MeshRenderer2::CreateTexturedQuad(float width, float height, tShader aShader_)
        {
		m_Shape=eTexturedQuad;
            // Because the box is centered at the origin, need to divide by two to find the + and - offsets
            width = width / 2.0f;
            height = height / 2.0f;

            D3DXFROMWINEVECTOR3 topLeftFront(-width, height, 0.0f);
            D3DXFROMWINEVECTOR3 bottomLeftFront(-width, -height, 0.0f);
            D3DXFROMWINEVECTOR3 topRightFront(width, height, 0.0f);
            D3DXFROMWINEVECTOR3 bottomRightFront(width, -height, 0.0f);

            D3DXFROMWINEVECTOR2 textureTopLeft(0.0f, 0.0f);
            D3DXFROMWINEVECTOR2 textureTopRight(1.0f, 0.0f);
            D3DXFROMWINEVECTOR2 textureBottomLeft(0.0f, 1.0f);
            D3DXFROMWINEVECTOR2 textureBottomRight(1.0f, 1.0f);

            D3DXFROMWINEVECTOR3 frontNormal(0.0f, 0.0f, 1.0f);

            // Front face.
            vertices.push_back(stx_VertexPositionNormalTexture(topLeftFront, frontNormal, textureTopLeft, D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f)));
            vertices.push_back(stx_VertexPositionNormalTexture(topRightFront, frontNormal, textureTopRight, D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f)));
            vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftFront, frontNormal, textureBottomLeft, D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f)));
            vertices.push_back(stx_VertexPositionNormalTexture(bottomRightFront, frontNormal, textureBottomRight, D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f)));

            unsigned int indices_[] = { 1, 0, 2, 1, 2, 3};

	    for(unsigned int i=0;i<elementsOf(indices_);i++)
		indices.push_back(indices_[i]);
            
            

		D3DXFROMWINEVECTOR3 aiVecs[2];D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);

            // Create the actual vertex buffer

	    // ??? computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertices.size());
            

            
            

	if(aShader_!=eShaderNone)
	{
        	const char* aShaderName=GetShaderName(aShader_);
		Shader = FW3ShadersFactory::GetShader(aShaderName, "main", "main");
		VertexDeclaration = FW3ShadersFactory::GetVertexFormat(eTexturedBox, Shader);
	}
	else
	{
        	Shader = -1;
		VertexDeclaration = -1;
	}
	
	
	texID=-1;
	
        }
        /// <summary>
        /// Fills vertices std::vector  with <see cref="stx_VertexPositionNormalTexture"/> vertices forming a box.
        /// </summary>
        /// <remarks>Always draw using a non-indexed TriangleList of 12 primitives.</remarks>
        /// <param name="device">The <see cref="GraphicsDevice"/> that is associated with the created box.</param>
        /// <param name="width">Width of the box, along the x-axis.</param>
        /// <param name="height">Height of the box, along the y-axis.</param>
        /// <param name="depth">Depth of the box, along the z-axis.</param>
        void MeshRenderer2::CreateBox(float width, float height, float depth, tShader aShader_)
        {
		m_Shape=eBox;
            // Because the box is centered at the origin, need to divide by two to find the + and - offsets
            width = width / 2.0f;
            height = height / 2.0f;
            depth = depth / 2.0f;

            D3DXFROMWINEVECTOR3 topLeftFront(-width, height, depth);
            D3DXFROMWINEVECTOR3 bottomLeftFront(-width, -height, depth);
            D3DXFROMWINEVECTOR3 topRightFront(width, height, depth);
            D3DXFROMWINEVECTOR3 bottomRightFront(width, -height, depth);
            D3DXFROMWINEVECTOR3 topLeftBack(-width, height, -depth);
            D3DXFROMWINEVECTOR3 topRightBack(width, height, -depth);
            D3DXFROMWINEVECTOR3 bottomLeftBack(-width, -height, -depth);
            D3DXFROMWINEVECTOR3 bottomRightBack(width, -height, -depth);

            D3DXFROMWINEVECTOR3 frontNormal(0.0f, 0.0f, 1.0f);
            D3DXFROMWINEVECTOR3 backNormal(0.0f, 0.0f, -1.0f);
            D3DXFROMWINEVECTOR3 topNormal(0.0f, 1.0f, 0.0f);
            D3DXFROMWINEVECTOR3 bottomNormal(0.0f, -1.0f, 0.0f);
            D3DXFROMWINEVECTOR3 leftNormal(-1.0f, 0.0f, 0.0f);
            D3DXFROMWINEVECTOR3 rightNormal(1.0f, 0.0f, 0.0f);
		
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topRightFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);
		
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topRightBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);
	
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topRightFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topRightBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);
	
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightBack, rightNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);
	
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightBack, rightNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topRightBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topRightFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);

	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);

		D3DXFROMWINEVECTOR3 aiVecs[2];D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);

            // Create the actual vertex buffer
	    computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertices.size());
            

            
            

	if(aShader_!=eShaderNone)
	{
        	const char* aShaderName=GetShaderName(aShader_);
		Shader = FW3ShadersFactory::GetShader(aShaderName, "main", "main");
		VertexDeclaration = FW3ShadersFactory::GetVertexFormat(eTexturedBox, Shader);
	}
	else
	{
        	Shader = -1;
		VertexDeclaration = -1;
	}
	
        }

        /// <summary>
        /// Fills vertices std::vector  with <see cref="stx_VertexPositionNormalTexture"/> vertices forming a sphere.
        /// </summary>
        /// <remarks>
        /// Draw with an INDEXED TriangleList.  The values to use are as follows:
        /// Vertex Count    = slices * (stacks - 1) + 2
        /// Primitive Count = slices * (stacks - 1) * 2
        /// </remarks>
        /// <param name="device">The <see cref="GraphicsDevice"/> that is associated with the created sphere.</param>
        /// <param name="radius">Radius of the sphere. This value should be greater than or equal to 0.0f.</param>
        /// <param name="slices">Number of slices about the Y axis.</param>
        /// <param name="stacks">Number of stacks along the Y axis. Should be 2 or greater. (stack of 1 is just a cylinder)</param>
        void MeshRenderer2::CreateSphere(float radius, tShader aShader_)
        {
		int slices = (int) (4 * std::cbrt(radius) + 8);
        	int stacks = (int) (4 * std::cbrt(radius) + 8);
		CreateSphere_(radius, slices, stacks, aShader_);
	}
        void MeshRenderer2::CreateSphere_(float radius, int aslices, int stacks, tShader aShader_)
        {
		m_Shape=eSphere;
            unsigned int slices=aslices;//+1;
	    float sliceStep = 2.0f*D3DXFROMWINE_PI / slices;
            float stackStep = D3DXFROMWINE_PI / stacks;
            unsigned int vertexCount = slices * (stacks - 1) + 2;
            unsigned int triangleCount = slices * (stacks - 1) * 2;
            unsigned int indexCount = triangleCount * 3;

            vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, -radius, 0), D3DXFROMWINEVECTOR3(0, -1, 0)));
            float stackAngle = D3DXFROMWINE_PI - stackStep;
            for (unsigned int i = 0; i < stacks - 1; i++)
            {
                float sliceAngle = 0;
                for (unsigned int j = 0; j < slices; j++)
                {
                    //NOTE: y and z were switched from normal spherical coordinates because the sphere is "oriented" along the Y axis as opposed to the Z axis
                    float z = (float)(radius * sinf(stackAngle) * sinf(sliceAngle));
                    float x = (float)(radius * sinf(stackAngle) * cosf(sliceAngle));
                    float y = (float)(radius * cosf(stackAngle));
                    D3DXFROMWINEVECTOR3 position(x, y, z);
			D3DXFROMWINEVECTOR3 normal(x, y, z);
			D3DXFROMWINEVec3Normalize(&normal, &normal);
    			float tu = asinf(normal.x)/D3DXFROMWINE_PI + 0.5;
    			float tv = -asinf(normal.y)/D3DXFROMWINE_PI - 0.5;

                   vertices.push_back(stx_VertexPositionNormalTexture(position, normal, D3DXFROMWINEVECTOR2(tu, tv), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f)));
                    sliceAngle += sliceStep;
                }
                stackAngle -= stackStep;
            }
            vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, radius, 0), D3DXFROMWINEVECTOR3(0, 1, 0)));

		D3DXFROMWINEVECTOR3 aiVecs[2];
		D3DXFROMWINEMATRIX m;
		//D3DXFROMWINEMatrixIdentity(&m);		
	D3DXFROMWINEMatrixScaling(&m, radius, radius, radius); // comment this line out to avoid a bug ???
	D3DXFROMWINEMatrixIdentity(&m);
		CalculateBounds(&aiVecs[0], &m);
	    computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertexCount);
            CreateIndexBuffer(vertexCount, indexCount, slices);
            
            

	if(aShader_!=eShaderNone)
	{
        	const char* aShaderName=GetShaderName(aShader_);
		Shader = FW3ShadersFactory::GetShader(aShaderName, "main", "main");
		VertexDeclaration = FW3ShadersFactory::GetVertexFormat(eTexturedBox, Shader);
	}
	else
	{
        	Shader = -1;
		VertexDeclaration = -1;
	}
	
        }

        void MeshRenderer2::CreateCone(float bottomRadius, float height, tShader aShader_)
        {
		m_Shape=eSphere;
        int slices = 20;//(int) (4 * std::cbrt(bottomRadius) + 8);
        float angle = 2.0f*D3DXFROMWINE_PI / slices;
        vertices.resize((slices + 2));
        int i = 0;
        for (int s = 0; s < slices; s++) {
            vertices[i].Position.x = bottomRadius * cosf(s * angle);
            vertices[i].Position.y = -height / 2;
            vertices[i].Position.z = bottomRadius * sinf(s * angle);
		i++;
        }
        vertices[i].Position.x = vertices[i].Position.z = 0;
        vertices[i].Position.y = -height / 2;
		i++;
        vertices[i].Position.y = height / 2;
        vertices[i].Position.x = vertices[i].Position.z = 0;
		i++;
/*
        indices = new short[slices * 6];
        short p = 0;
        for (i = 0; i < indices.length - 6; i += 3) {
            indices[i++] = indices[i + 2] = p++;
            indices[i++] = indices[i + 2] = p;
            indices[i++] = indices[i + 2] = slices;
            indices[i + 2]++;
        }
        indices[i++] = indices[i + 2] = p;
        indices[i++] = indices[i + 2] = 0;
        indices[i++] = indices[i + 2] = slices;
        indices[i + 2]++;
*/
#if 1
        indices.resize(slices * 6);
        int p = 0;
        for (i = 0; i < indices.size() - 6; i += 3) {
            indices[i++] = indices[i + 2] = p++;
            indices[i++] = indices[i + 2] = p;
            indices[i++] = indices[i + 2] = slices;
            indices[i + 2]++;
        }
        indices[i++] = indices[i + 2] = p;
        indices[i++] = indices[i + 2] = 0;
        indices[i++] = indices[i + 2] = slices;
        indices[i + 2]++;
#endif

        for (i = 0; i < vertices.size();i++) 
		stx_CalculatePositionalSphericalMapping(vertices[i]);
		D3DXFROMWINEVECTOR3 aiVecs[2];
		D3DXFROMWINEMATRIX m;	
	D3DXFROMWINEMatrixScaling(&m, bottomRadius, bottomRadius, bottomRadius); // comment this line out to avoid a bug ???
	D3DXFROMWINEMatrixIdentity(&m);
		CalculateBounds(&aiVecs[0], &m);
	    computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertices.size());
            CreateIndexBuffer(vertices.size(), indices.size(), slices);
	if(aShader_!=eShaderNone)
	{
        	const char* aShaderName=GetShaderName(aShader_);
		Shader = FW3ShadersFactory::GetShader(aShaderName, "main", "main");
		VertexDeclaration = FW3ShadersFactory::GetVertexFormat(eTexturedBox, Shader);
	}
	else
	{
        	Shader = -1;
		VertexDeclaration = -1;
	}
        }

        void MeshRenderer2::CreateHalfSphere(float radius, tShader aShader_)
        {
		m_Shape=eHalfSphere;
		int slices = (int) (4 * std::cbrt(radius) + 8);
        	int stacks = (int) (4 * std::cbrt(radius) + 8);
            
	    float sliceStep = 2.0f*D3DXFROMWINE_PI / slices;
            float stackStep = 0.5f*D3DXFROMWINE_PI / stacks;
            unsigned int vertexCount = slices * (stacks - 1) + 2;
            unsigned int triangleCount = slices * (stacks - 1) * 2;
            unsigned int indexCount = triangleCount * 3;

            vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, radius, 0), D3DXFROMWINEVECTOR3(0, 1, 0)));
            //float stackAngle = D3DXFROMWINE_PI - stackStep;
            float stackAngle = 0.0f;
            for (unsigned int i = 0; i < stacks - 1; i++)
            {
                float sliceAngle = 0.0f;
                for (unsigned int j = 0; j < slices; j++)
                {
                    //NOTE: y and z were switched from normal spherical coordinates because the sphere is "oriented" along the Y axis as opposed to the Z axis
                    float x = (float)(radius * sinf(stackAngle) * cosf(sliceAngle));
                    float y = (float)(radius * cosf(stackAngle));
                    float z = (float)(radius * sinf(stackAngle) * sinf(sliceAngle));
                    D3DXFROMWINEVECTOR3 position(x, y, z);
			D3DXFROMWINEVECTOR3 normal(x, y, z);
			D3DXFROMWINEVec3Normalize(&normal, &normal);
    			float tu = asinf(normal.x)/D3DXFROMWINE_PI + 0.5;
    			float tv = -asinf(normal.y)/D3DXFROMWINE_PI - 0.5;

                   vertices.push_back(stx_VertexPositionNormalTexture(position, normal, D3DXFROMWINEVECTOR2(tu, tv), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f)));
                    sliceAngle += sliceStep;
                }
                stackAngle += stackStep;
            }
            //vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, radius, 0), D3DXFROMWINEVECTOR3(0, 1, 0)));

		D3DXFROMWINEVECTOR3 aiVecs[2];D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);
	    computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertices.size());
            CreateIndexBuffer(vertices.size(), indexCount, slices, false);
            
            

	if(aShader_!=eShaderNone)
	{
        	const char* aShaderName=GetShaderName(aShader_);
		Shader = FW3ShadersFactory::GetShader(aShaderName, "main", "main");
		VertexDeclaration = FW3ShadersFactory::GetVertexFormat(eTexturedBox, Shader);
	}
	else
	{
        	Shader = -1;
		VertexDeclaration = -1;
	}
	
        }
        /// <summary>
        /// Fills vertices std::vector  with <see cref="stx_VertexPositionNormalTexture"/> vertices forming a cylinder.
        /// </summary>
        /// <remarks>
        /// Draw with an INDEXED TriangleList.  The values to use are as follows:
        /// Vertex Count    = slices * (stacks + 1) + 2
        /// Primitive Count = slices * (stacks + 1) * 2
        /// </remarks>
        /// <param name="device">The <see cref="GraphicsDevice"/> that is associated with the created cylinder.</param>
        /// <param name="bottomRadius">Radius at the negative Y end. Value should be greater than or equal to 0.0f.</param>
        /// <param name="topRadius">Radius at the positive Y end. Value should be greater than or equal to 0.0f.</param>
        /// <param name="length">Length of the cylinder along the Y-axis.</param>
        /// <param name="slices">Number of slices about the Y axis.</param>
        /// <param name="stacks">Number of stacks along the Y axis.</param>
        void MeshRenderer2::CreateCylinder(float bottomRadius, float topRadius, float length, tShader aShader_)
        {
		m_Shape=eCylinder;
		int slices = (int) (4 * std::cbrt((bottomRadius+topRadius)*0.5) + 8);
        	int stacks = (int) (4 * std::cbrt((bottomRadius+topRadius)*0.5) + 8);
		
            // if both the top and bottom have a radius of zero, just return 0, because invalid
            if (bottomRadius <= 0 && topRadius <= 0)
            {

            }

            float sliceStep = 2.0f*D3DXFROMWINE_PI / slices;
            float heightStep = length / stacks;
            float radiusStep = (topRadius - bottomRadius) / stacks;
            float currentHeight = -length / 2;
            unsigned int vertexCount = (stacks + 1) * slices + 2;   //cone = stacks * slices + 1
            unsigned int triangleCount = (stacks + 1) * slices * 2; //cone = stacks * slices * 2 + slices
            unsigned int indexCount = triangleCount * 3;
            float currentRadius = bottomRadius;

            // Start at the bottom of the cylinder
            vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, currentHeight, 0), D3DXFROMWINEVECTOR3(0, -1, 0)));
	stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
            for (unsigned int i = 0; i <= stacks; i++)
            {
                float sliceAngle = 0;
                for (unsigned int j = 0; j < slices; j++)
                {
                    float x = currentRadius * (float)cosf(sliceAngle);
                    float y = currentHeight;
                    float z = currentRadius * (float)sinf(sliceAngle);

                    D3DXFROMWINEVECTOR3 position(x, y, z);
			D3DXFROMWINEVECTOR3 normal(x, y, z);

			D3DXFROMWINEVec3Normalize(&normal, &normal);

                    vertices.push_back(stx_VertexPositionNormalTexture(position, normal));
	stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);

                    sliceAngle += sliceStep;
                }
                currentHeight += heightStep;
                currentRadius += radiusStep;
            }
            vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, length / 2, 0), D3DXFROMWINEVECTOR3(0, 1, 0)));
	stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);

		D3DXFROMWINEVECTOR3 aiVecs[2];D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);

            // Create the actual vertex buffer object
            computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertexCount);
            CreateIndexBuffer(vertexCount, indexCount, slices);

            
            

	if(aShader_!=eShaderNone)
	{
        	const char* aShaderName=GetShaderName(aShader_);
		Shader = FW3ShadersFactory::GetShader(aShaderName, "main", "main");
		VertexDeclaration = FW3ShadersFactory::GetVertexFormat(eTexturedBox, Shader);
	}
	else
	{
        	Shader = -1;
		VertexDeclaration = -1;
	}
	
        }

        /// <summary>
        /// Fills indices std::vector  for spherical shapes like Spheres, Cylinders, and Cones.
        /// </summary>
        /// <param name="device">The <see cref="GraphicsDevice"/> that is associated with the created cylinder.</param>
        /// <param name="vertexCount">The total number of vertices making up the </param>
        /// <param name="indexCount">The total number of indices making up the </param>
        /// <param name="slices">The number of slices about the Y axis.</param>
        /// <returns>The index buffer containing the index data for the </returns>
        void MeshRenderer2::CreateIndexBuffer(unsigned int vertexCount, unsigned int indexCount, unsigned int slices, bool abBottom)
        {
            indices.resize(indexCount);
            unsigned int currentIndex = 0;

            // Bottom circle/cone of shape
            for (unsigned int i = 1; i <= slices; i++)
            {
                indices[currentIndex++] = 0;
                indices[currentIndex++] = i;
                if (i - 1 == 0)
                    indices[currentIndex++] = i + (unsigned int)slices - 1;
                else
                    indices[currentIndex++] = i - 1;
            }

            // Middle sides of shape
            for (unsigned int i = 1; i < vertexCount - (unsigned int)slices - 1; i++)
            {
                indices[currentIndex++] = i;
                indices[currentIndex++] = i + (unsigned int)slices;
                if ((i - 1) % slices == 0)
                    indices[currentIndex++] = i + (unsigned int)slices + (unsigned int)slices - 1;
                else
                    indices[currentIndex++] = i + (unsigned int)slices - 1;

                if ((i - 1) % slices == 0)
                    indices[currentIndex++] = i + (unsigned int)slices - 1;
                else
                    indices[currentIndex++] = i - 1;
                indices[currentIndex++] = i;
                if ((i - 1) % slices == 0)
                    indices[currentIndex++] = i + (unsigned int)slices + (unsigned int)slices - 1;
                else
                    indices[currentIndex++] = i + (unsigned int)slices - 1;
            }

            // Top circle/cone of shape
		if(abBottom)
            for (unsigned int i = (unsigned int)vertexCount - (unsigned int)slices - 1; i < vertexCount - 1; i++)
            {
                indices[currentIndex++] = (unsigned int)vertexCount - 1;
                if ((i - 1) % slices == 0)
                    indices[currentIndex++] = i + (unsigned int)slices - 1;
                else
                    indices[currentIndex++] = i - 1;
                indices[currentIndex++] = i;
            }
        }
typedef int (*sortFunction)(const void*, const void*);
int comparePosition(float *a, float *b)
{
	return stx_memcmp(a, b, sizeof(D3DXFROMWINEVECTOR3));
}

void MeshRenderer2::computeNormals(void* aVertices, unsigned int aVertexSizeInBytes, unsigned int avertexCount, bool aSmoothGroup)
{
		return;
	if(aVertexSizeInBytes<sizeof(D3DXFROMWINEVECTOR3)*2)
		return;

  __WORD__ vertexIndex;

  std::vector<float> faceNormals;
  float *normalVecs  = 0,
        *texCoords   = 0,
        *vertices    = 0,
         magnitude   = 0,
         normal[3],
         v0[3],
         v1[3];

  int (*TriangleRefs)[32];

  int  i, j, k,
       count  = 0,
       vertex = 0,
       result = 0;

  unsigned int *pFaces  = 0;

  //D3DXFROMWINEVECTOR3* meshnormalVecs  = new D3DXFROMWINEVECTOR3[getVertexCount()];
  normalVecs        = (float*)&aVertices+sizeof(D3DXFROMWINEVECTOR3);
  vertices          = (float*)&aVertices;

  if(!aSmoothGroup)
  {
    for(i=0;i<getTriangleCount();i++)
    {

      v0[0] = vertices[3*pFaces[3*i+1]+0] - vertices[3*pFaces[3*i+0]+0];
      v0[1] = vertices[3*pFaces[3*i+1]+1] - vertices[3*pFaces[3*i+0]+1];
      v0[2] = vertices[3*pFaces[3*i+1]+2] - vertices[3*pFaces[3*i+0]+2];

      v1[0] = vertices[3*pFaces[3*i+2]+0] - vertices[3*pFaces[3*i+0]+0];
      v1[1] = vertices[3*pFaces[3*i+2]+1] - vertices[3*pFaces[3*i+0]+1];
      v1[2] = vertices[3*pFaces[3*i+2]+2] - vertices[3*pFaces[3*i+0]+2];

      normal[0] = (v0[1]*v1[2]-v0[2]*v1[1]);
      normal[1] = (v0[2]*v1[0]-v0[0]*v1[2]);
      normal[2] = (v0[0]*v1[1]-v0[1]*v1[0]);

      magnitude = sqrtf(normal[0]*normal[0] +
                                 normal[1]*normal[1] +
                                 normal[2]*normal[2]);

      if(magnitude > .00001f){
        magnitude  = 1.0f/magnitude;
        normal[0] *= magnitude;
        normal[1] *= magnitude;
        normal[2] *= magnitude;
      }

      for(j=0;j<3;j++){
        normalVecs[3*pFaces[3*i+j]+0] += normal[0];
        normalVecs[3*pFaces[3*i+j]+1] += normal[1];
        normalVecs[3*pFaces[3*i+j]+2] += normal[2];
      }
    }
    return;
  }

#if 0
  //indexArray(&vertexIndex, (char *)vertices, sizeof(float)*3, getVertexCount(), (sortFunction)comparePosition);

  TriangleRefs = (int (*)[32])malloc(sizeof(int)*32*vertexIndex.count);

  if(TriangleRefs==0)
    return;

  stx_memset(TriangleRefs, 0, sizeof(int)*32*vertexIndex.count);

  for(i=0;i<getTriangleCount();i++)
  {
    for(j=0;j<3;j++){
     vertex = indexFind(&vertexIndex, &vertices[3*pFaces[3*i+j]], &result);

      if(TriangleRefs[vertex][0]<48){
        TriangleRefs[vertex][0]++;
        TriangleRefs[vertex][TriangleRefs[vertex][0]]=i;
      }
    }
  }
#endif

  faceNormals.resize(3*getTriangleCount());

  for(i=0;i<getTriangleCount();i++)
  {
    v0[0] = vertices[3*pFaces[3*i+1]+0] - vertices[3*pFaces[3*i+0]+0];
    v0[1] = vertices[3*pFaces[3*i+1]+1] - vertices[3*pFaces[3*i+0]+1];
    v0[2] = vertices[3*pFaces[3*i+1]+2] - vertices[3*pFaces[3*i+0]+2];

    v1[0] = vertices[3*pFaces[3*i+2]+0] - vertices[3*pFaces[3*i+0]+0];
    v1[1] = vertices[3*pFaces[3*i+2]+1] - vertices[3*pFaces[3*i+0]+1];
    v1[2] = vertices[3*pFaces[3*i+2]+2] - vertices[3*pFaces[3*i+0]+2];

    faceNormals[3*i+0] = (v0[1]*v1[2]-v0[2]*v1[1]);
    faceNormals[3*i+1] = (v0[2]*v1[0]-v0[0]*v1[2]);
    faceNormals[3*i+2] = (v0[0]*v1[1]-v0[1]*v1[0]);

    magnitude = sqrtf(faceNormals[3*i+0]*faceNormals[3*i+0] +
                               faceNormals[3*i+1]*faceNormals[3*i+1] +
                               faceNormals[3*i+2]*faceNormals[3*i+2]);

    if(magnitude)
    {
      magnitude           = 1.0f/magnitude;
      faceNormals[3*i+0] *= magnitude;
      faceNormals[3*i+1] *= magnitude;
      faceNormals[3*i+2] *= magnitude;
    }
  }
#if 0
  for(i=0;i<getTriangleCount();i++)
  {
    for(j=0;j<3;j++){

      vertex = indexFind(&vertexIndex, (void *)&vertices[3*pFaces[3*i+j]], &result);
      count  = 0;

      for(k=1; k <= TriangleRefs[vertex][0];k++)
      {
        if(aSmoothGroup[i] == aSmoothGroup[TriangleRefs[vertex][k]])
        {
          normalVecs[3*pFaces[3*i+j]+0] += faceNormals[3*TriangleRefs[vertex][k]+0];
          normalVecs[3*pFaces[3*i+j]+1] += faceNormals[3*TriangleRefs[vertex][k]+1];
          normalVecs[3*pFaces[3*i+j]+2] += faceNormals[3*TriangleRefs[vertex][k]+2];
          count++;
        }
      }

      normalVecs[3*pFaces[3*i+j]+0] /= (float)count;
      normalVecs[3*pFaces[3*i+j]+1] /= (float)count;
      normalVecs[3*pFaces[3*i+j]+2] /= (float)count;
    }
  }
  indexFree(&vertexIndex);

  stx_free(TriangleRefs);

#endif
}

void MeshRenderer2::computeTangentSpace(void* aVertices, unsigned int aVertexSizeInBytes, unsigned int avertexCount, bool aSmoothGroup)
{
	LOG_FNLN;
	LOG_PRINT("aVertices=%x\n", aVertices);
	LOG_PRINT("vertices.size()=%d\n", vertices.size());
	LOG_PRINT("indices.size()=%d\n", indices.size());
	LOG_PRINT("Triangle Count=%d\n", getTriangleCount());
	LOG_PRINT("avertexCount=%d\n", avertexCount);
	LOG_PRINT("aVertexSizeInBytes=%d\n", aVertexSizeInBytes);
	if(aVertexSizeInBytes<sizeof(D3DXFROMWINEVECTOR3)*4)
		return;
#if 0
  if(!Mesh->texCoords){
    computeNormals(aVertices, aVertexSizeInBytes, avertexCount);
    return;
  }
#endif

	int i, j, k, Count, Vertex=0, Result=0;
	__WORD__  VertexIndex;
	int (*TriangleRefs)[32];
	std::vector<float> FaceTangent;
	std::vector<float> FaceBinormal;
	std::vector<float> FaceNormal;
	float v0[3], v1[3], cross[3];
  float *pTexCoords = 0,
        *pBiNormals = 0,
        *pVertices  = 0,
        *pTangents  = 0,
        *pNormals   = 0,
        Mag;

	D3DXFROMWINEVECTOR3 Tangent,
          Binormal,
          Normal;

  __WORD__   *pFaces = 0;

  //meshnormalVecs  = new D3DXFROMWINEVECTOR3[getVertexCount()];
  float* normalVecs        = (float*)&aVertices+sizeof(D3DXFROMWINEVECTOR3);
  float* vertices          = (float*)&aVertices;

  pBiNormals         = (float*)&aVertices+sizeof(D3DXFROMWINEVECTOR3)*2;
  pTexCoords         = (float*)&aVertices+sizeof(D3DXFROMWINEVECTOR3)*4;
  pTangents          = (float*)&aVertices+sizeof(D3DXFROMWINEVECTOR3)*3;
  pVertices          = (float*)&aVertices;
  pNormals           = (float*)&aVertices+sizeof(D3DXFROMWINEVECTOR3);
  pFaces             = (__WORD__*)&indices[0];

  if(aSmoothGroup)
	{
		for(i=0;i<getTriangleCount();i++)
		{
			v0[0]=pVertices[3*pFaces[3*i+1]+0]-pVertices[3*pFaces[3*i+0]+0];
			v0[1]=pTexCoords[2*pFaces[3*i+1]+0]-pTexCoords[2*pFaces[3*i+0]+0];
			v0[2]=pTexCoords[2*pFaces[3*i+1]+1]-pTexCoords[2*pFaces[3*i+0]+1];

			v1[0]=pVertices[3*pFaces[3*i+2]+0]-pVertices[3*pFaces[3*i+0]+0];
			v1[1]=pTexCoords[2*pFaces[3*i+2]+0]-pTexCoords[2*pFaces[3*i+0]+0];
			v1[2]=pTexCoords[2*pFaces[3*i+2]+1]-pTexCoords[2*pFaces[3*i+0]+1];

			cross[0]=(v0[1]*v1[2]-v0[2]*v1[1]);
			cross[1]=(v0[2]*v1[0]-v0[0]*v1[2]);
			cross[2]=(v0[0]*v1[1]-v0[1]*v1[0]);

			if(fabs(cross[0])>0.000001f)
			{
				Tangent[0]  =-cross[1]/cross[0];
				Binormal[0] =cross[2]/cross[0];
			}

			v0[0]=pVertices[3*pFaces[3*i+1]+1]-pVertices[3*pFaces[3*i+0]+1];
			v0[1]=pTexCoords[2*pFaces[3*i+1]+0]-pTexCoords[2*pFaces[3*i+0]+0];
			v0[2]=pTexCoords[2*pFaces[3*i+1]+1]-pTexCoords[2*pFaces[3*i+0]+1];

			v1[0]=pVertices[3*pFaces[3*i+2]+1]-pVertices[3*pFaces[3*i+0]+1];
			v1[1]=pTexCoords[2*pFaces[3*i+2]+0]-pTexCoords[2*pFaces[3*i+0]+0];
			v1[2]=pTexCoords[2*pFaces[3*i+2]+1]-pTexCoords[2*pFaces[3*i+0]+1];

			cross[0]=(v0[1]*v1[2]-v0[2]*v1[1]);
			cross[1]=(v0[2]*v1[0]-v0[0]*v1[2]);
			cross[2]=(v0[0]*v1[1]-v0[1]*v1[0]);

			if(fabs(cross[0])>0.000001f)
			{
				Tangent[1]=-cross[1]/cross[0];
				Binormal[1]=cross[2]/cross[0];
			}

			v0[0]=pVertices[3*pFaces[3*i+1]+2]-pVertices[3*pFaces[3*i+0]+2];
			v0[1]=pTexCoords[2*pFaces[3*i+1]+0]-pTexCoords[2*pFaces[3*i+0]+0];
			v0[2]=pTexCoords[2*pFaces[3*i+1]+1]-pTexCoords[2*pFaces[3*i+0]+1];

			v1[0]=pVertices[3*pFaces[3*i+2]+2]-pVertices[3*pFaces[3*i+0]+2];
			v1[1]=pTexCoords[2*pFaces[3*i+2]+0]-pTexCoords[2*pFaces[3*i+0]+0];
			v1[2]=pTexCoords[2*pFaces[3*i+2]+1]-pTexCoords[2*pFaces[3*i+0]+1];

			cross[0]=(v0[1]*v1[2]-v0[2]*v1[1]);
			cross[1]=(v0[2]*v1[0]-v0[0]*v1[2]);
			cross[2]=(v0[0]*v1[1]-v0[1]*v1[0]);

			if(fabs(cross[0])>0.000001f)
			{
				Tangent[2]=-cross[1]/cross[0];
				Binormal[2]=cross[2]/cross[0];
			}

			Mag = sqrtf(Tangent[0]*Tangent[0]+Tangent[1]*Tangent[1]+Tangent[2]*Tangent[2]);

			if(Mag)
			{
				Mag=1.0f/Mag;
				Tangent[0]*=Mag;
				Tangent[1]*=Mag;
				Tangent[2]*=Mag;
			}

			Mag= sqrtf(Binormal[0]*Binormal[0]+Binormal[1]*Binormal[1]+Binormal[2]*Binormal[2]);

			if(Mag)
			{
				Mag=1.0f/Mag;
				Binormal[0]*=Mag;
				Binormal[1]*=Mag;
				Binormal[2]*=Mag;
			}

			v0[0]=pVertices[3*pFaces[3*i+1]+0]-pVertices[3*pFaces[3*i+0]+0];
			v0[1]=pVertices[3*pFaces[3*i+1]+1]-pVertices[3*pFaces[3*i+0]+1];
			v0[2]=pVertices[3*pFaces[3*i+1]+2]-pVertices[3*pFaces[3*i+0]+2];

			v1[0]=pVertices[3*pFaces[3*i+2]+0]-pVertices[3*pFaces[3*i+0]+0];
			v1[1]=pVertices[3*pFaces[3*i+2]+1]-pVertices[3*pFaces[3*i+0]+1];
			v1[2]=pVertices[3*pFaces[3*i+2]+2]-pVertices[3*pFaces[3*i+0]+2];

			Normal[0]=(v0[1]*v1[2]-v0[2]*v1[1]);
			Normal[1]=(v0[2]*v1[0]-v0[0]*v1[2]);
			Normal[2]=(v0[0]*v1[1]-v0[1]*v1[0]);

			Mag = sqrtf(Normal[0]*Normal[0]+Normal[1]*Normal[1]+Normal[2]*Normal[2]);

			if(Mag)
			{
				Mag=1.0f/Mag;
				Normal[0]*=Mag;
				Normal[1]*=Mag;
				Normal[2]*=Mag;
			}

			for(j=0;j<3;j++)
			{
				pTangents[3*pFaces[3*i+j]+0]+=Tangent[0];
				pTangents[3*pFaces[3*i+j]+1]+=Tangent[1];
				pTangents[3*pFaces[3*i+j]+2]+=Tangent[2];

				pBiNormals[3*pFaces[3*i+j]+0]+=Binormal[0];
				pBiNormals[3*pFaces[3*i+j]+1]+=Binormal[1];
				pBiNormals[3*pFaces[3*i+j]+2]+=Binormal[2];

				pNormals[3*pFaces[3*i+j]+0]+=Normal[0];
				pNormals[3*pFaces[3*i+j]+1]+=Normal[1];
				pNormals[3*pFaces[3*i+j]+2]+=Normal[2];
			}
		}

		return;
	}
#if 0
 	//indexArray(&VertexIndex, (char *)pVertices, sizeof(float)*3, Mesh->NumVertex, (sortFunction)comparePosition);

	TriangleRefs=(int (*)[32])malloc(sizeof(int)*32*VertexIndex.count);

	if(TriangleRefs==0)
		return;

	stx_memset(TriangleRefs, 0, sizeof(int)*32*VertexIndex.count);

	for(i=0;i<getTriangleCount();i++)
	{
		for(j=0;j<3;j++)
		{
			Vertex=indexFind(&VertexIndex, &pVertices[3*pFaces[3*i+j]], &Result);

			if(TriangleRefs[Vertex][0]<48)
			{
				TriangleRefs[Vertex][0]++;
				TriangleRefs[Vertex][TriangleRefs[Vertex][0]]=i;
			}
		}
	}
#endif
	FaceTangent.resize(3*getTriangleCount());

	FaceBinormal.resize(3*getTriangleCount());

	FaceNormal.resize(3*getTriangleCount());

	for(i=0;i<getTriangleCount();i++)
	{
	//LOG_PRINT("i=%d\n", i);
		v0[0]=pVertices[3*pFaces[3*i+1]+0]-pVertices[3*pFaces[3*i+0]+0];
		v0[1]=pTexCoords[2*pFaces[3*i+1]+0]-pTexCoords[2*pFaces[3*i+0]+0];
		v0[2]=pTexCoords[2*pFaces[3*i+1]+1]-pTexCoords[2*pFaces[3*i+0]+1];

		v1[0]=pVertices[3*pFaces[3*i+2]+0]-pVertices[3*pFaces[3*i+0]+0];
		v1[1]=pTexCoords[2*pFaces[3*i+2]+0]-pTexCoords[2*pFaces[3*i+0]+0];
		v1[2]=pTexCoords[2*pFaces[3*i+2]+1]-pTexCoords[2*pFaces[3*i+0]+1];

		cross[0]=(v0[1]*v1[2]-v0[2]*v1[1]);
		cross[1]=(v0[2]*v1[0]-v0[0]*v1[2]);
		cross[2]=(v0[0]*v1[1]-v0[1]*v1[0]);

		if(fabs(cross[0])>0.000001f)
		{
			FaceTangent[3*i+0]=-cross[1]/cross[0];
			FaceBinormal[3*i+0]=cross[2]/cross[0];
		}

		v0[0]=pVertices[3*pFaces[3*i+1]+1]-pVertices[3*pFaces[3*i+0]+1];
		v0[1]=pTexCoords[2*pFaces[3*i+1]+0]-pTexCoords[2*pFaces[3*i+0]+0];
		v0[2]=pTexCoords[2*pFaces[3*i+1]+1]-pTexCoords[2*pFaces[3*i+0]+1];

		v1[0]=pVertices[3*pFaces[3*i+2]+1]-pVertices[3*pFaces[3*i+0]+1];
		v1[1]=pTexCoords[2*pFaces[3*i+2]+0]-pTexCoords[2*pFaces[3*i+0]+0];
		v1[2]=pTexCoords[2*pFaces[3*i+2]+1]-pTexCoords[2*pFaces[3*i+0]+1];

		cross[0]=(v0[1]*v1[2]-v0[2]*v1[1]);
		cross[1]=(v0[2]*v1[0]-v0[0]*v1[2]);
		cross[2]=(v0[0]*v1[1]-v0[1]*v1[0]);

		if(fabs(cross[0])>0.000001f)
		{
			FaceTangent[3*i+1]=-cross[1]/cross[0];
			FaceBinormal[3*i+1]=cross[2]/cross[0];
		}

		v0[0]=pVertices[3*pFaces[3*i+1]+2]-pVertices[3*pFaces[3*i+0]+2];
		v0[1]=pTexCoords[2*pFaces[3*i+1]+0]-pTexCoords[2*pFaces[3*i+0]+0];
		v0[2]=pTexCoords[2*pFaces[3*i+1]+1]-pTexCoords[2*pFaces[3*i+0]+1];

		v1[0]=pVertices[3*pFaces[3*i+2]+2]-pVertices[3*pFaces[3*i+0]+2];
		v1[1]=pTexCoords[2*pFaces[3*i+2]+0]-pTexCoords[2*pFaces[3*i+0]+0];
		v1[2]=pTexCoords[2*pFaces[3*i+2]+1]-pTexCoords[2*pFaces[3*i+0]+1];

		cross[0]=(v0[1]*v1[2]-v0[2]*v1[1]);
		cross[1]=(v0[2]*v1[0]-v0[0]*v1[2]);
		cross[2]=(v0[0]*v1[1]-v0[1]*v1[0]);

		if(fabs(cross[0])>0.000001f)
		{
			FaceTangent[3*i+2]=-cross[1]/cross[0];
			FaceBinormal[3*i+2]=cross[2]/cross[0];
		}

		Mag=(float)sqrt(FaceTangent[3*i+0]*FaceTangent[3*i+0]+FaceTangent[3*i+1]*FaceTangent[3*i+1]+FaceTangent[3*i+2]*FaceTangent[3*i+2]);

		if(Mag)
		{
			Mag=1.0f/Mag;
			FaceTangent[3*i+0]*=Mag;
			FaceTangent[3*i+1]*=Mag;
			FaceTangent[3*i+2]*=Mag;
		}

		Mag=(float)sqrt(FaceBinormal[3*i+0]*FaceBinormal[3*i+0]+FaceBinormal[3*i+1]*FaceBinormal[3*i+1]+FaceBinormal[3*i+2]*FaceBinormal[3*i+2]);

		if(Mag)
		{
			Mag=1.0f/Mag;
			FaceBinormal[3*i+0]*=Mag;
			FaceBinormal[3*i+1]*=Mag;
			FaceBinormal[3*i+2]*=Mag;
		}

		v0[0]=pVertices[3*pFaces[3*i+1]+0]-pVertices[3*pFaces[3*i+0]+0];
		v0[1]=pVertices[3*pFaces[3*i+1]+1]-pVertices[3*pFaces[3*i+0]+1];
		v0[2]=pVertices[3*pFaces[3*i+1]+2]-pVertices[3*pFaces[3*i+0]+2];

		v1[0]=pVertices[3*pFaces[3*i+2]+0]-pVertices[3*pFaces[3*i+0]+0];
		v1[1]=pVertices[3*pFaces[3*i+2]+1]-pVertices[3*pFaces[3*i+0]+1];
		v1[2]=pVertices[3*pFaces[3*i+2]+2]-pVertices[3*pFaces[3*i+0]+2];

		FaceNormal[3*i+0]=(v0[1]*v1[2]-v0[2]*v1[1]);
		FaceNormal[3*i+1]=(v0[2]*v1[0]-v0[0]*v1[2]);
		FaceNormal[3*i+2]=(v0[0]*v1[1]-v0[1]*v1[0]);

		Mag=(float)sqrt(FaceNormal[3*i+0]*FaceNormal[3*i+0]+FaceNormal[3*i+1]*FaceNormal[3*i+1]+FaceNormal[3*i+2]*FaceNormal[3*i+2]);

		if(Mag)
		{
			Mag=1.0f/Mag;
			FaceNormal[3*i+0]*=Mag;
			FaceNormal[3*i+1]*=Mag;
			FaceNormal[3*i+2]*=Mag;
		}
	}

#if 0

	for(i=0;i<getTriangleCount();i++)
	{
		for(j=0;j<3;j++)
		{
			Vertex=indexFind(&VertexIndex, (void *)&pVertices[3*pFaces[3*i+j]], &Result);

			Count=0;

			for(k=1;k<=TriangleRefs[Vertex][0];k++)
			{
				if(Mesh->smoothGroup[i]==Mesh->smoothGroup[TriangleRefs[Vertex][k]])
				{
					pTangents[3*pFaces[3*i+j]+0]+=FaceTangent[3*TriangleRefs[Vertex][k]+0];
					pTangents[3*pFaces[3*i+j]+1]+=FaceTangent[3*TriangleRefs[Vertex][k]+1];
					pTangents[3*pFaces[3*i+j]+2]+=FaceTangent[3*TriangleRefs[Vertex][k]+2];

					pBiNormals[3*pFaces[3*i+j]+0]+=FaceBinormal[3*TriangleRefs[Vertex][k]+0];
					pBiNormals[3*pFaces[3*i+j]+1]+=FaceBinormal[3*TriangleRefs[Vertex][k]+1];
					pBiNormals[3*pFaces[3*i+j]+2]+=FaceBinormal[3*TriangleRefs[Vertex][k]+2];

					pNormals[3*pFaces[3*i+j]+0]+=FaceNormal[3*TriangleRefs[Vertex][k]+0];
					pNormals[3*pFaces[3*i+j]+1]+=FaceNormal[3*TriangleRefs[Vertex][k]+1];
					pNormals[3*pFaces[3*i+j]+2]+=FaceNormal[3*TriangleRefs[Vertex][k]+2];

					Count++;
				}
			}

			pTangents[3*pFaces[3*i+j]+0]/=(float)Count;
			pTangents[3*pFaces[3*i+j]+1]/=(float)Count;
			pTangents[3*pFaces[3*i+j]+2]/=(float)Count;

			pBiNormals[3*pFaces[3*i+j]+0]/=(float)Count;
			pBiNormals[3*pFaces[3*i+j]+1]/=(float)Count;
			pBiNormals[3*pFaces[3*i+j]+2]/=(float)Count;

			pNormals[3*pFaces[3*i+j]+0]/=(float)Count;
			pNormals[3*pFaces[3*i+j]+1]/=(float)Count;
			pNormals[3*pFaces[3*i+j]+2]/=(float)Count;
		}
	}
	indexFree(&VertexIndex);

	stx_free(TriangleRefs);

#endif
}


std::map<std::string, ShaderID> FW3ShadersFactory::m_FW3ShadersFactory_Shadercache;
std::map<tShape, VertexFormatID> FW3ShadersFactory::m_FW3ShadersFactory_VertexFormatcache;

VertexFormatID FW3ShadersFactory::find_from_FW3ShadersFactory_VertexFormatcache(tShape aShape)
{
	std::map<tShape, VertexFormatID>::iterator it;
	it = m_FW3ShadersFactory_VertexFormatcache.find(tShape(aShape));
	if (it != m_FW3ShadersFactory_VertexFormatcache.end())
		return it->second;
	return -1;
}
void FW3ShadersFactory::insert_to_FW3ShadersFactory_VertexFormatcache(tShape aShape, VertexFormatID id)
{
	std::map<tShape, VertexFormatID>::iterator it;
	it = m_FW3ShadersFactory_VertexFormatcache.find(tShape(aShape));
	if (it == m_FW3ShadersFactory_VertexFormatcache.end())
		m_FW3ShadersFactory_VertexFormatcache.insert ( std::pair<tShape,VertexFormatID>(tShape(aShape), id) );
}
VertexFormatID FW3ShadersFactory::GetVertexFormat(tShape aShape, const ShaderID shd)
{
	VertexFormatID newvf=-1;
	std::map<tShape, VertexFormatID>::iterator it;
	it = m_FW3ShadersFactory_VertexFormatcache.find(tShape(aShape));
	if (it != m_FW3ShadersFactory_VertexFormatcache.end())
		newvf=it->second;
	else
	{
		FormatDesc format[] =
				{
					0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
					0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
					0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
					0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
					0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
					0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
				};
		newvf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
#if 0
	else if(0)
	{
		switch(aShape)
		{
			case eTexturedBox:
			case eBox:
			case eSphere:
			case eCylinder:
#if 1
			case eCone:
			case eHemis:
			case eTorus:
#endif
			case eOcta:
			case eTetra:
			case eEllipsoid:
			{
				FormatDesc format[] =
				{
					0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
					0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
					0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
					0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
					0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
					0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
				};
				newvf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
				break;
			}
		};

#endif
		m_FW3ShadersFactory_VertexFormatcache.insert ( std::pair<tShape,VertexFormatID>(tShape(aShape),newvf) );
	}
	return newvf;
	}

	ShaderID FW3ShadersFactory::find_from_FW3ShadersFactory_Shadercache(const char* aShaderName)
	{
		std::map<std::string, ShaderID>::iterator it;
		it = m_FW3ShadersFactory_Shadercache.find(std::string(aShaderName));
		if (it != m_FW3ShadersFactory_Shadercache.end())
			return it->second;
		return -1;
	}

	void FW3ShadersFactory::insert_to_FW3ShadersFactory_Shadercache(const char* aShaderName, ShaderID id)
	{
		std::map<std::string, ShaderID>::iterator it;
		it = m_FW3ShadersFactory_Shadercache.find(std::string(aShaderName));
		if (it == m_FW3ShadersFactory_Shadercache.end())
			m_FW3ShadersFactory_Shadercache.insert ( std::pair<std::string,ShaderID>(std::string(aShaderName), id) );
	}

	ShaderID FW3ShadersFactory::GetShader(const char* aShaderName, const char *vsName, const char *psName)
	{
		ShaderID newshd;
		std::map<std::string, ShaderID>::iterator it;
		it = m_FW3ShadersFactory_Shadercache.find(std::string(aShaderName));
		if (it != m_FW3ShadersFactory_Shadercache.end())
			newshd=it->second;
		else
		{
			std::string contents;
			GetShaderTxt(std::string(aShaderName), contents);
			newshd = IRenderer::GetRendererInstance()->addHLSLShader(contents.c_str(), vsName, psName);
			m_FW3ShadersFactory_Shadercache.insert ( std::pair<std::string,ShaderID>(std::string(aShaderName),newshd) );
		}
		return newshd;
	}

void FW3ShadersFactory::GetShaderTxt(std::string aShaderName, std::string& contents)
{
	//LOG_PRINT("aShaderName=%s\n", aShaderName);
#if defined(USE_GLES_SHADERS) && (defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR))
if(stx_strcmp(aShaderName.c_str(),"AmbientLighting")==0)
	STX_Service::LoadTxtFile("/MeshRenderer2/AmbientLighting.GLES2.shd", contents);
else if(stx_strcmp(aShaderName.c_str(),"DiffuseLighting")==0)
	STX_Service::LoadTxtFile("/MeshRenderer2/DiffuseLighting.GLES2.shd", contents);
else if(stx_strcmp(aShaderName.c_str(),"PhongLighting1")==0)
	STX_Service::LoadTxtFile("/MeshRenderer2/PhongLighting1.GLES2.shd", contents);
else if(stx_strcmp(aShaderName.c_str(),"PhongLighting2")==0)
	STX_Service::LoadTxtFile("/MeshRenderer2/PhongLighting2.GLES2.shd", contents);
else STX_Service::LoadTxtFile("/MeshRenderer2/rest.GLES2.shd", contents);
#elif defined(USE_GLES_SHADERS) && defined(_LINUX)
if(stx_strcmp(aShaderName.c_str(),"AmbientLighting")==0)
	STX_Service::LoadTxtFile("/MeshRenderer2/AmbientLighting.GLES2.LNX.shd", contents);
else if(stx_strcmp(aShaderName.c_str(),"DiffuseLighting")==0)
	STX_Service::LoadTxtFile("/MeshRenderer2/DiffuseLighting.GLES2.LNX.shd", contents);
else if(stx_strcmp(aShaderName.c_str(),"PhongLighting1")==0)
	STX_Service::LoadTxtFile("/MeshRenderer2/PhongLighting1.GLES2.LNX.shd", contents);
else if(stx_strcmp(aShaderName.c_str(),"PhongLighting2")==0)
	STX_Service::LoadTxtFile("/MeshRenderer2/PhongLighting2.GLES2.LNX.shd", contents);
else STX_Service::LoadTxtFile("/MeshRenderer2/rest.GLES2.LNX.shd", contents);
#elif defined(USE_HLSL_SHADERS)
if(stx_strcmp(aShaderName.c_str(),"AmbientLighting")==0)
	STX_Service::LoadTxtFile("/MeshRenderer2/AmbientLighting.HLSL.shd", contents);
else if(stx_strcmp(aShaderName.c_str(),"DiffuseLighting")==0)
	STX_Service::LoadTxtFile("/MeshRenderer2/DiffuseLighting.HLSL.shd", contents);
else if(stx_strcmp(aShaderName.c_str(),"PhongLighting1")==0)
	STX_Service::LoadTxtFile("/MeshRenderer2/PhongLighting1.HLSL.shd", contents);
else if(stx_strcmp(aShaderName.c_str(),"PhongLighting2")==0)
	STX_Service::LoadTxtFile("/MeshRenderer2/PhongLighting2.HLSL.shd", contents);
//else if(stx_strcmp(aShaderName.c_str(),"CelShader"")==0)
//	STX_Service::LoadTxtFile("/CelShading/outline.shd", contents);
//	STX_Service::LoadTxtFile("/CelShading/shading.shd", contents);
else STX_Service::LoadTxtFile("/MeshRenderer2/rest.HLSL.shd", contents);
#else
	STX_Service::LoadTxtFile("/MeshRenderer2/all.else.shd", contents);
#endif
}

D3DXFROMWINEVECTOR3 v4tov3(D3DXFROMWINEVECTOR4& a)
{
	D3DXFROMWINEVECTOR3 r;
	r.x=a.x;
	r.y=a.y;
	r.z=a.z;
	return r;
}
void MeshRenderer2::Grid(std::vector<stx_VertexPositionNormalTexture>& vertices, int gridX, int gridZ)
{
	vertices.resize(0);//(gridX * gridZ);
	indices.resize(0);
	//mesh->resize(gridX * gridZ, (gridX - 1) * (gridZ - 1) * 2);
	int vertCnt = 0;
	int faceCnt = 0;
	for (int x = 0; x < gridX; x++)
		for (int z = 0; z < gridZ; z++)
		{
			vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(1 - (float) x / (gridX - 1), 0, 1 - (float) z / (gridZ - 1))));
	stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
			if (x < gridX - 1 && z < gridZ - 1)
			{
				indices.push_back(vertCnt + 0);
				indices.push_back(vertCnt + 1);
				indices.push_back(vertCnt + 1 + gridZ);
				indices.push_back(vertCnt + 1 + gridZ);
				indices.push_back(vertCnt + gridZ);
				indices.push_back(vertCnt + 0);
			}
			vertCnt++;
		}
	
}

void MeshRenderer2::setFace(unsigned int c, unsigned int i1, unsigned int i2, unsigned int i3)
{
	indices.push_back(i1);
	indices.push_back(i2);
	indices.push_back(i3);
}

void MeshRenderer2::makeGrid(int gridX, int gridZ)
{
	int vertCnt = 0;
	int faceCnt = 0;
	for (int x = 0; x < gridX; x++)
		for (int z = 0; z < gridZ; z++)
		{
			vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(1 - (float) x / (gridX - 1), 0, 1 - (float) z / (gridZ - 1))));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
			if (x < gridX - 1 && z < gridZ - 1)
			{
				setFace(
					faceCnt++, vertCnt + 0, vertCnt + 1, vertCnt + 1 + gridZ
					);
				setFace(
					faceCnt++, vertCnt + 1 + gridZ, vertCnt + gridZ, vertCnt + 0
					);
			}
			vertCnt++;
		}
}
void MeshRenderer2::CreateHemis(tShader aShader_)
{
	m_Shape=eHemis;
		int stepLng = (int) (4 * std::cbrt(1.0f) + 8);
        	int stepLat = (int) (4 * std::cbrt(1.0f) + 8);
	makeGrid(stepLng + 1, (stepLat / 4) + 2);
	float unitLng = 2 * D3DXFROMWINE_PI / stepLng;
	float unitLat = D3DXFROMWINE_PI / (stepLat / 2);
	int vertCnt = 0;
	for (int i = 0; i < stepLng + 1; i++)
	{
		for (int j = 0; j < (stepLat / 4) + 1; j++)
		{
			D3DXFROMWINEVECTOR3 v(0, 1, 0);
		D3DXFROMWINEMATRIX RM;
			D3DXFROMWINEMatrixRotationYawPitchRoll(&RM, j * unitLat, i * unitLng, 0.0f);
		D3DXFROMWINEVec3TransformCoord(&v, &v, &RM);
			vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(v.x, v.y, v.z)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
		}
		vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, 0, 0)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	}
	if(aShader_!=eShaderNone)
	{
        	const char* aShaderName=GetShaderName(aShader_);
		Shader = FW3ShadersFactory::GetShader(aShaderName, "main", "main");
		VertexDeclaration = FW3ShadersFactory::GetVertexFormat(eTexturedBox, Shader);
	}
	else
	{
        	Shader = -1;
		VertexDeclaration = -1;
	}
	texID=-1;
}

#if 1
void MeshRenderer2::CreateTorus(float radMajor, float radMinor, tShader aShader_)
{
	m_Shape=eTorus;
		int num_rings = (int) (4 * std::cbrt(radMinor) + 8);
        	int num_segments = (int) (4 * std::cbrt(radMajor) + 8);
	    int slices = (int) (4 * std::cbrt(radMinor) + 8);
            int stacks = (int) (4 * std::cbrt(radMajor) + 8);
	    float sliceStep = 2.0f*D3DXFROMWINE_PI / slices;
            float stackStep = D3DXFROMWINE_PI / stacks;
    // Compute torus vertices
    const float pi = D3DXFROMWINE_PI;
    const float r1 = 1.0f;
    const float r2 = 0.3f;
#if 1
    for(std::size_t i=0, index=0; i<=num_rings; ++i) {
      for(std::size_t j=0; j<=num_segments; ++j, ++index) {
#else
            float stackAngle = D3DXFROMWINE_PI - stackStep;
            for (unsigned int i = 0; i < stacks - 1; i++)
            {
                float sliceAngle = 0;
                for (unsigned int j = 0; j < slices; j++)
                {
#endif
        // Compute texture coordinates (surface parameters)
        // Note that u is added to j. This creates a spiral pattern, which
        // allows us to draw the entire torus in one triangle strip without
        // using degenerate triangles.
        // (Yet we still need degenerate triangles to avoid texture
        // wrap-around)
        float u = float(i) / num_rings;
        float v = (float(j) + u) / num_segments;

        // Compute angles
        float u_angle = u * 2 * pi;
        float v_angle = v * 2 * pi;

        // Position
        float x = cos(u_angle) * (r1 + cos(v_angle) * r2);
        float y = sin(u_angle) * (r1 + cos(v_angle) * r2);
        float z = sin(v_angle) * r2;

        // Normal vector
        float nx = cos(u_angle) * cos(v_angle);
        float ny = sin(u_angle) * cos(v_angle);
        float nz = sin(v_angle);

	stx_VertexPositionNormalTexture v3;
        v3.Position = D3DXFROMWINEVECTOR3(x, y, z);
        v3.Normal = D3DXFROMWINEVECTOR3(nx, ny, nz);
        v3.Tex = D3DXFROMWINEVECTOR2(u, v);
	stx_CalculatePositionalSphericalMapping(v3);
	vertices.push_back(v3);
#if 1
      }
    }
#else
                   sliceAngle += sliceStep;
                }
                stackAngle -= stackStep;
            }
#endif
#if 1
	{
    int idx = 0;

	int nbrRingSegments=num_rings;
        int nbrTubeSegments=num_segments;

	int nbrRingSteps=num_rings;
        int nbrTubes=num_segments;

    indices.resize(nbrRingSegments * nbrTubeSegments * 6);
    for (int t = 0; t < nbrTubeSegments; t++) {
        for (int r = 0; r < nbrRingSegments; r++) {
            int idxTL = r + t * nbrRingSteps;
            int idxBL = r + (t + 1) * nbrRingSteps;
            int idxBR = r + 1 + (t + 1) * nbrRingSteps;
            int idxTR = r + 1 + t * nbrRingSteps;
            // Top left triangle defined in anti-clockwise order
            indices[idx++] = idxTL;   // Point
            indices[idx++] = idxBL;   // Point
            indices[idx++] = idxTR;   // Point
            // Bottom right triangle defined in anti-clockwise order
            indices[idx++] = idxBL;   // Point
            indices[idx++] = idxBR;   // Point
            indices[idx++] = idxTR;   // Point
        }
    }}
#elif 1
    for(std::size_t i=0, index=0; i<=vertices.size(); ++i) {
      indices.push_back((int)(i % vertices.size()));
      indices.push_back((int)((i + num_segments+1) % vertices.size()));
    }
#elif +
	D3DXFROMWINEVECTOR3 aiVecs[2];
	D3DXFROMWINEMATRIX m;
	D3DXFROMWINEMatrixIdentity(&m);
	CalculateBounds(&aiVecs[0], &m);
	computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertices.size());
	//unsigned int slices=num_rings;
	unsigned int slices=num_segments;
            unsigned int triangleCount = num_rings * (num_segments - 1) * 2;
            unsigned int indexCount = triangleCount * 3;
        CreateIndexBuffer(vertices.size(), indexCount, slices);
#endif
	if(aShader_!=eShaderNone)

	{
        	const char* aShaderName=GetShaderName(aShader_);

		Shader = FW3ShadersFactory::GetShader(aShaderName, "main", "main");
		VertexDeclaration = FW3ShadersFactory::GetVertexFormat(eTexturedBox, Shader);
	}
	else
	{
        	Shader = -1;
		VertexDeclaration = -1;
	}
	texID=-1;
}
#endif

void MeshRenderer2::CreateOcta(tShader aShader_)
{
	m_Shape=eOcta;
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, 0, 0)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(1, 0, 0)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(1, 0, 1)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, 0, 1)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0.5f, -1, 0.5f)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0.5f, 1, 0.5f)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	setFace(0, 0, 1, 4);
	setFace(1, 1, 2, 4);
	setFace(2, 2, 3, 4);
	setFace(3, 3, 0, 4);
	setFace(4, 0, 5, 1);
	setFace(5, 1, 5, 2);
	setFace(6, 2, 5, 3);
	setFace(7, 3, 5, 0);
	if(aShader_!=eShaderNone)
	{
        	const char* aShaderName=GetShaderName(aShader_);
		Shader = FW3ShadersFactory::GetShader(aShaderName, "main", "main");
		VertexDeclaration = FW3ShadersFactory::GetVertexFormat(eTexturedBox, Shader);
	}
	else
	{
        	Shader = -1;
		VertexDeclaration = -1;
	}
	texID=-1;
}
void MeshRenderer2::CreateTetra(tShader aShader_)
{
	m_Shape=eTetra;
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, 0, 0)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(1, 1, 0)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, 1, 1)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(1, 0, 1)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	setFace(0, 0, 1, 3);
	setFace(1, 0, 2, 1);
	setFace(2, 0, 3, 2);
	setFace(3, 1, 2, 3);
	D3DXFROMWINEVECTOR3 aiVecs[2];D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);
	if(aShader_!=eShaderNone)
	{
        	const char* aShaderName=GetShaderName(aShader_);
		Shader = FW3ShadersFactory::GetShader(aShaderName, "main", "main");
		VertexDeclaration = FW3ShadersFactory::GetVertexFormat(eTexturedBox, Shader);
	}
	else
	{
        	Shader = -1;
		VertexDeclaration = -1;
	}
	texID=-1;
} 

void MeshRenderer2::CreateEllipsoid(float a, float b, float c, tShader aShader_)
        {
		m_Shape=eEllipsoid;
		int slices = (int) (4 * std::cbrt(a) + 8);
        	int stacks = (int) (4 * std::cbrt(b) + 8);
            
	    float sliceStep = 2.0f*D3DXFROMWINE_PI / slices;
            float stackStep = D3DXFROMWINE_PI / stacks;
            unsigned int vertexCount = slices * (stacks - 1) + 2;
            unsigned int triangleCount = slices * (stacks - 1) * 2;
            unsigned int indexCount = triangleCount * 3;
            std::vector<stx_VertexPositionNormalTexture> sphereVertices(vertexCount);
            unsigned int currentVertex = 0;
            vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, -c, 0), D3DXFROMWINEVECTOR3(0, -1, 0)));
            float stackAngle = D3DXFROMWINE_PI - stackStep;
            for (unsigned int i = 0; i < stacks - 1; i++)
            {
                float sliceAngle = 0;
                for (unsigned int j = 0; j < slices; j++)
                {
                    //NOTE: y and z were switched from normal spherical coordinates because the sphere is "oriented" along the Y axis as opposed to the Z axis
                    float x = (float)a*cosf(sliceAngle)*sinf(stackAngle);
                    float y = (float)c*cosf(stackAngle);
                    float z = (float)b*sinf(sliceAngle)*sinf(stackAngle);
                    D3DXFROMWINEVECTOR3 position(x, y, z);
			D3DXFROMWINEVECTOR3 normal(x, y, z);
			D3DXFROMWINEVec3Normalize(&normal, &normal);
    			float tu = asinf(normal.x)/D3DXFROMWINE_PI + 0.5;
    			float tv = -asinf(normal.y)/D3DXFROMWINE_PI - 0.5;
                   vertices.push_back(stx_VertexPositionNormalTexture(position, normal, D3DXFROMWINEVECTOR2(tu, -(tv-1.0f)), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f)));
                    sliceAngle += sliceStep;
                }
                stackAngle -= stackStep;
            }
            vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, c, 0), D3DXFROMWINEVECTOR3(0, 1, 0)));

		D3DXFROMWINEVECTOR3 aiVecs[2];D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);
	    computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertexCount);
            CreateIndexBuffer(vertexCount, indexCount, slices);
            
            

	if(aShader_!=eShaderNone)
	{
        	const char* aShaderName=GetShaderName(aShader_);
		Shader = FW3ShadersFactory::GetShader(aShaderName, "main", "main");
		VertexDeclaration = FW3ShadersFactory::GetVertexFormat(eTexturedBox, Shader);
	}
	else
	{
        	Shader = -1;
		VertexDeclaration = -1;
	}

	
	
	texID=-1;
}

//-------------------------------------------------------------------------------
// Calculate the boundaries of a given node and all of its children
// The boundaries are in Worldspace (AABB)
// piNode Input node
// p_avOut Receives the min/max boundaries. Must point to 2 D3DXFROMWINEVECTOR3s
// piMatrix Transformation matrix of the graph at this position
//-------------------------------------------------------------------------------
D3DXFROMWINEMATRIX MeshRenderer2::CalculateBounds(D3DXFROMWINEVECTOR3* p_avOut, D3DXFROMWINEMATRIX* piMatrix)
{
	p_avOut[0] = D3DXFROMWINEVECTOR3( 1e10f, 1e10f, 1e10f);
	p_avOut[1] = D3DXFROMWINEVECTOR3( -1e10f, -1e10f, -1e10f);
	for( unsigned int a = 0; a < vertices.size();++a)
	{
		D3DXFROMWINEVECTOR3 pc1 = vertices[a].Position;
		p_avOut[0].x = std::min( p_avOut[0].x, pc1.x);
		p_avOut[0].y = std::min( p_avOut[0].y, pc1.y);
		p_avOut[0].z = std::min( p_avOut[0].z, pc1.z);
		p_avOut[1].x = std::max( p_avOut[1].x, pc1.x);
		p_avOut[1].y = std::max( p_avOut[1].y, pc1.y);
		p_avOut[1].z = std::max( p_avOut[1].z, pc1.z);
	}

	D3DXFROMWINEVECTOR3 vDelta = p_avOut[1]-p_avOut[0];
	D3DXFROMWINEVECTOR3 vHalf =  p_avOut[0] + (vDelta / 2.0f);
	float fScale = 2.0f / D3DXFROMWINEVec3Length(&vDelta);

	D3DXFROMWINEMATRIX m_mWorld =  D3DXFROMWINEMATRIX(
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		-vHalf.x,-vHalf.y,-vHalf.z,1.0f) *
		D3DXFROMWINEMATRIX(
		fScale,0.0f,0.0f,0.0f,
		0.0f,fScale,0.0f,0.0f,
		0.0f,0.0f,fScale,0.0f,
		0.0f,0.0f,0.0f,1.0f);

	if(0)
	for( unsigned int j = 0; j < vertices.size();++j)
	{
		D3DXFROMWINEVec3TransformCoord(&vertices[j].Position, &vertices[j].Position, &m_mWorld);
		if(piMatrix)
			D3DXFROMWINEVec3TransformCoord(&vertices[j].Position, &vertices[j].Position, piMatrix);
	}
	if(piMatrix)
		m_mWorld*=(*piMatrix);
	return m_mWorld;
}

void MeshRenderer2::GetBounds(D3DXFROMWINEVECTOR3& ____min, D3DXFROMWINEVECTOR3& ____max)
{
	D3DXFROMWINEVECTOR3 aiVecs[2] = {D3DXFROMWINEVECTOR3( 1e10f, 1e10f, 1e10f),
		D3DXFROMWINEVECTOR3( -1e10f, -1e10f, -1e10f) };

	D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);

	____min = *(D3DXFROMWINEVECTOR3*)&aiVecs[0];
	____max = *(D3DXFROMWINEVECTOR3*)&aiVecs[1];
}
    	D3DXFROMWINEVECTOR3 MeshRenderer2::GetMeshBBoxExtents()
	{
		D3DXFROMWINEVECTOR3 __min;
		D3DXFROMWINEVECTOR3 __max;
		GetBounds(__min, __max);
		return __max;
	}
    	D3DXFROMWINEVECTOR3 MeshRenderer2::GetMeshBBoxCenter()
	{
		D3DXFROMWINEVECTOR3 __min;
		D3DXFROMWINEVECTOR3 __max;
		GetBounds(__min, __max);
		return __max/2.0f;// is this correct ???
	}
void MeshRenderer2::CreateMesh(const char* afn)
{
	m_Shape=eMesh;
	vertices.clear();
	indices.clear();
	MeshRenderer2 mesh_;
	mesh_.Load(afn, eSimpleShader, false);
	//std::vector<stx_Mesh> meshes;
	if(mesh_.meshes.size())
	{
		vertices=mesh_.meshes[0].vertices;
		indices=mesh_.meshes[0].indices;
	}
}

/*
# The L-system for the 3D-modelling of an house.

# scene
# highway
# road
# path
# yard
# house
# wall
# door
# window
# ceiling/roof
# chimney
# rain gutter

# VertexVector
# IndexVector
# TRIANGLESTRIP
# TRIANGLE
*/
/*
http://paulbourke.net/geometry/platonic/:
The measured properties of the 3 dimensional regular polyhedra
Tetrahedron

	Vertices: 4
Edges: 6
Faces: 4
Edges per face: 3
Edges per vertex: 3
Sin of angle at edge: 2 * sqrt(2) / 3
Surface area: sqrt(3) * edgelength^2
Volume: sqrt(2) / 12 * edgelength^3
Circumscribed radius: sqrt(6) / 4 * edgelength
Inscribed radius: sqrt(6) / 12 * edgelength

Coordinates

 1  1  1   -1  1 -1    1 -1 -1
-1  1 -1   -1 -1  1    1 -1 -1
 1  1  1    1 -1 -1   -1 -1  1
 1  1  1   -1 -1  1   -1  1 -1

Divide each coordinate by 2
Octahedron

	Vertices: 6
Edges: 12
Faces: 8
Edges per face:3
Edges per vertex: 4
Sin of angle at edge: 2 * sqrt(2) / 3
Surface area: 2 * sqrt(3) * edgelength^2
Volume: sqrt(2) / 3 * edgelength^3
Circumscribed radius: sqrt(2) / 2 * edgelength
Inscribed radius: sqrt(6) / 6 * edgelength

Coordinates

-a  0  a   -a  0 -a    0  b  0
-a  0 -a    a  0 -a    0  b  0
 a  0 -a    a  0  a    0  b  0
 a  0  a   -a  0  a    0  b  0
 a  0 -a   -a  0 -a    0 -b  0
-a  0 -a   -a  0  a    0 -b  0
 a  0  a    a  0 -a    0 -b  0
-a  0  a    a  0  a    0 -b  0

Where a = 1 / (2 * sqrt(2)) and b = 1 / 2
Hexahedron (cube)

	Vertices: 8
Edges: 12
Faces: 6
Edges per face: 4
Edges per vertex: 3
Sin of angle at edge: 1
Surface area: 6 * edgelength^2
Volume: edgelength^3
Circumscribed radius: sqrt(3) / 2 * edgelength
Inscribed radius: 1 / 2 * edgelength

Coordinates

-1 -1 -1    1 -1 -1    1 -1  1   -1 -1  1
-1 -1 -1   -1 -1  1   -1  1  1   -1  1 -1
-1 -1  1    1 -1  1    1  1  1   -1  1  1
-1  1 -1   -1  1  1    1  1  1    1  1 -1
 1 -1 -1    1  1 -1    1  1  1    1 -1  1
-1 -1 -1   -1  1 -1    1  1 -1    1 -1 -1

Divide each vertex by 2
Icosahedron

	Vertices: 12
Edges: 30
Faces: 20
Edges per face: 3
Edges per vertex: 5
Sin of angle at edge: 2 / 3
Surface area: 5 * sqrt(3) * edgelength^2
Volume: 5 * (3 + sqrt(5)) / 12 * edgelength^3
Circumscribed radius: sqrt(10 + 2 * sqrt(5)) / 4 * edgelength
Inscribed radius: sqrt(42 + 18 * sqrt(5)) / 12 * edgelength

Coordinates

 0  b -a    b  a  0   -b  a  0
 0  b  a   -b  a  0    b  a  0
 0  b  a    0 -b  a   -a  0  b
 0  b  a    a  0  b    0 -b  a
 0  b -a    0 -b -a    a  0 -b
 0  b -a   -a  0 -b    0 -b -a
 0 -b  a    b -a  0   -b -a  0
 0 -b -a   -b -a  0    b -a  0
-b  a  0   -a  0  b   -a  0 -b
-b -a  0   -a  0 -b   -a  0  b
 b  a  0    a  0 -b    a  0  b
 b -a  0    a  0  b    a  0 -b
 0  b  a   -a  0  b   -b  a  0
 0  b  a    b  a  0    a  0  b
 0  b -a   -b  a  0   -a  0 -b
 0  b -a    a  0 -b    b  a  0
 0 -b -a   -a  0 -b   -b -a  0
 0 -b -a    b -a  0    a  0 -b
 0 -b  a   -b -a  0   -a  0  b
 0 -b  a    a  0  b    b -a  0

Where a = 1 / 2 and b = 1 / (2 * phi)
phi is the golden ratio = (1 + sqrt(5)) / 2

Contribution by Craig Reynolds: vertices and faces for the icosahedron. Along with C++ code to create a sphere based upon the icosahedron: sphere.cpp, see also surface refinement for related ideas.
Dodecahedron

	Vertices: 20
Edges: 30
Faces: 12
Edges per face: 5
Edges per vertex: 3
Sin of angle at edge: 2 / sqrt(5)
Surface area: 3 * sqrt(25 + 10 * sqrt(5)) * edgelength^2
Volume: (15 + 7 * sqrt(5)) / 4 * edgelength^3
Circumscribed radius: (sqrt(15) + sqrt(3)) / 4 * edgelength
Inscribed radius: sqrt(250 + 110 * sqrt(5)) / 20 * edgelength

Coordinates

 c  0  1   -c  0  1   -b  b  b    0  1  c    b  b  b
-c  0  1    c  0  1    b -b  b    0 -1  c   -b -b  b
 c  0 -1   -c  0 -1   -b -b -b    0 -1 -c    b -b -b
-c  0 -1    c  0 -1    b  b -b    0  1 -c   -b  b -b
 0  1 -c    0  1  c    b  b  b    1  c  0    b  b -b
 0  1  c    0  1 -c   -b  b -b   -1  c  0   -b  b  b
 0 -1 -c    0 -1  c   -b -b  b   -1 -c  0   -b -b -b
 0 -1  c    0 -1 -c    b -b -b    1 -c  0    b -b  b
 1  c  0    1 -c  0    b -b  b    c  0  1    b  b  b
 1 -c  0    1  c  0    b  b -b    c  0 -1    b -b -b
-1  c  0   -1 -c  0   -b -b -b   -c  0 -1   -b  b -b
-1 -c  0   -1  c  0   -b  b  b   -c  0  1   -b -b  b

Where b = 1 / phi and c = 2 - phi
Divide each coordinate by 2.
*/


