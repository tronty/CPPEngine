#include "meshfileheader.h"

extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;

class OglMeshHelper
{
public:
	OglMeshHelper() : _mesh(NULL) {}
	~OglMeshHelper() { if(_mesh) free(_mesh); _mesh = NULL; }

	bool loadModel(LPCSTR fname)
	{
		FILE *fd;
		fd = fopen(fname, "rb");
		if(!fd)
			return false;
		int offs = 0;
		char * memory = (char*)malloc(1024);
		int n;
		while(n=fread(memory + offs, 1024, 1, fd))
		{
			offs += 1024;
			memory = (char*)realloc(memory, 1024 + offs);
		}
		fclose(fd);
		if(strncmp(memory, "MESH", 4))
		{
			free(memory);
			return false;
		}
		_mesh = (FileHeader *)memory;
		return true;
	}
	void drawMesh()
	{
		if(!_mesh) return;
#if 1
		//----> Vtx
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, _mesh->data + _mesh->offs_vtx);
		//----> Normal
		if(_mesh->offs_normal)
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, 0, _mesh->data + _mesh->offs_normal);
		}
		else
			glDisableClientState(GL_NORMAL_ARRAY);
		//----> Color
#if 0
		if(_mesh->offs_color)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_FLOAT, 0, _mesh->data + _mesh->offs_color);
		}
		else
			glDisableClientState(GL_COLOR_ARRAY);
#endif
		//----> textures...
#if 0
        glClientActiveTextureARB(GL_TEXTURE0_ARB);
		if(_mesh->offs_uv[0])
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(3, GL_FLOAT, 0, _mesh->data + _mesh->offs_uv[0]);
		}
		else
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
		//----> Tan & binorm
        /*glClientActiveTextureARB(GL_TEXTURE1_ARB);
		if(_mesh->offs_tangent)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(3, GL_FLOAT, 0, _mesh->data + _mesh->offs_tangent);
		}
		else
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glClientActiveTextureARB(GL_TEXTURE2_ARB);
		if(_mesh->offs_binormal)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(3, GL_FLOAT, 0, _mesh->data + _mesh->offs_binormal);
		}
		else
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);*/
		//...
        glClientActiveTextureARB(GL_TEXTURE0_ARB);
		//
		// Draw the full Object
		//
		glEnableClientState(GL_INDEX_ARRAY);

		//glDrawRangeElements(GL_TRIANGLES, _mesh->min_idx, _mesh->max_idx, _mesh->num_polygon * 3, 
		glDrawElements(GL_TRIANGLES, _mesh->num_indices, 
							GL_UNSIGNED_INT, _mesh->data + _mesh->offs_indices);
#else
		unsigned int *idx = (unsigned int *)(_mesh->data + _mesh->offs_indices);
		typedef float VEC[4];
		VEC *v;
		glBegin(GL_TRIANGLES);
		for(unsigned int i=0; i<_mesh->num_indices; i++)
		{
				v = (VEC *)(_mesh->data + _mesh->offs_normal + 3*idx[i]*sizeof(float));
				glNormal3fv(*v);
				v = (VEC *)(_mesh->data + _mesh->offs_color + 4*idx[i]*sizeof(float));
				//glColor4fv(*v);
				v = (VEC *)(_mesh->data + _mesh->offs_uv[0] + 3*idx[i]*sizeof(float));
				glTexCoord3fv(*v);
				v = (VEC *)(_mesh->data + _mesh->offs_vtx + 3*idx[i]*sizeof(float));
				glVertex3fv(*v);
		}
		glEnd();
#endif
	}
	FileHeader *_mesh;
};


