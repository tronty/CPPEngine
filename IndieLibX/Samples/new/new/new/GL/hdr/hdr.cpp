/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

#define GLH_EXT_SINGLE_FILE 1

#include "hdrtexture.h"
#include "blur.h"

// HDR image-based lighting example
// sgreen

using namespace glh;

#define USE_REDUCTION 0

void initGL();
void setOrthoProjection(int w, int h);
void setPerspectiveProjection(int w, int h);

HDRtexture *hdr;
GLuint hdr_tex = 0, hilo_tex = 0, hilo_tex2 = 0;
GLuint gamma_tex = 0, vignette_tex = 0;

float exposure = 1.0;

CGcontext context;

// shaders
CGprogram object_vprog;
CGprogram skybox_fprog, object_fprog;
CGprogram skybox_hilo_fprog, object_hilo_fprog;
CGprogram texrect_fprog, tonemap_fprog, shrink_fprog;
GLuint blurh_fprog = 0, blurv_fprog = 0;

CGparameter blurAmount_param, windowSize_param, exposure_param, exposure_param2;
CGparameter modelViewProj_param, model_param, modelView_param, eyePos_param;

bool toggle[256];
bool keydown[256];

int win_w = 512, win_h = 512;
int blur_scale = 2;   // divides size of blur image relative to window
int blur_w, blur_h;
float blur_width = 2.0; // width of blur kernel
float blur_amount = 0.5;

float tx = 0.0, ty = 0.0, zoom = 1.0;

glut_simple_mouse_interactor camera, object;
glut_callbacks cb;

RenderTexture *scene_buffer, *blur_buffer, *blur_buffer2;
RenderTexture *reduce_buffer[2];
RenderTexture *final_reduce_buffer;
bool fp_filtering = false;  // does the hardware support fp16 texture filtering?

char *image_filename = "hdr/stpeters_cross.hdr";
NvIOModel *model = 0;
char *model_filename = "../../../../MEDIA/models/obj/venusm.obj";
int modelno = 0;
bool fullscreen = 0;

void initBlurCode(float blur_width);

void checkErrors(char *s)
{
  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR) {
    fprintf(stderr, "%s: error - %s\n", s, (char *) gluErrorString(error));
  }
}

struct App
{
	App(){}
	virtual ~App(){}

// draw a quad with texture coordinate for texture rectangle
void drawQuad(int w, int h, float tw, float th, bool offset=false)
{
  float blur_tw, blur_th;
  if (fp_filtering) {
    tw = 1.0;
    th = 1.0;
    blur_tw = 1.0;
    blur_th = 1.0;
  } else {
    blur_tw = tw / blur_scale;
    blur_th = th / blur_scale;
  }
  float ox = 0.0, oy = 0.0;
  if (offset) {
    // offset by half a texel
    ox = 0.5 / w;
    oy = 0.5 / h;
  }
  glBegin(GL_QUADS);
  glTexCoord2f(0+ox, th+oy);  glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, blur_th);
  glVertex3f(0, h, 0);
  glTexCoord2f(tw+ox, th+oy); glMultiTexCoord2fARB(GL_TEXTURE1_ARB, blur_tw, blur_th);
  glVertex3f(w, h, 0);
  glTexCoord2f(tw+ox, 0+oy);  glMultiTexCoord2fARB(GL_TEXTURE1_ARB, blur_tw, 0);
  glVertex3f(w, 0, 0);
  glTexCoord2f(0+ox, 0+oy);   glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, 0);
  glVertex3f(0, 0, 0);
  glEnd();
}

// as above, with inverted texcoords
void drawQuadInv(int w, int h, int tw, int th)
{
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(0, h, 0);
  glTexCoord2f(tw, 0); glVertex3f(w, h, 0);
  glTexCoord2f(tw, th); glVertex3f(w, 0, 0);
  glTexCoord2f(0, th); glVertex3f(0, 0, 0);
  glEnd();
}


void disableTexGen()
{
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN);
  glDisable(GL_TEXTURE_GEN_R);
}

void enableTexGen()
{
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN);
  glEnable(GL_TEXTURE_GEN_R);
}

void initObjectLinearTexGen()
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  GLfloat s_plane[] = { 1.0, 0.0, 0.0, 0.0 };
  GLfloat t_plane[] = { 0.0, 1.0, 0.0, 0.0 };
  GLfloat r_plane[] = { 0.0, 0.0, 1.0, 0.0 };
  glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane);
  glTexGenfv(GL, GL_OBJECT_PLANE, t_plane);
  glTexGenfv(GL_R, GL_OBJECT_PLANE, r_plane);
  glPopMatrix();
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  glTexGeni(GL, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
}

void setFilter()
{
  if (toggle['l']) {
    glTexParameteri( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else {
    glTexParameteri( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
//    glTexParameteri( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
}

void bindTextures()
{
  if (toggle['h']) {
    // hilo mode
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, hilo_tex);
    setFilter();
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, hilo_tex2);
    setFilter();
  } else {
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, hdr_tex);
    setFilter();
  }
}

// draw cubemap background
void drawSkyBox()
{
  if (toggle['h']) {
    cgGLBindProgram(skybox_hilo_fprog);
  } else {
    cgGLBindProgram(skybox_fprog);
  }
  cgGLEnableProfile(CG_PROFILE_FP30);
  glDisable(GL_DEPTH_TEST);

  // bind textures
  bindTextures();

  // initialize object linear texgen
  glActiveTextureARB(GL_TEXTURE0_ARB);
  initObjectLinearTexGen();
  enableTexGen();

  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  glLoadIdentity();
  camera.trackball.apply_inverse_transform();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
    glutSolidCube(1.0);
  glPopMatrix();

  glMatrixMode(GL_TEXTURE);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  disableTexGen();
  glEnable(GL_DEPTH_TEST);
  cgGLDisableProfile(CG_PROFILE_FP30);
}


// read from float texture, apply tone mapping, render to regular 8/8/8 display
void toneMappingPass()
{
  scene_buffer->Deactivate();

  // bind textures
  glActiveTextureARB(GL_TEXTURE0_ARB);
  scene_buffer->Bind();

  glActiveTextureARB(GL_TEXTURE1_ARB);
  blur_buffer2->Bind();

  glActiveTextureARB(GL_TEXTURE2_ARB);
  glBindTexture(GL_TEXTURE_1D, gamma_tex);

  glActiveTextureARB(GL_TEXTURE3_ARB);
  glBindTexture(fp_filtering ? GL_TEXTURE_2D : GL_TEXTURE_RECTANGLE_NV, vignette_tex);

#if USE_REDUCTION
  glActiveTextureARB(GL_TEXTURE4_ARB);
  final_reduce_buffer->Bind();
#endif

  cgGLBindProgram(tonemap_fprog);
  cgGLEnableProfile(CG_PROFILE_FP30);

  cgGLSetParameter1f(blurAmount_param, blur_amount);
  cgGLSetParameter4f(windowSize_param, 2.0/win_w, 2.0/win_h, -1.0, -1.0);
  if (toggle['h']) {
    // compensate for hilo remapping
    cgGLSetParameter1f(exposure_param, exposure * hdr->m_max );
  } else {
    cgGLSetParameter1f(exposure_param, exposure);
  }

  glDisable(GL_DEPTH_TEST);
  drawQuad(win_w, win_h, scene_buffer->GetWidth(), scene_buffer->GetHeight());

  cgGLDisableProfile(CG_PROFILE_FP30);
  scene_buffer->Release();
  blur_buffer2->Release();
#if USE_REDUCTION
  final_reduce_buffer->Release();
#endif

  glutSwapBuffers();
  checkErrors("tonemap");
}

void darkenScreen()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0, 0.0, 0.0, 0.1);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, win_w, 0.0, win_h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    drawQuad(win_w, win_h, win_w, win_h);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

// render scene to float pbuffer
void renderScene()
{
  if (toggle['M']) {
    glClear(GL_DEPTH_BUFFER_BIT);
    darkenScreen();
  } else {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  if (toggle['w'])
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  if (toggle['a']) {
    glEnable(GL_MULTISAMPLE_ARB);
    glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
  } else {
    glDisable(GL_MULTISAMPLE_ARB);
  }
  
  if (toggle['s']) {
    // draw background
    drawSkyBox();
  }

  // draw object
  cgGLBindProgram(object_vprog);
  cgGLEnableProfile(CG_PROFILE_VP30);

  if (toggle['h']) {
    cgGLBindProgram(object_hilo_fprog);
  } else {
    cgGLBindProgram(object_fprog);
  }
  cgGLEnableProfile(CG_PROFILE_FP30);

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  camera.trackball.apply_inverse_transform();

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  camera.apply_transform();
  object.apply_transform();

  cgGLSetStateMatrixParameter(modelViewProj_param, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
  cgGLSetMatrixParameterfc(model_param, object.get_transform().get_value());
//  cgGLSetStateMatrixParameter(modelView_param, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);

  // calculate eye position in cubemap space
  vec3f eyePos_eye(0.0, 0.0, 0.0), eyePos_model;
  matrix4f view = camera.get_transform();
  view.inverse().mult_matrix_vec(eyePos_eye, eyePos_model);
  cgGLSetParameter3f(eyePos_param, eyePos_model[0], eyePos_model[1], eyePos_model[2]);
//  cgGLSetParameter3f(eyePos_param, 0.0, 0.0, 0.0);

  bindTextures();

  glEnable(GL_CULL_FACE);

  if (toggle['b']) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
  } else {
    glEnable(GL_DEPTH_TEST);
  }

  if (model) {
    model->Render();
  } else {
    switch(modelno) {
    case 0:
      glutSolidSphere(1.0, 40, 40);
      break;
    case 1:
      glutSolidTetrahedron();
      break;
    case 2:
      glutSolidCube(1.0);
      break;
    case 3:
      break;
    }
  }

  glDisable(GL_BLEND);
  glDisable(GL_CULL_FACE);
  cgGLDisableProfile(CG_PROFILE_FP30);
  cgGLDisableProfile(CG_PROFILE_VP30);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  checkErrors("renderScene");
}

void renderImage()
{
  if (toggle['h']) {
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, hilo_tex);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, hilo_tex2);
  } else {
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, hdr_tex);
  }

  cgGLBindProgram(texrect_fprog);
  cgGLEnableProfile(CG_PROFILE_FP30);
  cgGLSetParameter1f(exposure_param2, exposure);

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  drawQuadInv(hdr->GetWidth(), hdr->GetHeight(), hdr->GetWidth(), hdr->GetHeight());

  cgGLDisableProfile(CG_PROFILE_FP30);
}

// blur floating point image
void glowPass()
{
  glEnable(GL_FRAGMENT_PROGRAM_NV);
  glDisable(GL_DEPTH_TEST);
  drawQuad(blur_w, blur_h, blur_w, blur_h);
  glDisable(GL_FRAGMENT_PROGRAM_NV);
}

void glow()
{
    // shrink image (pbuffer -> blur_buffer2)
    blur_buffer2->Activate();
    cgGLBindProgram(shrink_fprog);
    cgGLEnableProfile(CG_PROFILE_FP30);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    scene_buffer->Bind();
    drawQuad(blur_w, blur_h, scene_buffer->GetWidth(), scene_buffer->GetHeight());
    scene_buffer->Release();
    cgGLDisableProfile(CG_PROFILE_FP30);
    blur_buffer2->Deactivate();

#if 1
    // horizontal blur (blur_buffer2 -> blur_buffer)
    blur_buffer->Activate();
    glBindProgramNV(GL_FRAGMENT_PROGRAM_NV, blurh_fprog);
    glEnable(GL_FRAGMENT_PROGRAM_NV);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    blur_buffer2->Bind();
    glowPass();
    blur_buffer2->Release();
    glDisable(GL_FRAGMENT_PROGRAM_NV);
    blur_buffer->Deactivate();

    // vertical blur (blur_buffer -> blur_buffer2)
    blur_buffer2->Activate();
    glBindProgramNV(GL_FRAGMENT_PROGRAM_NV, blurv_fprog);
    glEnable(GL_FRAGMENT_PROGRAM_NV);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    blur_buffer->Bind();
    glowPass();
    blur_buffer->Release();
    glDisable(GL_FRAGMENT_PROGRAM_NV);
    blur_buffer2->Deactivate();
#endif
}

int log2(int x)
{
    if (x == 0) return -1;
    int i = 0;
    for (;;) {
		  if (x & 1) {
			  if (x != 1) return -1; // error
			  return i;
		  }
		  x = x >> 1;
		  i++;
    }
}

// calculate average color of scene image by repeatedly halving size (like generating mipmaps)
// assumes square image
// returns pointer to final destination buffer
RenderTexture *
reduce()
{
	RenderTexture *src_buffer = scene_buffer, *dest_buffer = reduce_buffer[0];

	int size = scene_buffer->GetWidth();
	int levels = log2(size);
//	LOG_PRINT("size: %d levels %: %d\n", size, levels);

	for(int i=0; i<levels; i++) {
		dest_buffer->Activate();
    glClear(GL_COLOR_BUFFER_BIT);
		cgGLBindProgram(shrink_fprog);
		cgGLEnableProfile(CG_PROFILE_FP30);
		glActiveTextureARB(GL_TEXTURE0_ARB);
		src_buffer->Bind();
		drawQuad(size/2, size/2, size, size);
		src_buffer->Release();
		dest_buffer->Deactivate();
//    LOG_PRINT("level %d: %d\n", i, size);

    // switch source and destination buffers
		if (i==0) {
			src_buffer = reduce_buffer[0];
			dest_buffer = reduce_buffer[1];
		} else {
			RenderTexture *temp = src_buffer;
			src_buffer = dest_buffer;
			dest_buffer = temp;
		}
		size /= 2;
	}

	cgGLDisableProfile(CG_PROFILE_FP30);
	return src_buffer;
}

void display()
{	
  scene_buffer->Activate();
  renderScene();

#if USE_REDUCTION
  // reduction pass
  if (toggle['r']) {
	  final_reduce_buffer = reduce();
  }
#endif

  // blur pass
  if (toggle['g']) {
    glow();
  }
  // tone mapping
  toneMappingPass();

  checkErrors("display");
}

void testKeys()
{
  if (keydown[']']) {
    exposure *= 1.01;
	glutPostRedisplay();
  }
  if (keydown['[']) {
    exposure /= 1.01;
	glutPostRedisplay();
  }
}

void idle()
{
  static float last_time;
  float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
  float dt = time - last_time;
  last_time = time;

  if (toggle[' ']) {
    float angle_step = (GLH_PI*2.0 / 360.0) * 30.0;
    rotationf increment;
    increment.set_value(vec3f(0.0, 1.0, 0), dt*angle_step);
	camera.trackball.r *= increment;

    glutPostRedisplay();
  }

  testKeys();
}

void key(unsigned char key, int x, int y)
{
  toggle[key] ^= 1;

  switch (key) {
  case '\033':
  case 'q':
    if (fullscreen)
      glutLeaveGameMode();
	exit(0);
	break;

  case 'o':
    object.enable();
    camera.disable();
    break;

  case 'c':
    camera.enable();
    object.disable();
    break;

  case '=':
    exposure*=2;
    break;
  case '-':
    exposure*=0.5;
    break;

  case '+':
    exposure+=1.0;
    break;
  case '_':
    exposure-=1.0;
    break;

  case '.':
    if (blur_amount < 1.0) blur_amount += 0.1;
    break;
  case ',':
    blur_amount -= 0.1;
    break;

  case '>':
    blur_width += 0.1;
    initBlurCode(blur_width);
    break;
  case '<':
    if (blur_width > 1.0)
      blur_width -= 0.1;
    else
      blur_width = 1.0;
    initBlurCode(blur_width);
    break;
  case 'L':
    initBlurCode(blur_width);
    break;

  case 'g':
    if (toggle['g'])
      blur_amount = 0.5;
    else
      blur_amount = 0.0;
    break;

  case 'm':
    modelno = (modelno + 1) % 4;
    break;

  default:
    keydown[key] = 1;
    break;
  }
  LOG_PRINT("exposure = %f, blur = %f, blur_sigma = %f\n", exposure, blur_amount, blur_width);
  glutPostRedisplay();
}


void keyUp(unsigned char key, int x, int y)
{
  keydown[key] = 0;
}

void setOrthoProjection(int w, int h)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, 0, h, -1.0, 1.0);
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(0, 0, w, h);
}

void setPerspectiveProjection(int w, int h)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (float) w / (float) h, 0.1, 10.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(0, 0, w, h);
}


void reshape(int w, int h)
{
  win_w = w; win_h = h;

  scene_buffer->Deactivate();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, win_w, 0, win_h, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glViewport(0, 0, win_w, win_h);
}

GLuint loadProgram(GLenum target, const GLubyte *code)
{
    GLuint prog_id;
    glGenProgramsNV(1, &prog_id);
    glBindProgramNV(target, prog_id);
    size_t size = strlen((const char *) code);
    glLoadProgramNV(target, prog_id, (GLsizei) size, code);
    GLint errpos;
    glGetIntegerv(GL_PROGRAM_ERROR_POSITION_NV, &errpos);
    if(errpos != -1) {
      fprintf(stderr, "program error:\n");
      int bgn = errpos - 10;
      bgn = bgn < 0 ? 0 : bgn;
      const char * c = (const char *)(code + bgn);
      for(int i = 0; i < 30; i++)
      {
        if(bgn+i >= int(size-1))
          break;
        fprintf(stderr, "%c", *c++);
      }
      fprintf(stderr, "\n");
    }
    return prog_id;
}


// create gamma lookup table texture
GLuint createGammaTexture(int size, float gamma)
{
  GLuint texid;
  glGenTextures(1, &texid);

  GLenum target = GL_TEXTURE_1D;
  glBindTexture(target, texid);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  float *img = new float [size];
  if (!img) return 0;

  for(int i=0; i<size; i++) {
    float x = i / (float) size;
    img[i] = pow(x, gamma);
  }

  glTexImage1D(target, 0, GL_LUMINANCE, size, 0, GL_LUMINANCE, GL_FLOAT, img);
  delete [] img;

  return texid;
}

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

// create vignette texture
// based on Debevec's pflare.c
GLuint createVignetteTexture(GLenum target, int xsiz, int ysiz, float r0, float r1)
{
  GLuint texid;
  glGenTextures(1, &texid);
  glBindTexture(target, texid);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(target, GL_TEXTURE_WRAP, GL_CLAMP_TO_EDGE);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  float *img = new float [xsiz*ysiz];
  if (!img) return 0;

  for (int y = 0; y < ysiz; y++) {
    for (int x = 0; x < xsiz; x++) {
      float radius = sqrtf((x-xsiz/2)*(x-xsiz/2) + (y-ysiz/2)*(y-ysiz/2));
      if (radius > r0) {
	    if (radius < r1) {
	      float t = 1.0 - (radius-r0)/(r1-r0);
	      float a = t * 2 - 1;
	      float reduce = (0.25 * M_PI + 0.5 * asin(a) + 0.5 * a * sqrt( 1 - a*a ))/(0.5 * GLH_PI);
	      img[y*xsiz + x] = reduce;
	    } else {
	      img[y*xsiz + x] = 0.0;
	    }
      } else {
        img[y*xsiz + x] = 1.0;
      }
    }
  }

  glTexImage2D(target, 0, GL_LUMINANCE, xsiz, ysiz, 0, GL_LUMINANCE, GL_FLOAT, img);
  delete [] img;

  return texid;
}

void initCg()
{
  path.path.push_back("../../../MEDIA/programs");
  path.path.push_back("../../../../MEDIA/programs");

  if (fp_filtering) {
    shrink_fprog = loadProgram(context, "hdr/shrink_2D.cg", CG_PROFILE_FP30, path);
    tonemap_fprog = loadProgram(context, "hdr/tonemap_2D.cg", CG_PROFILE_FP30, path);
  } else {
    shrink_fprog = loadProgram(context, "hdr/shrink.cg", CG_PROFILE_FP30, path);
    tonemap_fprog = loadProgram(context, "hdr/tonemap.cg", CG_PROFILE_FP30, path);
  }
  blurAmount_param = cgGetNamedParameter(tonemap_fprog, "blurAmount");
  windowSize_param = cgGetNamedParameter(tonemap_fprog, "windowSize");
  exposure_param   = cgGetNamedParameter(tonemap_fprog, "exposure");

  texrect_fprog = loadProgram(context, "hdr/texrect.cg", CG_PROFILE_FP30, path);
  exposure_param2   = cgGetNamedParameter(texrect_fprog, "exposure");

  object_vprog = loadProgram(context, "hdr/object_vp.cg", CG_PROFILE_VP30, path);
  modelViewProj_param = cgGetNamedParameter(object_vprog, "modelViewProj");
  model_param         = cgGetNamedParameter(object_vprog, "model");
  modelView_param     = cgGetNamedParameter(object_vprog, "modelView");
  eyePos_param        = cgGetNamedParameter(object_vprog, "eyePos");

  object_fprog = loadProgram(context, "hdr/object.cg", CG_PROFILE_FP30, path);
  object_hilo_fprog = loadProgram(context, "hdr/object_hilo.cg", CG_PROFILE_FP30, path);

  skybox_fprog = loadProgram(context, "hdr/skybox.cg", CG_PROFILE_FP30, path);
  skybox_hilo_fprog = loadProgram(context, "hdr/skybox_hilo.cg", CG_PROFILE_FP30, path);
}

void initGL()
{
  if (glutExtensionSupported("GL_ATI_texture_float")) {
    // hardware supports fp16 texture filtering
    fp_filtering = true;
    toggle['h'] = false; // don't use HILO
  }

  setOrthoProjection(win_w, win_h);
  gamma_tex = createGammaTexture(256, 1.0 / 2.2);
  vignette_tex = createVignetteTexture(fp_filtering ? GL_TEXTURE_2D : GL_TEXTURE_RECTANGLE_NV, win_w, win_h, 0.25*win_w, 0.7*win_w);

  // create float pbuffers
  char *fp_mode, *fp_mode_nodepth;
  GLenum fp_target;
  if (fp_filtering) {
    // use ATI float texture formats and 2D textures
    fp_mode = "ati_float=16 rgba depth texture2D";
    fp_mode_nodepth = "ati_float=16 rgba texture2D";
    fp_target = GL_TEXTURE_2D;
  } else {
    // use NV float texture formats and texture rectangles
    fp_mode = "float=16 rgba depth textureRECT";
    fp_mode_nodepth = "float=16 rgba textureRECT";
    fp_target = GL_TEXTURE_RECTANGLE_NV;
  }

  scene_buffer = new RenderTexture(fp_mode, win_w, win_h, fp_target);
  if (fp_filtering) {
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }
  scene_buffer->Activate();
  glEnable(GL_DEPTH_TEST);
  setPerspectiveProjection(win_w, win_h);
  scene_buffer->Deactivate();

  // blur pbuffers
  blur_buffer = new RenderTexture(fp_mode_nodepth, blur_w, blur_h, fp_target);
  if (fp_filtering) {
      blur_buffer->Bind();
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      blur_buffer->Release();
  }
  blur_buffer->Activate();
  setOrthoProjection(blur_w, blur_h);

  blur_buffer2 = new RenderTexture(fp_mode_nodepth, blur_w, blur_h, fp_target);
  if (fp_filtering) {
      blur_buffer2->Bind();
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      blur_buffer2->Release();
  }
  blur_buffer2->Activate();
  setOrthoProjection(blur_w, blur_h);

#if USE_REDUCTION
  // pbuffers for reduction operation
  for(int i=0; i<2; i++) {
	  reduce_buffer[i] = new RenderTexture(fp_mode_nodepth, win_w/2, win_h/2, fp_target);
	  reduce_buffer[i]->Activate();
    glClear(GL_COLOR_BUFFER_BIT);
	  setOrthoProjection(win_w/2, win_h/2);
  }
 final_reduce_buffer = reduce_buffer[0];
#endif

  scene_buffer->Activate();

  initCg();

  checkErrors("initGL");
}

void initBlurCode(float blur_width)
{
  // delete old programs
  if (blurh_fprog) {
      glDeleteProgramsNV(1, &blurh_fprog);
  }
  if (blurh_fprog) {
      glDeleteProgramsNV(1, &blurh_fprog);
  }

  // generate weights for gaussian blur
  float *weights;
  int width;
  weights = generateGaussianWeights(blur_width, width);

  // generate blur code
  const unsigned char *blurCodeH, *blurCodeV;
  if (fp_filtering && !toggle['L']) {
    blurCodeH = generate1DConvolutionFP_filter(weights, width, false, fp_filtering, blur_w, blur_h);
    blurCodeV = generate1DConvolutionFP_filter(weights, width, true, fp_filtering, blur_w, blur_h);
  } else {
    blurCodeH = generate1DConvolutionFP(weights, width, false, fp_filtering, blur_w, blur_h);
    blurCodeV = generate1DConvolutionFP(weights, width, true, fp_filtering, blur_w, blur_h);
  }
//  LOG_PRINT("%s\n", blurCodeH);
//  LOG_PRINT("%s\n", blurCodeV);

  blurh_fprog = loadProgram(GL_FRAGMENT_PROGRAM_NV, blurCodeH);
  blurv_fprog = loadProgram(GL_FRAGMENT_PROGRAM_NV, blurCodeV);

  delete [] blurCodeH;
  delete [] blurCodeV;
  delete [] weights;
}


// get extension pointers
void getExts()
{
  if(! glh_init_extensions(
    "GL_ARB_multitexture "
    "GL_NV_vertex_program "
    "GL_NV_fragment_program "
    "WGL_ARB_pbuffer "
    "WGL_ARB_pixel_format "
    "WGL_ARB_render_texture "
    ))	{
    fprintf(stderr, "Error - required extensions were not supported: %s", glh_get_unsupported_extensions());
  }
    fprintf(stderr, "Extensions: %s\n", glGetString(GL_EXTENSIONS) );
}

void mainMenu(int i)
{
  key((unsigned char) i, 0, 0);
}

void initMenus(void)
{
  glutCreateMenu(mainMenu);
  glutAddMenuEntry("Toggle animation [ ]", ' ');
  glutAddMenuEntry("Toggle glow [g]", 'g');
  glutAddMenuEntry("Toggle wireframe [w]", 'w');
  glutAddMenuEntry("Toggle hilo / fp16 texture [h]", 'h');
  glutAddMenuEntry("Double exposure [+]]", '+');
  glutAddMenuEntry("Half exposure [-]", '-');
  glutAddMenuEntry("Increment exposure []]", ']');
  glutAddMenuEntry("Decrement exposure [[]", '[');
  glutAddMenuEntry("Increase blur amount [.]", '.');
  glutAddMenuEntry("Decrease blur amount [,]", ',');
  glutAddMenuEntry("Increase blur width [>]", '>');
  glutAddMenuEntry("Decrease blur width [<]", '<');
  glutAddMenuEntry("Control camera [c]", 'c');
  glutAddMenuEntry("Control object [o]", 'o');
  glutAddMenuEntry("Switch model [m]", 'm');
  glutAddMenuEntry("Quit (esc)", '\033');
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void parseArgs(int argc, char **argv)
{
  for(int i=1; i<argc; i++) {
    if (strcmp("-image", argv[i])==0) {
      image_filename = argv[++i];

    } else if (strcmp("-window", argv[i])==0) {
      win_w = atoi(argv[++i]);
      win_h = atoi(argv[++i]);

    } else if (strcmp("-exposure", argv[i])==0) {
      exposure = atof(argv[++i]);

    } else if (strcmp("-blurwidth", argv[i])==0) {
      blur_width = atoi(argv[++i]);

    } else if (strcmp("-blurscale", argv[i])==0) {
      blur_scale = atoi(argv[++i]);

    } else if (strcmp("-model", argv[i])==0) {
      model_filename = argv[++i];

    } else if (strcmp("-fullscreen", argv[i])==0) {
      fullscreen = true;

    } else {
      fprintf(stderr, "Unrecognized option '%s'\n", argv[i]);
      LOG_PRINT("usage: %s [-image image.hdr] [-model model.obj] [-window width height] [-exposure f] [-blurwidth f] [-blurscale d]\n", argv[0]);
    }
  }
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  parseArgs(argc, argv);

  blur_w = win_w / blur_scale;
  blur_h = win_h / blur_scale;

  if (model_filename) {
    model = new NvIOModel();
    if (model->ReadOBJ(model_filename)) {
      model->Rescale();
      object.pan.pan = vec3f(0.0, 0.0, 0.0);
      camera.dolly.dolly[2] = -1.5;
    } else {
      fprintf(stderr, "Error loading model '%s'\n", model_filename);
      delete model;
      model = 0;
      object.pan.pan = vec3f(0.0, 0.0, 0.0);
      camera.dolly.dolly[2] = -3.0;
    }
  }

  char displayString[256];
  int samples = 4;
  sprintf(displayString, "double rgb~8 depth~16 samples~%d", samples);

  if (fullscreen) {
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  //  glutInitDisplayString(displayString);
    char gamemode[256];
    sprintf(gamemode, "%dx%d:%d", win_w, win_h, 32);
    glutGameModeString(gamemode);
    int win = glutEnterGameMode();

  } else {
    // windowed
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
//    glutInitDisplayString(displayString);
  
    glutInitWindowSize(win_w, win_h);
    (void) glutCreateWindow("HDR");
  }

  glut_helpers_initialize();
  cb.keyboard_function = key;
  cb.idle_function = idle;
  cb.reshape_function = reshape;

  camera.configure_buttons(1);
  camera.set_camera_mode(false);
  camera.trackball.r = rotationf(vec3f(1, 0, 0), -20.0 * GLH_PI / 180.0);

  object.dolly.dolly = vec3f(0.0, 0.0, 0.0);
  object.trackball.r = rotationf(vec3f(1, 0, 0), 0.0);
  object.configure_buttons(1);

  camera.enable();
  object.disable();

  glut_add_interactor(&cb);
  glut_add_interactor(&camera);
  glut_add_interactor(&object);

  glutDisplayFunc(display);
  glutIdleFunc(idle);
  glutKeyboardUpFunc(keyUp);
//  glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
//  glutIgnoreKeyRepeat(1);
  initMenus();

  toggle['f'] = true; // fragment programs
  toggle['g'] = true; // glow
  toggle['l'] = true; // filtering
  toggle[' '] = true; // animation
  toggle['s'] = true; // skybox
  toggle['h'] = true; // use hilo

  hdr = new HDRtexture(image_filename);
  if (!hdr->IsValid()) {
    fprintf(stderr, "Error loading hdr file '%s'\n", image_filename);
    quitapp(-1);
  }
  hdr->Analyze();

  // rgbe format
//  hdr_tex = hdr->CreateCubemapRGBE();

  // real float format
//    hdr_tex = hdr->CreateCubemap(GL_RGB);
  hdr_tex = hdr->CreateCubemap(GL_RGB_FLOAT16_ATI);

  // hilo format
  hdr->ConvertHILO();
  hilo_tex = hdr->CreateCubemapHILO(true);
  hilo_tex2 = hdr->CreateCubemapHILO(false);

  getExts();
  initGL();
  initBlurCode(blur_width);

  glutMainLoop();
  return 0;
}

	void render()
	{
	}	
	int init(const char* aTitle)
	{
		return 0;
	}
};
App app;
int init(const char* aTitle)
{	
	return 0;
}

void render()
{
}
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("hdr");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		IRenderer::GetRendererInstance()->BeginScene();
		render();
		IRenderer::GetRendererInstance()->EndScene();
		IRenderer::GetRendererInstance()->Present( );
	}
	return 0;
}

