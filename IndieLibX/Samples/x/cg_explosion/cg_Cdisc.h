/****************************************************************/ /**
 **
 **
 **/ /****************************************************************/
/** \file cg_Cdisc.h **/
#ifndef __Cdisc__
#define __Cdisc__

#if defined(WIN32)
#  pragma warning (disable:4786)
#endif

#include <glh/glh_linear.h>

#include <glh/glh_linear.h>
#include <gl/glut.h>
#include <shared/data_path.h>
//#include <nv_rtmodules/modhelper.h>
#include <nv_GenericTools/nv_GenericTools.h>


#include "noise_ex.h"

#define TEX_SZ 256

/**
 ** The class container.
 **/
class CDisc
{
public:

private:
    bool binit;
    /// \name Cg variables
    //@{
    CGcontext Context;
    CGprogram vertexProg;
    CGprogram vertexProg2;
    CGprogram fragmentProg;
    CGprogram fragmentProg2; ///< shockwave fp
    CGprofile   vertexprofile;
    CGprofile   fragmentprofile;

    CGerror Ret;
    //@}
    /// \name params for vertex program
    //@{
    CGparameter param_mat_mvp;
    CGparameter param_mat_mv;
    /*CGparameter param_mat_mvinv;
    CGparameter param_mat_mvit;*/
    CGparameter param_NoiseMatrix;
    //@}
    /// \name params for fragment program
    //@{
    CGparameter param_NoiseAmp;
    CGparameter param_1Dtexture;
    CGparameter param_Noise3D;
    CGparameter param_perlin;
    //@}
    /// \name params for vertex program 2
    //@{
    CGparameter param_mat_proj2;
    CGparameter param_mat_mvp2;
    CGparameter param_mat_mv2;
    CGparameter param_mat_mvit2;
    CGparameter param_NoiseMatrix2;
    //@}
    /// \name params for fragment program 2 : shock wave
    //@{
    CGparameter param_Noise3D2;
    CGparameter param_NoiseAmp2;
    CGparameter param_1Dtexture2;
    //@}
public:
    CDisc();
    ~CDisc();
	// NO "delete this" for this smart reference since we know this object is staticaly declared
    const char *get_error_msg();
    bool init();
    void finish();
    void begin(bool mode=0);
    void end();
    void drawdisc_back();
    void drawdisc_front();
    void draw_shockwave();
    void playtime(float t);

    void loadPermuteTable3(void);

    float noise_translate[3];
    float noise_scale;
    float noise_scale2;
    float noise_amp;
    float noise_amp2;
	float shockwave;

    float in_scale, out_scale;
    float slices;
    float dfadeout;

    glh::tex_object_rectangle   textureperlin; ///< texture for perlin
    glh::tex_object_1D          texture1d;
    glh::tex_object_3D			noisetex;
    glh::tex_object_1D          texture1d2;
    glh::tex_object_3D			noisetex2;
    static FilteredNoiseEx			*noise;
	static CPerlinNoise				*pnoise;
};

#endif
