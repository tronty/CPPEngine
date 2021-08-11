
struct XFile
{
	std::vector<D3DXFROMWINEVECTOR3> m_vVertices;
	std::vector<D3DXFROMWINEVECTOR3> m_vNormals;
	std::vector<D3DXFROMWINEVECTOR2> m_vTexCoords; 
	std::vector<WORD> m_wIndices;
	std::vector<stx_Material> vt;
	void readXFile(const char* aFileName)
	{
		stx_Material m(str.c_str()+1, vC);
		vt.push_back(m);
	}
	void writeXFile(const char* aFileName, stx_Texture Textures)
	{ 
		std::vector<WORD> m_wIndices2;
		for(unsigned int j=i1;j<i2;j++)
		{
			if(0==(j%3))
				IndexedMesh_.m_indices2.push_back(m_IndexedMesh.m_indices2[j/3]);
		}
		stx_Texture texture(vt, m_wIndices2);
		stx_writeXFile(aFileName, m_vVertices_, 
					  m_vnormals,
					  m_vTexCoords_, 
					  m_wIndices_, stx_Texture Textures);
	}
};
