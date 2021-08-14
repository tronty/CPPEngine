/****************************************************************/ /**
 **
 **
 **/ /****************************************************************/
/** \file cg_Cexplosion.h **/
#ifndef __Cexplosion__
#define __Cexplosion__

#include <glh/glh_linear.h>
#include <gl/glut.h>
#include <shared/data_path.h>
//#include <nv_rtmodules/modhelper.h>
#include <nv_GenericTools/nv_GenericTools.h>

#include "noise_ex.h"

#define TEX_SZ 256

/**
 ** The class container
 **/
class CNV30Effect
{
private:
    string          msg;          ///< If there are some error messages
    
    bool binit;
    /// \name Cg variables
    //@{
    CGcontext Context;
    CGprogram vertexProg;
    CGprogram fragmentProg;
    CGprofile   vertexprofile;
    CGprofile   fragmentprofile;

    CGerror Ret;
    //@}
    /// \name params for vertex program
    //@{
    CGparameter param_mat_mvp;
    CGparameter param_mat_mv;
    CGparameter param_mat_mvinv;
    CGparameter param_mat_mvit;
    CGparameter param_NoiseMatrix;
    CGparameter param_permutetable;
    CGparameter param_displacement;
    CGparameter param_scale;
    CGparameter param_FreqAmp;
    //@}
    /// \name params for fragment program
    //@{
	CGparameter param_scalebias;
    CGparameter param_Noise3D;
    CGparameter param_Ramp1D;
public:
    CNV30Effect();
    ~CNV30Effect();
	// NO "delete this" for this smart reference since we know this object is staticaly declared
	const char *get_error_msg();
    bool init(const char * vpfname = NULL, const char * fpfname = NULL);
    void finish();
    void begin(bool mode=0);
    void end();
    void drawSphere(int mode=0);
	void drawMesh();
	void AnimSubSphere(CNV30Effect *parent, float timestep);

    void loadPermuteTable(void);

    float displacement;   
    float sfadeout;        
    float noisescale;          
    float noise_rotate;   
    float ambient;        
    float freq, amp;    
    float noisematrixcache[16];    

    float objx, objy, objz;

	float ss_lifetime;
	float ss_starttime;
	float ss_al, ss_be;

    float sphscale[3];
    float sphslices;      

	float texscale, texbias;

	// for silouette case
	float moveup; 
	float silouette; 
	float silouettelength; 

    glh::vec4f *gradients;

    static glh::tex_object_3D *noisetex;
    static FilteredNoiseEx  *noise;
	glh::tex_object_1D ramptex;
};

#endif
