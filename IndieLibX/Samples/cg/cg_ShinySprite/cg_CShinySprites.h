/****************************************************************/ /**
 **
 **
 **/ /****************************************************************/
/** \file cg_CShinySprites.h **/
#ifndef __CShinySprites__
#define __CShinySprites__

#ifdef WIN32
#  pragma warning (disable:4786)
#endif

#include <glh/glh_linear.h>
#include <shared/pbuffer.h>

#define TEX_SZ 256

/**
 ** The class container
 **/
class CSSprite
{
public:
	typedef struct
	{
		glh::vec3f Pos, Normal, Tangent;
		float MinSize, MaxSize;
	} SSprite;
private:
	string			m_msg;			///< If there are some error messages
	PBuffer 		*m_PBuffer;		///< pbuffer for this light
	bool			m_inpbuffer;

	bool m_bNoRenderTexture;
	bool binit;

	GLuint dl_bind_vtxprg;
	GLuint dl_bind_vtxprg_pbuffer;
	glh::tex_object_2D texture;
	glh::tex_object_2D	m_occlusionmap;
    bool m_bManaged;

	CGcontext Context;
	CGprogram Program;
    CGprofile Profile;

    CGparameter Projection;
    CGparameter ModelView;
    CGparameter ModelViewIT;
    CGparameter Light;
    CGparameter Shininess;
    CGparameter Slices;
    CGparameter Size;
    CGparameter CSTable;
    
public:
	CSSprite(bool managed = false);
	~CSSprite();
	const char *get_error_msg();
	void set_test_cst(GLfloat f1, GLfloat f2);
	bool init();
	void finish();
	void begin();
	void end();
	void draw_sprite(	glh::vec3f &pos, glh::vec3f &norm, 
				GLfloat MinSz, GLfloat MaxSz);
	void bind_light(GLfloat *l, GLfloat shine);	

	void begin_occlusion(bool bDebug=false);
	void end_occlusion(bool bDebug=false);

	void begin_render();
	void end_render();

	void display_occlusion_map();

    void free();
    CGcontext get_cg_context() { return Context; }
};

#endif
