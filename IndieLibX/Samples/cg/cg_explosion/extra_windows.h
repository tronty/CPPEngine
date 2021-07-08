#include "nv_GenericTools/nv_GenericTools.h"

extern void do_menu( int v );
//----------------------------------------------------------------------------------
//
/// Event implementation for any optional windows from rt_modules
//
//----------------------------------------------------------------------------------
class MyEventWnd : public IEventsWnd 
{
public:
  bool rtMouseMove(IWindow *, float mx, float my, unsigned int buttons, unsigned int lval) {return false;}
  void rtDataUpdated(IWindow *, unsigned int types) {}
  bool rtWindowDestroyed(IWindow *)  {return false;}
  bool rtDestroyingWindow(IWindow *)  {return false;}
  bool rtWindowResized(IWindow *, int cx, int cy) {return false;}
  bool rtWindowRepaint(IWindow *) {return false;}
  bool rtWindowClosed(IWindow *pWin) 
  {
        pWin->Hide();
        return false; 
  };
  int rtCtrlEvent(IWindow *ctrl, const char *ctrltype, unsigned int valptr, float val, unsigned int tag1, unsigned int tag2 )
  {
    //
    // Toolbar event
	  //
    if(!strcmp(ctrltype,"toolbar"))
    {
      switch((int)tag1)
      {
      case  's': //"spherical"
      case  'm': //"Mushroom"
      case  'u': // reset view
      case  'c': //"Cg compile ('c')"
        do_menu((int)tag1);
        break;
      case  '1': //"camera on/off"
        do_menu((int)val ? '2' : '1');
        break;
      case  '3': //"tweak window"
        do_menu((int)val ? '3' : '4');
        break;
      }
      return 1;
    }
    else return 0;
  }
};
MyEventWnd myeventwnd;

static __DWORD__    ressz;
static HRSRC    HRes    = NULL;
static HGLOBAL hglobal = NULL;
static void*    resptr;


//----------------------------------------------------------------------------------
//
/// Initialize extra windows
//
//----------------------------------------------------------------------------------
void InitExtraWindows()
{
  CREATEWINDOWCONTAINER("parameters",805,1,474,800,NULL);
  SHOWWINDOW("parameters", false);
  // include the log ?
  // include the console ?

  CREATEWINDOWCONTAINER_FOLDING("compositing",-1,-1,447,59,"parameters");
    CREATECTRL_VALUE("scale", "compositing", &compositing.scale, 0, 20);

  CREATEWINDOWCONTAINER_FOLDING("bullets",-1,-1,447,59,"parameters");
    CREATECTRL_VALUE("b_scale", "bullets", &vline.bulletnoisescale , 0, 3);
    CREATECTRL_VALUE("b_amp", "bullets", &vline.bulletnoiseamp , 0, 0.5);

  CREATEWINDOWCONTAINER_FOLDING("sphere_params",-1,-1,447,59,"parameters");
    CREATECTRL_VALUE("sph_displacement", "sphere_params", &nv30effectsphere.displacement , 0, 3);
    CREATECTRL_VALUE("sph_fadeout", "sphere_params", &nv30effectsphere.sfadeout , 0, 2);
    CREATECTRL_VALUE("sph_noise_scale", "sphere_params", &nv30effectsphere.noisescale , 0, 5);
    CREATECTRL_VALUE("sph_noise_rotate", "sphere_params", &nv30effectsphere.noise_rotate , 0, 360);
    CREATECTRL_VALUE("sph_scale", "sphere_params", (float*)NULL,0,3);
      CREATEPLUG_FLOATVEC3(NULL, nv30effectsphere.sphscale);
      CONNECT_PLUG_1WAY(GETCONTROL_PLUG_VAL("sph_scale"), GET_PLUG_OF_PTR(nv30effectsphere.sphscale));

    CREATECTRL_VALUE("sph_slices", "sphere_params", &nv30effectsphere.sphslices , 0, 200);
    CREATECTRL_VALUE("sph_vert_speed", "sphere_params", &nv30effectsphere.moveup , 0, 0.5);
    CREATECTRL_VALUE("sph_silouette", "sphere_params", &nv30effectsphere.silouette , 0, 2);
    CREATECTRL_VALUE("sph_silouette_len", "sphere_params", &nv30effectsphere.silouettelength , 0, 2);
    CREATECTRL_VALUE("sph_tex_scale", "sphere_params", &nv30effectsphere.texscale , 0, 2);
    CREATECTRL_VALUE("sph_tex_bias", "sphere_params", &nv30effectsphere.texbias , 0, 2);

  CREATEWINDOWCONTAINER_FOLDING("sub_sphere_params",-1,-1,447,59,"parameters");
  // need to connect to the others...
    CREATECTRL_VALUE("sph2_displacement", "sub_sphere_params", &nv30effectsubspheres[0].displacement , 0, 3);
    CREATECTRL_VALUE("sph2_noise_scale", "sub_sphere_params", &nv30effectsubspheres[0].noisescale , 0, 5);
    CREATECTRL_VALUE("sph2_noise_rotate", "sub_sphere_params", &nv30effectsubspheres[0].noise_rotate , 0, 360);
    CREATECTRL_VALUE("sph2_slices", "sub_sphere_params", &nv30effectsubspheres[0].sphslices , 0, 200);

  CREATEWINDOWCONTAINER_FOLDING("disc_params",-1,-1,447,59,"parameters");
    CREATECTRL_VALUE("in_scale", "disc_params", &nv30effectdisc.in_scale , 0, 10);
    CREATECTRL_VALUE("out_scale", "disc_params", &nv30effectdisc.out_scale , 0, 10);
    CREATECTRL_VALUE("noise_scale", "disc_params", &nv30effectdisc.noise_scale , 0, 2);
    CREATECTRL_VALUE("noise_scale2", "disc_params", &nv30effectdisc.noise_scale2 , 0, 2);
    CREATECTRL_VALUE("disc_slices", "disc_params", &nv30effectdisc.slices , 0, 200);
    CREATECTRL_VALUE("noise_amp", "disc_params", &nv30effectdisc.noise_amp , 0, 3);
    CREATECTRL_VALUE("noise_amp2", "disc_params", &nv30effectdisc.noise_amp2, 0, 1);
    CREATECTRL_VALUE("disc_fadeout", "disc_params", &nv30effectdisc.dfadeout, 0, 1);
    CREATECTRL_VALUE("shockwave", "disc_params", &nv30effectdisc.shockwave, 0, 1);
    CREATECTRL_VALUE("shockwave", "disc_params", &nv30effectdisc.shockwave, 0, 1);

  CREATEWINDOWCONTAINER("control_menu",0,625,160,340,NULL);
    CREATETOOLBAR_V("toolbar", "control_menu", -1,-1,160,235);
    CTRL_NOTIFYME("toolbar", &myeventwnd, 0, 0);
      CREATETOOLBARITEM_CHECK("toolbar", "anim (spacebar)", &b[' '], ' ');
      CREATETOOLBARITEM("toolbar", "spherical", 's');
      CREATETOOLBARITEM("toolbar", "Mushroom", 'm');
      CREATETOOLBARITEM("toolbar", "reset view", 'u');
      CREATETOOLBARITEM("toolbar", "Cg compile ('c')", 'c');
      CREATETOOLBARITEM_CHECK("toolbar", "camera on/off", (bool*)NULL, '1');
      CREATETOOLBARITEM_CHECK("toolbar", "particles ('t')", &b['t'],'t');
      CREATETOOLBARITEM_CHECK("toolbar", "shockwave ('p')", &b['p'], 'p');
      CREATETOOLBARITEM_CHECK("toolbar", "shockwave on/off ('r')", &b['r'], 'r');
      CREATETOOLBARITEM_CHECK("toolbar", "Tweaks window ('3'/'4')", (bool*)NULL, '3');
      CREATECTRL_VALUE("timeline", "control_menu", &timeline, 0, 100);
      CREATECTRL_VALUE("timespeed", "control_menu", &timespeed, 0, 0.5);
  //
  // put in 1 windows the console and log window
  //
  CREATEWINDOWCONTAINER_SPLIT("logs",160,625,600,340,NULL);
      APPENDWINDOW("logwnd","logs");
      APPENDWINDOW("console","logs");
}
//----------------------------------------------------------------------------------
//
/// Closing extra windows
//
//----------------------------------------------------------------------------------
void CloseExtraWindows()
{
}
//#else
//void InitExtraWindows() {}
//void CloseExtraWindows() {}
//#endif
