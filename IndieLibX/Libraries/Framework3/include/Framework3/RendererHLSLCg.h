#ifndef __RendererHLSLCg3_H__
#define __RendererHLSLCg3_H__

#if 1
#define STX_FNLN
#define STX_PRINT
#define LOG_FNLN
#define LOG_PRINT
#endif

#ifndef SWIG
//define MAX_BUF_SIZE 65536
#define checkForCgError
#if 1//def _WIN32
#if 1//VISTA_AND_WINDOWS7

#define CAMERA_FOVY  60.0f
#define CAMERA_ZFAR  10.0f
#define CAMERA_ZNEAR 0.1f
#define MOUSE_ORBIT_SPEED 0.30f     // 0 = SLOWEST, 1 = FASTEST
#define MOUSE_DOLLY_SPEED 0.22f     // same as above...but much more sensitive
#define MOUSE_TRACK_SPEED 0.055f    // same as above...but much more sensitive
#define CUBE_SIZE	1.0f


/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*
*  |_|	   |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|	*
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _	*
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_	*
*  |_|	   |_| |_|_ _ _|_| |_|	   |_| |_|_ _ _|_|  _ _ _ _|_|	*
*  |_|	   |_|	 |_|_|_|   |_|	   |_|	 |_|_|_|   |_|_|_|_|	*
*								*
*		      http://www.humus.name			*
*								 *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.		   *
*								 *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#define R2VB 0x1000
#define DMAP 0x1000
#define AUTOGENMIPMAP 0x2000
#define RT_FRAMEBUFFER (-1) /**< Default render target handle; refers to framebuffer's colorbuffer (can also refer to framebuffer's depthstencil surface). */
#define RT_USERDEFINED (-3)
#define RT_NONE      (-2)   /**< 0 render target. */
#define MAX_MRTS 8
#define MAX_VERTEXSTREAM 8
#define MAX_TEXTUREUNIT  16
#define MAX_SAMPLERSTATE 16
#define VB_INVALID (-2)

#if 0
extern char *texSHD;
extern const char *texVS;
extern const char *texPS;
#endif
typedef enum _RSRSENUMERATION
{
	RSERS_START					= 0,
	RSERS_LIGHTING				= 0,
	RSERS_LIGHT0				= 1,
	RSERS_LIGHT1				= 2,
	RSERS_LIGHT2				= 3,
	RSERS_LIGHT3				= 4,
	RSERS_LIGHT4				= 5,
	RSERS_LIGHT5				= 6,
	RSERS_LIGHT6				= 7,
	RSERS_LIGHT7				= 8,
	RSERS_NORMALARRAY			= 9,
	RSERS_VERTEXARRAY			= 10,
	RSERS_TEXTURECOORDARRAY 	= 11,
	RSERS_Z 					= 12,
	RSERS_DEPTHTEST 			= 12,
	RSERS_BLEND					= 13,
	RSERS_COLORMATERIAL			= 14,
	RSERS_TEXTURE2D 			= 15,
	RSERS_CULLING				= 16,
	RSERS_POLYGONSMOOTH			= 17,
	RSERS_CLIP_PLANE0			= 18,
	RSERS_FOG					= 19,
	RSERS_SHADEMODEL			= 20,
	RSERS_DEPTHFUNC 			= 21,
	RSERS_CULLFACEMODE			= 22,
	RSERS_FRONTFACE 			= 23,
	RSERS_NORMALIZE 			= 24,
	RSERS_FILLMODE				= 25,
	RSERS_POINT_SIZE			= 26,
	RSERS_LINE_WIDTH			= 27,
	RSERS_ENDPLUSONE			= 28,
	RSERS_ZWRITEENABLE			= 29,
	RSERS_COLORWRITEENABLE		= 30,
	RSERS_SRCBLEND				= 31,
	RSERS_DESTBLEND 			= 32,
	RSERS_BLENDOP				= 33
} RSRS_ENUMERATION;
typedef enum __FILLMODE
{
  RSFILL_WIREFRAME = 0x0000,
  RSFILL_SOLID = 0x0001
  } RSE_FILLMODE;
typedef enum _RSGLENUMERATION
{
	RS_EXT_abgr	=1,
	RS_EXT_bgra	=1,
	RS_EXT_packed_pixels	=1,
	RS_EXT_paletted_texture =1,
	RS_EXT_vertex_array	=1,
	RS_SGI_compiled_vertex_array	=1,
	RS_SGI_cull_vertex	=1,
	RS_SGI_index_array_formats	=1,
	RS_SGI_index_func	=1,
	RS_SGI_index_material	=1,
	RS_SGI_index_texture	=1,
	RS_WIN_swap_hint	=1,
	RS_CURRENT_BIT	=0x00000001,
	RS_POINT_BIT	=0x00000002,
	RS_LINE_BIT	=0x00000004,
	RS_POLYGON_BIT	=0x00000008,
	RS_POLYGON_STIPPLE_BIT	=0x00000010,
	RS_PIXEL_MODE_BIT	=0x00000020,
	RS_LIGHTING_BIT =0x00000040,
	RS_FOG_BIT	=0x00000080,
	RS_DEPTH_BUFFER_BIT	=0x00000100,
	RS_ACCUM_BUFFER_BIT	=0x00000200,
	RS_STENCIL_BUFFER_BIT	=0x00000400,
	RS_VIEWPORT_BIT =0x00000800,
	RS_TRANSFORM_BIT	=0x00001000,
	RS_ENABLE_BIT	=0x00002000,
	RS_COLOR_BUFFER_BIT	=0x00004000,
	RS_HINT_BIT	=0x00008000,
	RS_EVAL_BIT	=0x00010000,
	RS_LIST_BIT	=0x00020000,
	RS_TEXTURE_BIT	=0x00040000,
	RS_SCISSOR_BIT	=0x00080000,
	RS_ALL_ATTRIB_BITS	=0x000fffff,
	RS_CLIENT_PIXEL_STORE_BIT	=0x00000001,
	RS_CLIENT_VERTEX_ARRAY_BIT	=0x00000002,
	RS_CLIENT_ALL_ATTRIB_BITS	=0xFFFFFFFF,
	RS_FALSE	=0,
	RS_TRUE =1,
	RS_POINTS	=0x0000,
	RS_LINES	=0x0001,
	RS_LINE_LOOP	=0x0002,
	RS_LINE_STRIP	=0x0003,
	RS_TRIANGLES	=0x0004,
	RS_TRIANGLE_STRIP	=0x0005,
	RS_TRIANGLE_FAN =0x0006,
	RS_QUADS	=0x0007,
	RS_QUAD_STRIP	=0x0008,
	RS_POLYGON	=0x0009,
	RS_ACCUM	=0x0100,
	RS_LOAD =0x0101,
	RS_RETURN	=0x0102,
	RS_MULT =0x0103,
	RS_ADD	=0x0104,
	RS_NEVER	=0x0200,
	RS_LESS =0x0201,
	RS_EQUAL	=0x0202,
	RS_LEQUAL	=0x0203,
	RS_GREATER	=0x0204,
	RS_NOTEQUAL	=0x0205,
	RS_GEQUAL	=0x0206,
	RS_ALWAYS	=0x0207,
	RS_ZERO =0,
	RS_ONE	=1,
	RS_SRC_COLOR	=0x0300,
	RS_ONE_MINUS_SRC_COLOR	=0x0301,
	RS_SRC_ALPHA	=0x0302,
	RS_ONE_MINUS_SRC_ALPHA	=0x0303,
	RS_DST_ALPHA	=0x0304,
	RS_ONE_MINUS_DST_ALPHA	=0x0305,
	RS_DST_COLOR	=0x0306,
	RS_ONE_MINUS_DST_COLOR	=0x0307,
	RS_SRC_ALPHA_SATURATE	=0x0308,
//	  RS_NONE =0,
	RS_FRONT_LEFT	=0x0400,
	RS_FRONT_RIGHT	=0x0401,
	RS_BACK_LEFT	=0x0402,
	RS_BACK_RIGHT	=0x0403,
	RS_FRONT	=0x0404,
	RS_BACK =0x0405,
	RS_LEFT =0x0406,
	RS_RIGHT	=0x0407,
	RS_FRONT_AND_BACK	=0x0408,
	RS_AUX0 =0x0409,
	RS_AUX1 =0x040A,
	RS_AUX2 =0x040B,
	RS_AUX3 =0x040C,
	RS_NO_ERROR	=0,
	RS_INVALID_ENUM =0x0500,
	RS_INVALID_VALUE	=0x0501,
	RS_INVALID_OPERATION	=0x0502,
	RS_STACK_OVERFLOW	=0x0503,
	RS_STACK_UNDERFLOW	=0x0504,
	RS_OUT_OF_MEMORY	=0x0505,
	RS_2D	=0x0600,
	RS_3D	=0x0601,
	RS_3D_COLOR	=0x0602,
	RS_3D_COLOR_TEXTURE	=0x0603,
	RS_4D_COLOR_TEXTURE	=0x0604,
	RS_PASS_THROUGH_TOKEN	=0x0700,
	RS_POINT_TOKEN	=0x0701,
	RS_LINE_TOKEN	=0x0702,
	RS_POLYGON_TOKEN	=0x0703,
	RS_BITMAP_TOKEN =0x0704,
	RS_DRAW_PIXEL_TOKEN	=0x0705,
	RS_COPY_PIXEL_TOKEN	=0x0706,
	RS_LINE_RESET_TOKEN	=0x0707,
	RS_EXP	=0x0800,
	RS_EXP2 =0x0801,
	RS_CW	=0x0900,
	RS_CCW	=0x0901,
	RS_COEFF	=0x0A00,
	RS_ORDER	=0x0A01,
	RS_DOMAIN	=0x0A02,
	RS_PIXEL_MAP_I_TO_I	=0x0C70,
	RS_PIXEL_MAP_S_TO_S	=0x0C71,
	RS_PIXEL_MAP_I_TO_R	=0x0C72,
	RS_PIXEL_MAP_I_TO_G	=0x0C73,
	RS_PIXEL_MAP_I_TO_B	=0x0C74,
	RS_PIXEL_MAP_I_TO_A	=0x0C75,
	RS_PIXEL_MAP_R_TO_R	=0x0C76,
	RS_PIXEL_MAP_G_TO_G	=0x0C77,
	RS_PIXEL_MAP_B_TO_B	=0x0C78,
	RS_PIXEL_MAP_A_TO_A	=0x0C79,
	RS_VERTEX_ARRAY_POINTER =0x808E,
	RS_NORMAL_ARRAY_POINTER =0x808F,
	RS_COLOR_ARRAY_POINTER	=0x8090,
	RS_INDEX_ARRAY_POINTER	=0x8091,
	RS_TEXTURE_COORD_ARRAY_POINTER	=0x8092,
	RS_EDGE_FLAG_ARRAY_POINTER	=0x8093,
	RS_CURRENT_COLOR	=0x0B00,
	RS_CURRENT_INDEX	=0x0B01,
	RS_CURRENT_NORMAL	=0x0B02,
	RS_CURRENT_TEXTURE_COORDS	=0x0B03,
	RS_CURRENT_RASTER_COLOR =0x0B04,
	RS_CURRENT_RASTER_INDEX =0x0B05,
	RS_CURRENT_RASTER_TEXTURE_COORDS	=0x0B06,
	RS_CURRENT_RASTER_POSITION	=0x0B07,
	RS_CURRENT_RASTER_POSITION_VALID	=0x0B08,
	RS_CURRENT_RASTER_DISTANCE	=0x0B09,
	RS_POINT_SMOOTH =0x0B10,
	RS_POINT_SIZE	=0x0B11,
	RS_POINT_SIZE_RANGE	=0x0B12,
	RS_POINT_SIZE_GRANULARITY	=0x0B13,
	RS_LINE_SMOOTH	=0x0B20,
	RS_LINE_WIDTH	=0x0B21,
	RS_LINE_WIDTH_RANGE	=0x0B22,
	RS_LINE_WIDTH_GRANULARITY	=0x0B23,
	RS_LINE_STIPPLE =0x0B24,
	RS_LINE_STIPPLE_PATTERN =0x0B25,
	RS_LINE_STIPPLE_REPEAT	=0x0B26,
	RS_LIST_MODE	=0x0B30,
	RS_MAX_LIST_NESTING	=0x0B31,
	RS_LIST_BASE	=0x0B32,
	RS_LIST_INDEX	=0x0B33,
	RS_POLYGON_MODE =0x0B40,
	RS_POLYGON_SMOOTH	=0x0B41,
	RS_POLYGON_STIPPLE	=0x0B42,
	RS_EDGE_FLAG	=0x0B43,
	RS_CULL_FACE	=0x0B44,
	RS_CULL_FACE_MODE	=0x0B45,
	RS_FRONT_FACE	=0x0B46,
	RS_LIGHTING	=0x0B50,
	RS_LIGHT_MODEL_LOCAL_VIEWER	=0x0B51,
	RS_LIGHT_MODEL_TWO_SIDE =0x0B52,
	RS_LIGHT_MODEL_AMBIENT	=0x0B53,
	RS_SHADE_MODEL	=0x0B54,
	RS_COLOR_MATERIAL_FACE	=0x0B55,
	RS_COLOR_MATERIAL_PARAMETER	=0x0B56,
	RS_COLOR_MATERIAL	=0x0B57,
	RS_FOG	=0x0B60,
	RS_FOG_INDEX	=0x0B61,
	RS_FOG_DENSITY	=0x0B62,
	RS_FOG_START	=0x0B63,
	RS_FOG_END	=0x0B64,
	RS_FOG_MODE	=0x0B65,
	RS_FOG_COLOR	=0x0B66,
	RS_DEPTH_RANGE	=0x0B70,
	RS_DEPTH_TEST	=0x0B71,
	RS_DEPTH_WRITEMASK	=0x0B72,
	RS_DEPTH_CLEAR_VALUE	=0x0B73,
	RS_DEPTH_FUNC	=0x0B74,
	RS_ACCUM_CLEAR_VALUE	=0x0B80,
	RS_STENCIL_TEST =0x0B90,
	RS_STENCIL_CLEAR_VALUE	=0x0B91,
	RS_STENCIL_FUNC =0x0B92,
	RS_STENCIL_VALUE_MASK	=0x0B93,
	RS_STENCIL_FAIL =0x0B94,
	RS_STENCIL_PASS_DEPTH_FAIL	=0x0B95,
	RS_STENCIL_PASS_DEPTH_PASS	=0x0B96,
	RS_STENCIL_REF	=0x0B97,
	RS_STENCIL_WRITEMASK	=0x0B98,
	RS_MATRIX_MODE	=0x0BA0,
	RS_NORMALIZE	=0x0BA1,
	RS_VIEWPORT	=0x0BA2,
	RS_MODELVIEW_STACK_DEPTH	=0x0BA3,
	RS_PROJECTION_STACK_DEPTH	=0x0BA4,
	RS_TEXTURE_STACK_DEPTH	=0x0BA5,
	RS_MODELVIEW_MATRIX	=0x0BA6,
	RS_PROJECTION_MATRIX	=0x0BA7,
	RS_TEXTURE_MATRIX	=0x0BA8,
	RS_ATTRIB_STACK_DEPTH	=0x0BB0,
	RS_CLIENT_ATTRIB_STACK_DEPTH	=0x0BB1,
	RS_ALPHA_TEST	=0x0BC0,
	RS_ALPHA_TEST_FUNC	=0x0BC1,
	RS_ALPHA_TEST_REF	=0x0BC2,
	RS_DITHER	=0x0BD0,
	RS_BLEND_DST	=0x0BE0,
	RS_BLEND_SRC	=0x0BE1,
	RS_BLEND	=0x0BE2,
	RS_LOGIC_OP_MODE	=0x0BF0,
	RS_INDEX_LOGIC_OP	=0x0BF1,
	RS_LOGIC_OP	=RS_INDEX_LOGIC_OP,
	RS_COLOR_LOGIC_OP	=0x0BF2,
	RS_AUX_BUFFERS	=0x0C00,
	RS_DRAW_BUFFER	=0x0C01,
	RS_READ_BUFFER	=0x0C02,
	RS_SCISSOR_BOX	=0x0C10,
	RS_SCISSOR_TEST =0x0C11,
	RS_INDEX_CLEAR_VALUE	=0x0C20,
	RS_INDEX_WRITEMASK	=0x0C21,
	RS_COLOR_CLEAR_VALUE	=0x0C22,
	RS_COLOR_WRITEMASK	=0x0C23,
	RS_INDEX_MODE	=0x0C30,
	RS_RGBA_MODE	=0x0C31,
	RS_DOUBLEBUFFER =0x0C32,
	RS_STEREO	=0x0C33,
	RS_RENDER_MODE	=0x0C40,
	RS_PERSPECTIVE_CORRECTION_HINT	=0x0C50,
	RS_POINT_SMOOTH_HINT	=0x0C51,
	RS_LINE_SMOOTH_HINT	=0x0C52,
	RS_POLYGON_SMOOTH_HINT	=0x0C53,
	RS_FOG_HINT	=0x0C54,
	RS_TEXTURE_GEN_S	=0x0C60,
	RS_TEXTURE_GEN_T	=0x0C61,
	RS_TEXTURE_GEN_R	=0x0C62,
	RS_TEXTURE_GEN_Q	=0x0C63,
	RS_PIXEL_MAP_I_TO_I_SIZE	=0x0CB0,
	RS_PIXEL_MAP_S_TO_S_SIZE	=0x0CB1,
	RS_PIXEL_MAP_I_TO_R_SIZE	=0x0CB2,
	RS_PIXEL_MAP_I_TO_G_SIZE	=0x0CB3,
	RS_PIXEL_MAP_I_TO_B_SIZE	=0x0CB4,
	RS_PIXEL_MAP_I_TO_A_SIZE	=0x0CB5,
	RS_PIXEL_MAP_R_TO_R_SIZE	=0x0CB6,
	RS_PIXEL_MAP_G_TO_G_SIZE	=0x0CB7,
	RS_PIXEL_MAP_B_TO_B_SIZE	=0x0CB8,
	RS_PIXEL_MAP_A_TO_A_SIZE	=0x0CB9,
	RS_UNPACK_SWAP_BYTES	=0x0CF0,
	RS_UNPACK_LSB_FIRST	=0x0CF1,
	RS_UNPACK_ROW_LENGTH	=0x0CF2,
	RS_UNPACK_SKIP_ROWS	=0x0CF3,
	RS_UNPACK_SKIP_PIXELS	=0x0CF4,
	RS_UNPACK_ALIGNMENT	=0x0CF5,
	RS_PACK_SWAP_BYTES	=0x0D00,
	RS_PACK_LSB_FIRST	=0x0D01,
	RS_PACK_ROW_LENGTH	=0x0D02,
	RS_PACK_SKIP_ROWS	=0x0D03,
	RS_PACK_SKIP_PIXELS	=0x0D04,
	RS_PACK_ALIGNMENT	=0x0D05,
	RS_MAP_COLOR	=0x0D10,
	RS_MAP_STENCIL	=0x0D11,
	RS_INDEX_SHIFT	=0x0D12,
	RS_INDEX_OFFSET =0x0D13,
	RS_RED_SCALE	=0x0D14,
	RS_RED_BIAS	=0x0D15,
	RS_ZOOM_X	=0x0D16,
	RS_ZOOM_Y	=0x0D17,
	RS_GREEN_SCALE	=0x0D18,
	RS_GREEN_BIAS	=0x0D19,
	RS_BLUE_SCALE	=0x0D1A,
	RS_BLUE_BIAS	=0x0D1B,
	RS_ALPHA_SCALE	=0x0D1C,
	RS_ALPHA_BIAS	=0x0D1D,
	RS_DEPTH_SCALE	=0x0D1E,
	RS_DEPTH_BIAS	=0x0D1F,
	RS_MAX_EVAL_ORDER	=0x0D30,
	RS_MAX_LIGHTS	=0x0D31,
	RS_MAX_CLIP_PLANES	=0x0D32,
	RS_MAX_TEXTURE_SIZE	=0x0D33,
	RS_MAX_PIXEL_MAP_TABLE	=0x0D34,
	RS_MAX_ATTRIB_STACK_DEPTH	=0x0D35,
	RS_MAX_MODELVIEW_STACK_DEPTH	=0x0D36,
	RS_MAX_NAME_STACK_DEPTH =0x0D37,
	RS_MAX_PROJECTION_STACK_DEPTH	=0x0D38,
	RS_MAX_TEXTURE_STACK_DEPTH	=0x0D39,
	RS_MAX_VIEWPORT_DIMS	=0x0D3A,
	RS_MAX_CLIENT_ATTRIB_STACK_DEPTH	=0x0D3B,
	RS_SUBPIXEL_BITS	=0x0D50,
	RS_INDEX_BITS	=0x0D51,
	RS_RED_BITS	=0x0D52,
	RS_GREEN_BITS	=0x0D53,
	RS_BLUE_BITS	=0x0D54,
	RS_ALPHA_BITS	=0x0D55,
	RS_DEPTH_BITS	=0x0D56,
	RS_STENCIL_BITS =0x0D57,
	RS_ACCUM_RED_BITS	=0x0D58,
	RS_ACCUM_GREEN_BITS	=0x0D59,
	RS_ACCUM_BLUE_BITS	=0x0D5A,
	RS_ACCUM_ALPHA_BITS	=0x0D5B,
	RS_NAME_STACK_DEPTH	=0x0D70,
	RS_AUTO_NORMAL	=0x0D80,
	RS_MAP1_COLOR_4 =0x0D90,
	RS_MAP1_INDEX	=0x0D91,
	RS_MAP1_NORMAL	=0x0D92,
	RS_MAP1_TEXTURE_COORD_1 =0x0D93,
	RS_MAP1_TEXTURE_COORD_2 =0x0D94,
	RS_MAP1_TEXTURE_COORD_3 =0x0D95,
	RS_MAP1_TEXTURE_COORD_4 =0x0D96,
	RS_MAP1_VERTEX_3	=0x0D97,
	RS_MAP1_VERTEX_4	=0x0D98,
	RS_MAP2_COLOR_4 =0x0DB0,
	RS_MAP2_INDEX	=0x0DB1,
	RS_MAP2_NORMAL	=0x0DB2,
	RS_MAP2_TEXTURE_COORD_1 =0x0DB3,
	RS_MAP2_TEXTURE_COORD_2 =0x0DB4,
	RS_MAP2_TEXTURE_COORD_3 =0x0DB5,
	RS_MAP2_TEXTURE_COORD_4 =0x0DB6,
	RS_MAP2_VERTEX_3	=0x0DB7,
	RS_MAP2_VERTEX_4	=0x0DB8,
	RS_MAP1_GRID_DOMAIN	=0x0DD0,
	RS_MAP1_GRID_SEGMENTS	=0x0DD1,
	RS_MAP2_GRID_DOMAIN	=0x0DD2,
	RS_MAP2_GRID_SEGMENTS	=0x0DD3,
	RS_TEXTURE_1D	=0x0DE0,
	RS_TEXTURE_2D	=0x0DE1,
	RS_FEEDBACK_BUFFER_POINTER	=0x0DF0,
	RS_FEEDBACK_BUFFER_SIZE =0x0DF1,
	RS_FEEDBACK_BUFFER_TYPE =0x0DF2,
	RS_SELECTION_BUFFER_POINTER	=0x0DF3,
	RS_SELECTION_BUFFER_SIZE	=0x0DF4,
	RS_POLYGON_OFFSET_UNITS =0x2A00,
	RS_POLYGON_OFFSET_POINT =0x2A01,
	RS_POLYGON_OFFSET_LINE	=0x2A02,
	RS_POLYGON_OFFSET_FILL	=0x8037,
	RS_POLYGON_OFFSET_FACTOR	=0x8038,
	RS_TEXTURE_BINDING_1D	=0x8068,
	RS_TEXTURE_BINDING_2D	=0x8069,
	RS_VERTEX_ARRAY =0x8074,
	RS_NORMAL_ARRAY =0x8075,
	RS_COLOR_ARRAY	=0x8076,
	RS_INDEX_ARRAY	=0x8077,
	RS_TEXTURE_COORD_ARRAY	=0x8078,
	RS_EDGE_FLAG_ARRAY	=0x8079,
	RS_VERTEX_ARRAY_SIZE	=0x807A,
	RS_VERTEX_ARRAY_TYPE	=0x807B,
	RS_VERTEX_ARRAY_STRIDE	=0x807C,
	RS_NORMAL_ARRAY_TYPE	=0x807E,
	RS_NORMAL_ARRAY_STRIDE	=0x807F,
	RS_COLOR_ARRAY_SIZE	=0x8081,
	RS_COLOR_ARRAY_TYPE	=0x8082,
	RS_COLOR_ARRAY_STRIDE	=0x8083,
	RS_INDEX_ARRAY_TYPE	=0x8085,
	RS_INDEX_ARRAY_STRIDE	=0x8086,
	RS_TEXTURE_COORD_ARRAY_SIZE	=0x8088,
	RS_TEXTURE_COORD_ARRAY_TYPE	=0x8089,
	RS_TEXTURE_COORD_ARRAY_STRIDE	=0x808A,
	RS_EDGE_FLAG_ARRAY_STRIDE	=0x808C,
	RS_TEXTURE_WIDTH	=0x1000,
	RS_TEXTURE_HEIGHT	=0x1001,
	RS_TEXTURE_INTERNAL_FORMAT	=0x1003,
	RS_TEXTURE_COMPONENTS	=RS_TEXTURE_INTERNAL_FORMAT,
	RS_TEXTURE_BORDER_COLOR =0x1004,
	RS_TEXTURE_BORDER	=0x1005,
	RS_TEXTURE_RED_SIZE	=0x805C,
	RS_TEXTURE_GREEN_SIZE	=0x805D,
	RS_TEXTURE_BLUE_SIZE	=0x805E,
	RS_TEXTURE_ALPHA_SIZE	=0x805F,
	RS_TEXTURE_LUMINANCE_SIZE	=0x8060,
	RS_TEXTURE_INTENSITY_SIZE	=0x8061,
	RS_TEXTURE_PRIORITY	=0x8066,
	RS_TEXTURE_RESIDENT	=0x8067,
	RS_DONT_CARE	=0x1100,
	RS_FASTEST	=0x1101,
	RS_NICEST	=0x1102,
	RS_AMBIENT	=0x1200,
	RS_DIFFUSE	=0x1201,
	RS_SPECULAR	=0x1202,
	RS_POSITION	=0x1203,
	RS_SPOT_DIRECTION	=0x1204,
	RS_SPOT_EXPONENT	=0x1205,
	RS_SPOT_CUTOFF	=0x1206,
	RS_CONSTANT_ATTENUATION =0x1207,
	RS_LINEAR_ATTENUATION	=0x1208,
	RS_QUADRATIC_ATTENUATION	=0x1209,
	RS_COMPILE	=0x1300,
	RS_COMPILE_AND_EXECUTE	=0x1301,
	RS_BYTE =0x1400,
	RS_UNSIGNED_BYTE	=0x1401,
	RS_SHORT	=0x1402,
	RS_UNSIGNED_SHORT	=0x1403,
	RS_INT	=0x1404,
	RS_UNSIGNED_INT =0x1405,
	RS_FLOAT	=0x1406,
	RS_2_BYTES	=0x1407,
	RS_3_BYTES	=0x1408,
	RS_4_BYTES	=0x1409,
	RS_DOUBLE	=0x140A,
	RS_DOUBLE_EXT	=0x140A,
	RS_CLEAR	=0x1500,
	RS_AND	=0x1501,
	RS_AND_REVERSE	=0x1502,
	RS_COPY =0x1503,
	RS_AND_INVERTED =0x1504,
	RS_NOOP =0x1505,
	RS_XOR	=0x1506,
	RS_OR	=0x1507,
	RS_NOR	=0x1508,
	RS_EQUIV	=0x1509,
	RS_INVERT	=0x150A,
	RS_OR_REVERSE	=0x150B,
	RS_COPY_INVERTED	=0x150C,
	RS_OR_INVERTED	=0x150D,
	RS_NAND =0x150E,
	RS_SET	=0x150F,
	RS_EMISSION	=0x1600,
	RS_SHININESS	=0x1601,
	RS_AMBIENT_AND_DIFFUSE	=0x1602,
	RS_COLOR_INDEXES	=0x1603,
	RS_MODELVIEW	=0x1700,
	RS_PROJECTION	=0x1701,
	RS_TEXTURE	=0x1702,
	RS_COLOR	=0x1800,
	RS_DEPTH	=0x1801,
	RS_STENCIL	=0x1802,
	RS_COLOR_INDEX	=0x1900,
	RS_STENCIL_INDEX	=0x1901,
	RS_DEPTH_COMPONENT	=0x1902,
	RS_RED	=0x1903,
	RS_GREEN	=0x1904,
	RS_BLUE =0x1905,
	RS_ALPHA	=0x1906,
	RS_RGB	=0x1907,
	RS_RGBA =0x1908,
	RS_LUMINANCE	=0x1909,
	RS_LUMINANCE_ALPHA	=0x190A,
	RS_BITMAP	=0x1A00,
	RS_POINT	=0x1B00,
	RS_LINE =0x1B01,
	RS_FILL =0x1B02,
	RS_RENDER	=0x1C00,
	RS_FEEDBACK	=0x1C01,
	RS_SELECT	=0x1C02,
	RS_FLAT =0x1D00,
	RS_SMOOTH	=0x1D01,
	RS_KEEP =0x1E00,
	RS_REPLACE	=0x1E01,
	RS_INCR =0x1E02,
	RS_DECR =0x1E03,
	RS_VENDOR	=0x1F00,

	RS_RENDERER	=0x1F01,
	RS_VERSION	=0x1F02,
	RS_EXTENSIONS	=0x1F03,
	RS_S	=0x2000,
	RS_T	=0x2001,
	RS_R	=0x2002,
	RS_Q	=0x2003,
	RS_MODULATE	=0x2100,
	RS_DECAL	=0x2101,
	RS_TEXTURE_ENV_MODE	=0x2200,
	RS_TEXTURE_ENV_COLOR	=0x2201,
	RS_TEXTURE_ENV	=0x2300,
	RS_EYE_LINEAR	=0x2400,
	RS_OBJECT_LINEAR	=0x2401,
	RS_SPHERE_MAP	=0x2402,
	RS_TEXTURE_GEN_MODE	=0x2500,
	RS_OBJECT_PLANE =0x2501,
	RS_EYE_PLANE	=0x2502,
	RS_NEAREST	=0x2600,
	RS_LINEAR	=0x2601,
	RS_NEAREST_MIPMAP_NEAREST	=0x2700,
	RS_LINEAR_MIPMAP_NEAREST	=0x2701,
	RS_NEAREST_MIPMAP_LINEAR	=0x2702,
	RS_LINEAR_MIPMAP_LINEAR =0x2703,







	RS_TEXTURE_MAG_FILTER	=0x2800,
	RS_TEXTURE_MIN_FILTER	=0x2801,
	RS_TEXTURE_WRAP_S	=0x2802,
	RS_TEXTURE_WRAP_T	=0x2803,
	RS_PROXY_TEXTURE_1D	=0x8063,
	RS_PROXY_TEXTURE_2D	=0x8064,
	RS_CLAMP	=0x2900,
	RS_REPEAT	=0x2901,
	RS_R3_G3_B2	=0x2A10,
	RS_ALPHA4	=0x803B,
	RS_ALPHA8	=0x803C,
	RS_ALPHA12	=0x803D,
	RS_ALPHA16	=0x803E,
	RS_LUMINANCE4	=0x803F,
	RS_LUMINANCE8	=0x8040,
	RS_LUMINANCE12	=0x8041,
	RS_LUMINANCE16	=0x8042,
	RS_LUMINANCE4_ALPHA4	=0x8043,

	RS_LUMINANCE6_ALPHA2	=0x8044,
	RS_LUMINANCE8_ALPHA8	=0x8045,
	RS_LUMINANCE12_ALPHA4	=0x8046,
	RS_LUMINANCE12_ALPHA12	=0x8047,
	RS_LUMINANCE16_ALPHA16	=0x8048,
	RS_INTENSITY	=0x8049,
	RS_INTENSITY4	=0x804A,
	RS_INTENSITY8	=0x804B,
	RS_INTENSITY12	=0x804C,
	RS_INTENSITY16	=0x804D,
	RS_RGB4 =0x804F,
	RS_RGB5 =0x8050,
	RS_RGB8 =0x8051,
	RS_RGB10	=0x8052,
	RS_RGB12	=0x8053,
	RS_RGB16	=0x8054,



	RS_RGBA2	=0x8055,
	RS_RGBA4	=0x8056,
	RS_RGB5_A1	=0x8057,
	RS_RGBA8	=0x8058,
	RS_RGB10_A2	=0x8059,
	RS_RGBA12	=0x805A,
	RS_RGBA16	=0x805B,
	RS_V2F	=0x2A20,
	RS_V3F	=0x2A21,
	RS_C4UB_V2F	=0x2A22,
	RS_C4UB_V3F	=0x2A23,
	RS_C3F_V3F	=0x2A24,
	RS_N3F_V3F	=0x2A25,
	RS_C4F_N3F_V3F	=0x2A26,
	RS_T2F_V3F	=0x2A27,
	RS_T4F_V4F	=0x2A28,
	RS_T2F_C4UB_V3F =0x2A29,
	RS_T2F_C3F_V3F	=0x2A2A,
	RS_T2F_N3F_V3F	=0x2A2B,
	RS_T2F_C4F_N3F_V3F	=0x2A2C,
	RS_T4F_C4F_N3F_V4F	=0x2A2D,
	RS_CLIP_PLANE0	=0x3000,
	RS_CLIP_PLANE1	=0x3001,
	RS_CLIP_PLANE2	=0x3002,
	RS_CLIP_PLANE3	=0x3003,
	RS_CLIP_PLANE4	=0x3004,
	RS_CLIP_PLANE5	=0x3005,
	RS_LIGHT0	=0x4000,
	RS_LIGHT1	=0x4001,
	RS_LIGHT2	=0x4002,
	RS_LIGHT3	=0x4003,
	RS_LIGHT4	=0x4004,
	RS_LIGHT5	=0x4005,
	RS_LIGHT6	=0x4006,
	RS_LIGHT7	=0x4007,
	RS_ABGR_EXT	=0x8000,
	RS_UNSIGNED_BYTE_3_3_2_EXT	=0x8032,
	RS_UNSIGNED_SHORT_4_4_4_4_EXT	=0x8033,
	RS_UNSIGNED_SHORT_5_5_5_1_EXT	=0x8034,
	RS_UNSIGNED_INT_8_8_8_8_EXT	=0x8035,
	RS_UNSIGNED_INT_10_10_10_2_EXT	=0x8036,
	RS_VERTEX_ARRAY_EXT	=0x8074,
	RS_NORMAL_ARRAY_EXT	=0x8075,
	RS_COLOR_ARRAY_EXT	=0x8076,
	RS_INDEX_ARRAY_EXT	=0x8077,
	RS_TEXTURE_COORD_ARRAY_EXT	=0x8078,
	RS_EDGE_FLAG_ARRAY_EXT	=0x8079,
	RS_VERTEX_ARRAY_SIZE_EXT	=0x807A,
	RS_VERTEX_ARRAY_TYPE_EXT	=0x807B,
	RS_VERTEX_ARRAY_STRIDE_EXT	=0x807C,
	RS_VERTEX_ARRAY_COUNT_EXT	=0x807D,
	RS_NORMAL_ARRAY_TYPE_EXT	=0x807E,
	RS_NORMAL_ARRAY_STRIDE_EXT	=0x807F,
	RS_NORMAL_ARRAY_COUNT_EXT	=0x8080,
	RS_COLOR_ARRAY_SIZE_EXT =0x8081,
	RS_COLOR_ARRAY_TYPE_EXT =0x8082,
	RS_COLOR_ARRAY_STRIDE_EXT	=0x8083,
	RS_COLOR_ARRAY_COUNT_EXT	=0x8084,
	RS_INDEX_ARRAY_TYPE_EXT =0x8085,
	RS_INDEX_ARRAY_STRIDE_EXT	=0x8086,
	RS_INDEX_ARRAY_COUNT_EXT	=0x8087,
	RS_TEXTURE_COORD_ARRAY_SIZE_EXT =0x8088,
	RS_TEXTURE_COORD_ARRAY_TYPE_EXT =0x8089,
	RS_TEXTURE_COORD_ARRAY_STRIDE_EXT	=0x808A,
	RS_TEXTURE_COORD_ARRAY_COUNT_EXT	=0x808B,
	RS_EDGE_FLAG_ARRAY_STRIDE_EXT	=0x808C,
	RS_EDGE_FLAG_ARRAY_COUNT_EXT	=0x808D,
	RS_VERTEX_ARRAY_POINTER_EXT	=0x808E,
	RS_NORMAL_ARRAY_POINTER_EXT	=0x808F,
	RS_COLOR_ARRAY_POINTER_EXT	=0x8090,
	RS_INDEX_ARRAY_POINTER_EXT	=0x8091,
	RS_TEXTURE_COORD_ARRAY_POINTER_EXT	=0x8092,
	RS_EDGE_FLAG_ARRAY_POINTER_EXT	=0x8093,
	RS_TABLE_TOO_LARGE_EXT	=0x8031,
	RS_COLOR_TABLE_FORMAT_EXT	=0x80D8,
	RS_COLOR_TABLE_WIDTH_EXT	=0x80D9,
	RS_COLOR_TABLE_RED_SIZE_EXT	=0x80DA,
	RS_COLOR_TABLE_GREEN_SIZE_EXT	=0x80DB,
	RS_COLOR_TABLE_BLUE_SIZE_EXT	=0x80DC,
	RS_COLOR_TABLE_ALPHA_SIZE_EXT	=0x80DD,
	RS_COLOR_TABLE_LUMINANCE_SIZE_EXT	=0x80DE,
	RS_COLOR_TABLE_INTENSITY_SIZE_EXT	=0x80DF,
	RS_BGR_EXT	=0x80E0,
	RS_BGRA_EXT	=0x80E1,
	RS_COLOR_INDEX1_EXT	=0x80E2,
	RS_COLOR_INDEX2_EXT	=0x80E3,
	RS_COLOR_INDEX4_EXT	=0x80E4,
	RS_COLOR_INDEX8_EXT	=0x80E5,
	RS_COLOR_INDEX12_EXT	=0x80E6,
	RS_COLOR_INDEX16_EXT	=0x80E7,
	RS_ARRAY_ELEMENT_LOCK_FIRST_SGI =0x81A8,
	RS_ARRAY_ELEMENT_LOCK_COUNT_SGI =0x81A9,
	RS_CULL_VERTEX_SGI	=0x81AA,
	RS_CULL_VERTEX_EYE_POSITION_SGI =0x81AB,
	RS_CULL_VERTEX_OBJECT_POSITION_SGI	=0x81AC,
	RS_IUI_V2F_SGI	=0x81AD,
	RS_IUI_V3F_SGI	=0x81AE,
	RS_IUI_N3F_V2F_SGI	=0x81AF,
	RS_IUI_N3F_V3F_SGI	=0x81B0,
	RS_T2F_IUI_V2F_SGI	=0x81B1,
	RS_T2F_IUI_V3F_SGI	=0x81B2,
	RS_T2F_IUI_N3F_V2F_SGI	=0x81B3,
	RS_T2F_IUI_N3F_V3F_SGI	=0x81B4,
	RS_INDEX_TEST_SGI	=0x81B5,
	RS_INDEX_TEST_FUNC_SGI	=0x81B6,
	RS_INDEX_TEST_REF_SGI	=0x81B7,
	RS_INDEX_MATERIAL_SGI	=0x81B8,
	RS_INDEX_MATERIAL_PARAMETER_SGI =0x81B9,
	RS_INDEX_MATERIAL_FACE_SGI	=0x81BA
} RSGLENUMERATION;
typedef enum _RSCMPFUNC {
    RSCMP_NEVER 	      = 1,
    RSCMP_LESS		      = 2,
    RSCMP_EQUAL 	      = 3,
    RSCMP_LESSEQUAL	      = 4,
    RSCMP_GREATER	      = 5,
    RSCMP_NOTEQUAL	      = 6,
    RSCMP_GREATEREQUAL	      = 7,
    RSCMP_ALWAYS	      = 8,
#if(DIRECT3D_VERSION >= 0x0500)
    RSCMP_FORCE_DWORD	      = 0x7fffffff, /* force 32-bit size enum */
#endif /* DIRECT3D_VERSION >= 0x0500 */
} RSCMPFUNC;
typedef enum _RSBLEND {
    RSBLEND_ZERO	       = 1,
    RSBLEND_ONE 	       = 2,
    RSBLEND_SRCCOLOR	       = 3,
    RSBLEND_INVSRCCOLOR        = 4,
    RSBLEND_SRCALPHA	       = 5,
    RSBLEND_INVSRCALPHA        = 6,
    RSBLEND_DESTALPHA	       = 7,
    RSBLEND_INVDESTALPHA       = 8,
    RSBLEND_DESTCOLOR	       = 9,
    RSBLEND_INVDESTCOLOR       = 10,
    RSBLEND_SRCALPHASAT        = 11,
    RSBLEND_BOTHSRCALPHA       = 12,
    RSBLEND_BOTHINVSRCALPHA    = 13,
    RSBLEND_FORCE_DWORD        = 0x7fffffff, /* force 32-bit size enum */
} RSBLEND;
typedef enum _RSBLENDOP {
    RSBLENDOP_ADD	       = 1,
    RSBLENDOP_SUBTRACT	       = 2,
    RSBLENDOP_REVSUBTRACT      = 3,
    RSBLENDOP_MIN	       = 4,
    RSBLENDOP_MAX	       = 5,
    RSBLENDOP_FORCE_DWORD      = 0x7fffffff, /* force 32-bit size enum */
} RSBLENDOP;
// This was created to allow us to use this camera code in other projects
// without having to cut and paste code.  This file and camera.cpp could be
// added to a future project.
// This is our camera class
enum ImageExt
{
	IF_DDS,
	IF_JPG,
	IF_BMP,
	IF_TGA,
	IF_PNG,
	IF_PPM,
	IF_DIB,
	IF_HDR,
	IF_PFM,
};
typedef struct _BOX
{
    UINT		Left;
    UINT		Top;
    UINT		Right;
    UINT		Bottom;
    UINT		Front;
    UINT		Back;
} BOX;

#if 0
#if 1
//include "asyncloader.h"
#else
//--------------------------------------------------------------------------------------
// IDataLoader is an interface that the AsyncLoader class uses to load data from disk.
//
// Load is called from the IO thread to load data.
// Decompress is called by one of the processing threads to decompress the data.
// Destroy is called by the graphics thread when it has consumed the data.
//--------------------------------------------------------------------------------------
class IDataLoader3
{
public:
    virtual		    ~IDataLoader3()
    {
    };
    virtual HRESULT WINAPI  Decompress( void** ppData, SIZE_T* pcBytes ) = 0;
    virtual HRESULT WINAPI  Destroy() = 0;
    virtual HRESULT WINAPI  Load() = 0;
};

//--------------------------------------------------------------------------------------
// IDataProcessor is an interface that the AsyncLoader class uses to process and copy
// data into locked resource pointers.
//
// Process is called by one of the processing threads to process the data before it is
//   consumed.
// LockDeviceObject is called from the Graphics thread to lock the device object (D3D9).
// UnLockDeviceObject is called from the Graphics thread to unlock the device object, or
//   to call updatesubresource for D3D10.
// CopyToResource copies the data from memory to the locked device object (D3D9).
// SetResourceError is called to set the resource pointer to an error code in the event
//   that something went wrong.
// Destroy is called by the graphics thread when it has consumed the data.
//--------------------------------------------------------------------------------------
class IDataProcessor3
{
public:
    virtual		    ~IDataProcessor3()
    {
    };
    virtual HRESULT WINAPI  LockDeviceObject() = 0;
    virtual HRESULT WINAPI  UnLockDeviceObject() = 0;
    virtual HRESULT WINAPI  Destroy() = 0;
    virtual HRESULT WINAPI  Process( void* pData, SIZE_T cBytes ) = 0;

    virtual HRESULT WINAPI  CopyToResource() = 0;
    virtual void WINAPI     SetResourceError() = 0;
};
#endif
//--------------------------------------------------------------------------------------
// CResourceReuseCache class
//--------------------------------------------------------------------------------------
class CResourceReuseCache3
{
private:
VArray<TextureID>		  m_TextureList;
VArray<VertexBufferID>	 m_VBList;
VArray<IndexBufferID>	 m_IBList;
UINT64									m_MaxManagedMemory;
UINT64									m_UsedManagedMemory;
BOOL									m_bSilent;
BOOL									m_bDontCreateResources;

int FindTexture( TextureID pTex9 );
int EnsureFreeTexture( UINT Width, UINT Height, UINT MipLevels, UINT Format );

UINT64 GetEstimatedSize( UINT Width, UINT Height, UINT MipLevels, UINT Format );

int FindVB( VertexBufferID pVB );
int EnsureFreeVB( UINT iSizeBytes );

int FindIB( IndexBufferID pIB );
int EnsureFreeIB( UINT iSizeBytes, UINT ibFormat );

void DestroyTexture9( TextureID pTex );
void DestroyVB9( VertexBufferID pVB );
void DestroyIB9( IndexBufferID pVB );

public:
CResourceReuseCache3(  );
~CResourceReuseCache3();

// memory handling
void SetMaxManagedMemory( UINT64 MaxMemory );
UINT64 GetMaxManagedMemory();
UINT64 GetUsedManagedMemory();
void SetDontCreateResources( BOOL bDontCreateResources );
UINT64 DestroyLRUTexture();
UINT64 DestroyLRUVB();
UINT64 DestroyLRUIB();
void DestroyLRUResources( UINT64 SizeGainNeeded );

// texture functions
TextureID GetFreeTexture9( UINT Width, UINT Height, UINT MipLevels, UINT Format );
void UnuseDeviceTexture9( TextureID pTexture );
int GetNumTextures();
TextureID GetTexture( int i );

// vertex buffer functions
VertexBufferID GetFreeVB9( UINT sizeBytes );
void UnuseDeviceVB9( VertexBufferID pVB );
int GetNumVBs();
VertexBufferID GetVB( int i );

// index buffer functions
IndexBufferID GetFreeIB9( UINT sizeBytes, UINT ibFormat );
void UnuseDeviceIB9( IndexBufferID pVB );
int GetNumIBs();
IndexBufferID GetIB( int i );

void OnDestroy();

};
  #endif

//typedef int AsyncLoaderID;
//typedef int ResourceReuseCacheID;

std::string replaceAll( const std::string& s, const std::string& f, const std::string& r );
#if 0
struct FileCache
{
		std::vector<std::string> s_Names;
		std::vector<int> s_IDs;
			std::string fileName2;
		int begin(const char* fileName, bool stripPath=true)
		{
			fileName2="";
	int strLastSlash=0;
	if(stripPath)
	{
		std::string fileName3=fileName;

		fileName3=replaceAll(
			 fileName3,
             std::string(stx_getOsSeparatorString()),
             std::string(stx_getOsSeparatorString()) );

	std::transform(fileName3.begin(), fileName3.end(), fileName3.begin(),(int(*)(int)) tolower);
    //int strLastSlash1 = int(stx_strrchr( fileName3.c_str(), stx_getOsSeparatorString() )-fileName3.c_str());
    int strLastSlash = int(stx_strrchr( fileName3.c_str(), stx_getOsSeparator() )-fileName3.c_str());//???
	//int strLastSlash=max(strLastSlash1,strLastSlash2);
				fileName2.append(fileName3.c_str()+strLastSlash+1);
	}else {
//		for(unsigned int i=strLastSlash;i<fileName3.length();i++)
//			fileName2.append(std::string(fileName3[i]));

		std::string fileName3=fileName;
		fileName3=replaceAll(
			 fileName3,
             std::string(stx_getOsSeparatorString()),
             std::string(stx_getOsSeparatorString()) );

			fileName2.append(fileName3);
	}

		//bool found=false;
		unsigned int i=0;
		for(i=0;i<s_Names.size();i++)
		{

			if(strcmp(s_Names[i].c_str(),fileName2.c_str())==0)
			{
		//		found=true;
				return s_IDs[i];
			}
		}
		return -1;
	}
	void end(unsigned int id)
	{
		s_Names.push_back(fileName2);
		s_IDs.push_back(id);
		//LOG_PRINT("%d fileName2=%s\n",s_Names.size(),fileName2.c_str());
	}
	};
#endif
typedef int ObjectID;

#endif
struct RENDERERAPI RendererHLSLCg : public IRenderer
{
public:

	virtual unsigned int RenderTexture(TextureID id, D3DXFROMWINEVECTOR2 aposition, D3DXFROMWINEVECTOR2 asize, D3DXFROMWINEVECTOR2 texposition=D3DXFROMWINEVECTOR2(0.0f, 0.0f), D3DXFROMWINEVECTOR2 texsize=D3DXFROMWINEVECTOR2(1.0f, 1.0f));

	D3DXFROMWINEVECTOR2 sp;
	D3DXFROMWINEVECTOR2 ep;
	virtual void moveTo(int x, int y){sp=D3DXFROMWINEVECTOR2(x,y);}
	virtual void lineTo(int x, int y)
	{
		ep=D3DXFROMWINEVECTOR2(x,y);
		D3DXFROMWINEVECTOR2 vertices[2];
		vertices[0]=sp;
		vertices[1]=ep;
		D3DXFROMWINEVECTOR4 v=D3DXFROMWINEVECTOR4(1,1,1,1);
		drawPlain(PRIM_LINES, (D3DXFROMWINEVECTOR2*)&vertices[0], 2, GetblendSrcAlpha(), GetnoDepthTest(), &v);	
		sp=ep;
	}
	static void switchReset()
	{
RendererHLSLCg::vb=-1;
RendererHLSLCg::ib=-1;
	RendererHLSLCg::defaultFont=-1;
	RendererHLSLCg::fonts.clear();
	RendererHLSLCg::fonts.empty();
	}
	virtual void DumpRenderer();
	IInput* m_pInput;
	virtual void SetInput(IInput* a_pInput){m_pInput=a_pInput;}
	virtual int getFPS(){return fps;}
	virtual FontID GetdefaultFont(){return defaultFont;}

   BYTE* m_VertexPointer;
   BYTE* m_NormalPointer;
   BYTE* m_TexCoordPointer;
   int m_VertexOff;
   int m_NormalOff;
   int m_TexCoordOff;
   int m_VertexW;
   int m_NormalW;
   int m_TexCoordW;

   virtual void VertexPointer(const int w, /*GL_INT,*/ const int off, const void* p)
   {
	   m_VertexPointer=(BYTE *)p;
	   m_VertexOff=off;
	   m_VertexW=w;
   }
   virtual void NormalPointer(const int off, const void* p)
   {
	   m_VertexPointer=(BYTE *)p;
	   m_NormalOff=off;
	   m_NormalW=3;
   }
   virtual void TexCoordPointer(const int w, /*GL_FLOAT,*/ const int off, const void* p)
   {
	   m_TexCoordPointer=(BYTE *)p;
	   m_TexCoordOff=off;
	   m_TexCoordW=w;
   }


	virtual void Begin(Primitives prim);
	virtual void EndVrtl(const TextureID id=-1, const D3DXFROMWINEMATRIX* pMVP=0, const char* aXFile=0, const char* aTextureFile=0);
	virtual void Vertex2i(int x,int y);
  virtual void Vertex3i(int x,int y, int z);
  virtual void Vertex4i(int x,int y, int z ,int w);
  virtual void Vertex2f(float x,float  y);
  virtual void Vertex3f(float x,float  y, float  z);
  virtual void Vertex4f(float x,float  y, float  z ,float  w);
  virtual void Vertex2fv(const float* v);
  virtual void Vertex3fv(const float* v);
  virtual void Vertex4fv(const float* v);
  virtual void Color3f(float x,float  y, float  z);
  virtual void Color4f(float x,float  y, float  z ,float  w);
  virtual void Color3fv(const float* v);
  virtual void Color4fv(const float* v);
  virtual void TexCoord2f(float x,float  y);
  virtual void getExtents(FontID font,char *string, float x, float y, const float charWidth, const float charHeight, const float skewX = 0.0f, const float skewY = 0.0f,
		//const FONT_HADJUST hAdjust = HA_LEFT,
		//const FONT_VADJUST vAdjust = VA_TOP,
		float *left = 0, float *right = 0, float *top = 0, float *bottom = 0);

	virtual void InitTex(ShaderID& s, VertexFormatID& vf);
	virtual void InitTex3(ShaderID& s, VertexFormatID& vf);
	virtual void InitPlain(ShaderID& s, VertexFormatID& vf);
	virtual void InitPlain3(ShaderID& s, VertexFormatID& vf);

	virtual void InitTexN(ShaderID& s, VertexFormatID& vf);
	virtual void InitTex2N(ShaderID& s, VertexFormatID& vf);
	virtual void InitTex3N(ShaderID& s, VertexFormatID& vf);
	virtual void InitPlainN(ShaderID& s, VertexFormatID& vf);
	virtual void InitPlain3N(ShaderID& s, VertexFormatID& vf);

	virtual unsigned int drawArrays(const Primitives primitives, const int firstVertex, const int nVertices)=0;
	virtual unsigned int drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices)=0;

	virtual unsigned int drawLineVrtl(const float x0, const float y0, const float x1, const float y1, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);
	virtual unsigned int drawRectVrtl(const float x0, const float y0, const float x1, const float y1, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);
	virtual unsigned int drawRoundRectVrtl(const float x0, const float y0, const float x1, const float y1, const float r, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);

virtual unsigned int drawEllipseVrtl(const float originx, const float originy, const float width, const float height, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);
virtual unsigned int drawParaboleVrtl(const float xpos, const float x0, const float k, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);
virtual unsigned int drawParaboleVrtl(const float p, const float h, const unsigned int k, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);
virtual unsigned int drawParaboleVrtl(const float originx, const float originy, const float width, const float height, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);
virtual unsigned int drawParaboleVrtl(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);
virtual unsigned int drawHyperboleVrtl(const float originx, const float originy, const float width, const float height, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);
virtual unsigned int drawBezierVrtl(D3DXFROMWINEVECTOR2 anchor1, D3DXFROMWINEVECTOR2 anchor2, D3DXFROMWINEVECTOR2 control1, D3DXFROMWINEVECTOR2 control2, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);

	virtual unsigned int drawCircleVrtl(const float x, const float y, const float r, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);
	virtual unsigned int drawTriangleVrtl(const float x0, const float y0, const float x1, const float y1, const float x2, const float y2, const D3DXFROMWINEVECTOR4 &color, const float lineWidth);
	virtual unsigned int drawQuadVrtl(const float x0, const float y0, const float x1, const float y1, const float s0, const float t0, const float s1, const float t1, const D3DXFROMWINEVECTOR4 &color);

	virtual float GetAspect(){return float(m_viewport.Width)/m_viewport.Height;}
	virtual void SetInt(const char* n, int d){setShaderConstant1i(n,d);}
	virtual void SetFloat(const char* n, float d){setShaderConstant1f(n,d);}
	virtual void SetVector(const char* n, const D3DXFROMWINEVECTOR4 * d){setShaderConstant4f(n,*(const D3DXFROMWINEVECTOR4*)d);}
	virtual void SetVectorArray(const char* n, const D3DXFROMWINEVECTOR4 * d,unsigned int nn)
	{setShaderConstantRaw(n, d, sizeof(D3DXFROMWINEVECTOR4)*nn);}
	virtual void SetMatrix(const char* n, const D3DXFROMWINEMATRIX * d){setShaderConstant4x4f(n,*(const D3DXFROMWINEMATRIX*)d);}
	virtual void SetMatrixTransposeArray(const char* n, const D3DXFROMWINEMATRIX * d,unsigned int nn){IMPLEMENT;}
	virtual void SetTexture(const char* n, TextureID id)
	{
		setTexture(n,id);
		setSamplerState(n, Getlinear());
	}

    virtual BYTE* LockTexture(TextureID id, int m, int& pitch)=0;
    virtual void UnlockTexture(TextureID id, int m)=0;
    virtual BYTE* LockVertexBuffer(VertexBufferID id)=0;
    virtual void UnLockVertexBuffer(VertexBufferID id)=0;
    virtual BYTE* LockIndexBuffer(IndexBufferID id)=0;
    virtual void UnLockIndexBuffer(IndexBufferID id)=0;

	virtual float	GetMaxPointSpriteSize()=0;
	virtual void	BeginPointSpriteRendering()=0;
	virtual void	EndPointSpriteRendering()=0;
#if 0
	virtual void	CreateTextureFromFile_Serial(  CHAR* szFileName,
						TextureID* ppTexture,
						//, void* pContext
						CPackedFile*		    g_PackFile,
						UINT g_SkipMips=0
						)=0;
	virtual void	CreateVertexBuffer_Serial(  VertexBufferID& ppBuffer, UINT iSizeBytes,
//					    __DWORD__ Usage, D3DFORMAT ibFormat, D3DPOOL Pool,
						void* pData
//					    void* pContext
						)=0;
	virtual void	CreateIndexBuffer_Serial(  IndexBufferID& ppBuffer, UINT iSizeBytes,
//					    __DWORD__ Usage, D3DFORMAT ibFormat, D3DPOOL Pool,
						void* pData
//					    void* pContext
						)=0;
#endif
	//static ???
#if 0
	CResourceReuseCache*	    g_pResourceReuseCache;

	CResourceReuseCache* Getg_pResourceReuseCache()
{
	return g_pResourceReuseCache;
}
#endif

	virtual BYTE *fillVertexBufferVrtl(const VertexBufferID id,  BYTE*  m_pData,  int l)=0;

inline BYTE *fillVertexBuffer(const VertexBufferID id, BYTE*  m_pData, int l=0){return fillVertexBufferVrtl(id, m_pData, l);}
	virtual BYTE *fillIndexBufferVrtl(const IndexBufferID id,  BYTE*  m_pData,  int l)=0;

inline BYTE *fillIndexBuffer(const IndexBufferID id, BYTE*  m_pData, int l=0){return fillIndexBufferVrtl(id, m_pData, l);}
protected:
	float pp[16];
static float vv[16];
static float ww[16];
public:
	std::vector<D3DXFROMWINEMATRIX> ms;
	virtual void PushMatrix(){ms.push_back(*(D3DXFROMWINEMATRIX*)ww);}//???
	virtual void PopMatrix(){(*(D3DXFROMWINEMATRIX*)ww)=ms[ms.size()-1];ms.pop_back();}//???
	virtual void Translatef(FLOAT ax, FLOAT ay, FLOAT az)
	{
#if 0
				D3DXFROMWINEMATRIX mOut;
				if(!ms.size())
				{
					D3DXFROMWINEMatrixIdentity(&mOut);
					ms.push_back(mOut);
				}
				D3DXFROMWINEMatrixTranslation(&mOut,ax, ay, az);
				ms[ms.size()-1]*=mOut;	
#endif				
	}//???

virtual D3DXFROMWINEMATRIX GetMVP()
{
		D3DXFROMWINEMATRIX mvp;
  mvp = (*(D3DXFROMWINEMATRIX*)ww) * (*(D3DXFROMWINEMATRIX*)vv) * (*(D3DXFROMWINEMATRIX*)pp);
  D3DXFROMWINEMatrixTranspose(&mvp, &mvp);
  return mvp;
}

	static VertexBufferID vb;
	static IndexBufferID ib;

	static std::vector<D3DXFROMWINEMATRIX> s_pV;
	static std::vector<D3DXFROMWINEMATRIX> s_mvV;

	static float m_fYaw;
	static float m_fPitch;
	static float m_fRoll;
	static D3DXFROMWINEVECTOR3 m_eye;
	static D3DXFROMWINEVECTOR3 m_center;
	static D3DXFROMWINEVECTOR3 m_up;

#if 0
	static VArray <Quad> Quads;
	static VArray <Cube> Cubes;
	static VArray <FastBuffers> FastBuffersv;
	static ObjectID addQuad(ShaderID id);
	static void renderQuad(ObjectID id);
	static ObjectID addCube(ShaderID id);
	static void renderCube(ObjectID id);
	static ObjectID addFastBuffer(ShaderID id);
	static void renderFastBuffer(ObjectID id);
#endif
	//virtual FontID GetdefaultFont(){defaultFont;}
	static FontID defaultFont;
	static VArray <TexFont> fonts;
//	virtual FontID GetDefaultFont()=0;
//	virtual TexFont* GetFont(unsigned int i)=0;
//	virtual VArray <TexFont*>& GetFonts()=0;

#if 0
	static VArray <CTextureLoader> textureLoaders;
	static VArray <CVertexBufferLoader> vertexBufferLoaders;
	static VArray <CIndexBufferLoader> indexBufferLoaders;
#endif
	struct Sound
	{
		Sound(){m_name="";}
		Sound(char* aname){m_name=aname;}
		virtual ~Sound(){}
		std::string m_name;
	};

/*
	static VArray <FW3_Animation*> animations;
	static VArray <AssimpView::MeshRenderer3*> meshs;
	static VArray <Sound*> sounds;
*/
    virtual int addAnimation(char* afile);
    virtual int addMesh(char* afile);
    virtual int addSound(char* afile, char* aname);
private:
    virtual void deleteAnimation(char* afile);
    virtual void deleteMesh(char* afile);
    virtual void deleteSound(char* afile, char* aname);
public:
    virtual void renderAnimation(int id, int x, int y, int sx, int sy);
    virtual void renderMesh(int id, D3DXFROMWINEMATRIX mvp);
    virtual void playSound(int id);
    virtual void stopSound(int id);


	

	virtual int GetTextureWidth(TextureID id)=0;
	virtual int GetTextureHeight(TextureID id)=0;

	VOID UpdateViewer(
                                                   D3DXFROMWINEVECTOR3 &pos, float* quat);

	virtual void viewport(int x, int y, int viewportWidth, int viewportHeight)=0;

	unsigned int DrawText(unsigned int x, unsigned int y, const char *pszFormat, ...);
	unsigned int DrawSmallText(unsigned int x, unsigned int y, const char *pszFormat, ...);

	D3DXFROMWINEVECTOR3 GetEyePt(){return m_eye;}

	D3DXFROMWINEVECTOR3 GetLookAtPt(){return m_center;}
	D3DXFROMWINEVECTOR3 GetUpPt(){return m_up;}

	float GetYaw(){return m_fYaw;}
	float GetPitch(){return m_fPitch;}
	float GetRoll(){return m_fRoll;}

	D3DXFROMWINEMATRIX Getp(){return *(D3DXFROMWINEMATRIX*)pp;}
	void Setp(const D3DXFROMWINEMATRIX& m){(*(D3DXFROMWINEMATRIX*)pp)=m;}
	D3DXFROMWINEMATRIX Getmv(){return (*(D3DXFROMWINEMATRIX*)vv);}


	D3DXFROMWINEMATRIX Getpmv(){return (*(D3DXFROMWINEMATRIX*)pp)*(*(D3DXFROMWINEMATRIX*)vv);}
	D3DXFROMWINEMATRIX Getmvp(){return (*(D3DXFROMWINEMATRIX*)vv)*(*(D3DXFROMWINEMATRIX*)pp);}

	D3DXFROMWINEMATRIX Getpmvt(){D3DXFROMWINEMATRIX m=(*(D3DXFROMWINEMATRIX*)pp)*(*(D3DXFROMWINEMATRIX*)vv);D3DXFROMWINEMATRIX mo;D3DXFROMWINEMatrixTranspose(&mo,&m);return mo;}
	D3DXFROMWINEMATRIX Getmvpt(){D3DXFROMWINEMATRIX m=(*(D3DXFROMWINEMATRIX*)vv)*(*(D3DXFROMWINEMATRIX*)pp);D3DXFROMWINEMATRIX mo;D3DXFROMWINEMatrixTranspose(&mo,&m);return mo;}

	D3DXFROMWINEMATRIX Getpp();
	D3DXFROMWINEMATRIX Getvv();
	D3DXFROMWINEMATRIX Getww();

	void Setmv(const D3DXFROMWINEMATRIX& m)
	{
		(*(D3DXFROMWINEMATRIX*)vv)=m;
		/*
	mv.index(0) = vRight.x;  m_up.x=mv.index(1);  mv.index(2) = vDir.x;
	mv.index(4) = vRight.y;  m_up.y=mv.index(5);  mv.index(6) = vDir.y;
	mv.index(8) = vRight.z;  m_up.z=mv.index(9);  mv.index(10) = vDir.z;
	*/
	}
	void GetTransform( TS t, D3DXFROMWINEMATRIX& m)
	{
		switch(t)
	{
		case TS_PROJECTION:
			m=(*(D3DXFROMWINEMATRIX*)pp);
			break;
		case TS_VIEW:
			m=(*(D3DXFROMWINEMATRIX*)vv);
			break;
		case TS_WORLD:
			m=(*(D3DXFROMWINEMATRIX*)ww);
			break;
		}
	}
	void SetTransform( TS t, const D3DXFROMWINEMATRIX& m)
	{
		switch(t)
	{
		case TS_PROJECTION:
			(*(D3DXFROMWINEMATRIX*)pp)=m;
			break;
		case TS_VIEW:
			(*(D3DXFROMWINEMATRIX*)vv)=m;
			break;
		case TS_WORLD:
			(*(D3DXFROMWINEMATRIX*)ww)=m;
			break;
		}
	}

	void GetTransform( TS t, D3DXFROMWINEMATRIX* m)
	{
		switch(t)
	{
		case TS_PROJECTION:
			*m=(*(D3DXFROMWINEMATRIX*)pp);
			break;
		case TS_VIEW:
			*m=(*(D3DXFROMWINEMATRIX*)vv);
			break;
		case TS_WORLD:
			*m=(*(D3DXFROMWINEMATRIX*)ww);
			break;
		}
	}
	void SetTransform( TS t, const D3DXFROMWINEMATRIX* m)
	{
		switch(t)
	{
		case TS_PROJECTION:
			(*(D3DXFROMWINEMATRIX*)pp)=*m;
			break;
		case TS_VIEW:
			(*(D3DXFROMWINEMATRIX*)vv)=*m;
			break;
		case TS_WORLD:
			(*(D3DXFROMWINEMATRIX*)ww)=*m;
			break;


		}
	}
	
	virtual void GetRenderTargets(UINT N, TextureID* colorBuffers, TextureID& backBuffer)
	{
	#if 0//def LINUX
	printf("N=%x\n", N);
	printf("backBuffer=%x\n", backBuffer);
	printf("currentDepthRT=%x\n", currentDepthRT);
	#endif
		for(unsigned int i=0;i<N;i++)
		{
			if ((i+1)>MAX_MRTS)
				break;
			colorBuffers[i]=currentColorRT[i];
	#if 0//def LINUX
	printf("colorBuffers[%d]=%x\n", i, colorBuffers[i] );
	printf("currentColorRT[%d]=%x\n", i, currentColorRT[i]);
	#endif
		}
		backBuffer=currentDepthRT;
	}

	D3DFROMWINEVIEWPORT9 GetViewport(){return m_viewport;}
	void SetViewport(D3DFROMWINEVIEWPORT9& vp){m_viewport=vp;}

	int GetViewportWidth(){return m_viewport.Width;}
	int GetViewportHeight(){return m_viewport.Height;}
	int GetViewportX(){return m_viewport.X;}
	int GetViewportY(){return m_viewport.Y;}
#if 0
//	AsyncLoaderID addAsyncLoader( int g_NumProcessingThreads ){return -1;}
//	ResourceReuseCacheID addResourceReuseCache(  ){return -1;}

	//ProcessorID' to 'IDataProcessor *
    HRESULT  AddWorkItem( LoaderID pDataLoader,CAsyncLoader * l, ProcessorID pDataProcessor,
		HRESULT* pHResult, void** ppDeviceObject ){return -1;}
#if 0
    void SetProcessorResourceError(ProcessorID pProcessor);
    void DestroyProcessor(ProcessorID pProcessor);
    void DestroyLoader(ProcessorID pLoader);

	virtual BYTE* GetVertexBufferProcessorData(ProcessorID ID)=0;
	virtual BYTE* GetIndexBufferProcessorData(ProcessorID ID)=0;
	virtual BYTE* GetTextureProcessorData(ProcessorID ID, UINT i)=0;
	virtual UINT GetTextureProcessorPitch(ProcessorID ID, UINT i)=0;
#endif
#if 0
	virtual LoaderID addTextureLoader( CHAR* szFileName, CPackedFile* g_PackFile );
    virtual ProcessorID addTextureProcessor( TextureID& ppTexture, int g_SkipMips )=0;
    virtual HRESULT LoadTexture(LoaderID pLoader);
    virtual HRESULT DecompressTexture(LoaderID	pLoader, void** pLocalData, SIZE_T* Bytes );
    virtual HRESULT ProcessTexture( ProcessorID pProcessor,void*pLocalData, SIZE_T Bytes )=0;
    virtual HRESULT LockTextureDeviceObject(ProcessorID pProcessor)=0;
    virtual HRESULT CopyToResourceTexture(ProcessorID pProcessor)=0;
    virtual HRESULT UnLockTextureDeviceObject(ProcessorID pProcessor)=0;

    virtual LoaderID addVertexBufferLoader();
    virtual ProcessorID addVertexBufferProcessor( VertexBufferID& ppBuffer, int iSizeBytes,   void* pData)=0;
    virtual HRESULT LoadVertexBuffer(LoaderID pLoader);
    virtual HRESULT DecompressVertexBuffer(LoaderID pLoader, void**pLocalData, SIZE_T* Bytes );
    virtual HRESULT ProcessVertexBuffer( ProcessorID pProcessor,void*pLocalData, SIZE_T Bytes )=0;
    virtual HRESULT LockVertexBufferDeviceObject(ProcessorID pProcessor)=0;
    virtual HRESULT CopyToResourceVertexBuffer(ProcessorID pProcessor)=0;
    virtual HRESULT UnLockVertexBufferDeviceObject(ProcessorID pProcessor)=0;

    virtual LoaderID addIndexBufferLoader();
    virtual ProcessorID addIndexBufferProcessor( IndexBufferID& ppBuffer, int iSizeBytes, void* pData)=0;
    virtual HRESULT LoadIndexBuffer(LoaderID pLoader);
    virtual HRESULT DecompressIndexBuffer(LoaderID pLoader, void**pLocalData, SIZE_T*Bytes );
    virtual HRESULT ProcessIndexBuffer(ProcessorID pProcessor, void*pLocalData, SIZE_T Bytes )=0;
    virtual HRESULT LockIndexBufferDeviceObject(ProcessorID pProcessor)=0;
    virtual HRESULT CopyToResourceIndexBuffer(ProcessorID pProcessor)=0;
    virtual HRESULT UnLockIndexBufferDeviceObject(ProcessorID pProcessor)=0;
#endif
#endif
	virtual UINT32 GetAvailableTextureMem()=0;
	 D3DXFROMWINEMATRIX identity(){ D3DXFROMWINEMATRIX _identity; D3DXFROMWINEMatrixIdentity(&_identity);return _identity;}//???
private:
	virtual int removeTexture(const TextureID texture) = 0;
		int deleteTexture(const TextureID texture){removeTexture(texture); return -1;}
		virtual int deleteVertexBuffer(const VertexBufferID vertexBuffer)=0;
		virtual int deleteIndexBuffer(const IndexBufferID indexBuffer)=0;
public:
		virtual BlendStateID getBlendStateVrtl(const TextureID texture)
			=0;

inline BlendStateID getBlendState(const TextureID texture){return getBlendStateVrtl(texture);}

		virtual int texturePitchVrtl(const TextureID texture,  int level)
			=0;

inline int texturePitch(const TextureID texture, int level=0){return texturePitchVrtl(texture, level);}
			virtual BYTE* lockTexture(const TextureID texture, int level, int& pitch)=0;
		virtual BYTE* lockVertexBuffer(const VertexBufferID vertexBuffer)=0;
		virtual BYTE* lockIndexBuffer(const IndexBufferID indexBuffer)=0;
			virtual BYTE* unlockTextureVrtl(const TextureID texture,  int level)=0;

inline BYTE* unlockTexture(const TextureID texture, int level=0){return unlockTextureVrtl(texture, level);}
		virtual BYTE* unlockVertexBuffer(const VertexBufferID vertexBuffer)=0;
		virtual BYTE* unlockIndexBuffer(const IndexBufferID indexBuffer)=0;

virtual float getTextWidth(const FontID font, const char *str, int length = -1) const=0;

unsigned int drawPlain(const Primitives primitives, D3DXFROMWINEVECTOR2 *vertices, const unsigned int nVertices, const BlendStateID blendState, const DepthStateID depthState, const D3DXFROMWINEVECTOR4 *color);
unsigned int drawTextureVrtl(TextureID texture, int x=0, int y=0, int sx=600, int sy=600, bool InScene=true, float tx=0.0f, float ty=0.0f, float stx=1.0f, float sty=1.0f);

	D3DXFROMWINEMATRIX GetProjMatrix(){return (*(D3DXFROMWINEMATRIX*)pp);}
		D3DXFROMWINEMATRIX GetViewMatrix(){return (*(D3DXFROMWINEMATRIX*)vv);}
	virtual void elevate(float add)
		{
buildLookAtMatrix(m_eye.x, m_eye.y+add, m_eye.z,
                              m_center.x, m_center.y, m_center.z,
                              m_up.x, m_up.y, m_up.z);
}
	virtual void buildLookAtMatrix(D3DXFROMWINEVECTOR3 at, IInput* Input=0)
	;

	virtual void buildLookAtMatrix(D3DXFROMWINEVECTOR3 eye,
			      D3DXFROMWINEVECTOR3 center,
			      D3DXFROMWINEVECTOR3 up//,D3DXFROMWINEMATRIX& m
			      )
				  {
	buildLookAtMatrix(eye.x, eye.y, eye.z,
			      center.x, center.y, center.z,
			      up.x, up.y, up.z//,m
			      );
				  }

	virtual void buildLookAtMatrix(D3DXFROMWINEVECTOR4 eye,
			      D3DXFROMWINEVECTOR4 center,
			      D3DXFROMWINEVECTOR4 up//,D3DXFROMWINEMATRIX& m
			      )
				  {
	buildLookAtMatrix(eye.x, eye.y, eye.z,
			      center.x, center.y, center.z,
			      up.x, up.y, up.z//,m
			      );
				  }
#if 0


	virtual void buildLookAtMatrix(float eyex, float eyey, float eyez,
			      float centerx, float centery, float centerz,
			      float upx, float upy, float upz
			      )
							  {
buildLookAtMatrix(eyex, eyey, eyez,
			      centerx, centery, centerz,
			      upx, upy, upz,mv);
							  }

virtual void buildPerspectiveMatrix(float fieldOfView,
				   float aspectRatio,
				   float zNear, float zFar)
								   {
buildPerspectiveMatrix(fieldOfView,
				   aspectRatio,
				   zNear, zFar,p

				   );
}
#endif

	virtual void buildLookAtMatrix(float eyex, float eyey, float eyez,
			      float centerx, float centery, float centerz,
			      float upx, float upy, float upz//,D3DXFROMWINEMATRIX& m
			      )=0;
				  virtual void buildPerspectiveMatrix(float fieldOfView,
				   float aspectRatio,
				   float zNear, float zFar//,D3DXFROMWINEMATRIX& m
				   )=0;

	void updateClipPlanes(float zNear, float zFar)
	{
		buildPerspectiveMatrix(this->m_fieldOfView,
				   this->m_aspectRatio,
				   zNear, zFar
				   );
	}
#if 0
	static void PushmvMatrix()//D3DXFROMWINEMATRIX worldTransform=identity4())
	{
	   s_mvV.push_back(mv);/*
	  mv=mv*worldTransform;
IRenderer::GetRendererInstance()->setShaderConstant4x4f( "worldViewProj",
										   IRenderer::GetRendererInstance()->p()*IRenderer::GetRendererInstance()->mv());
*/}
	static void PopmvMatrix()
	{
		mv=s_mvV[s_mvV.size()-1];
		s_mvV.pop_back();
	}
	void PushpMatrix()
	{
	   s_pV.push_back(p);
	}
	void PoppMatrix()
	{
		p=s_pV[s_pV.size()-1];
		s_pV.pop_back();
	}
	static void MultmvMatrix(D3DXFROMWINEMATRIX w)
	{
		mv=mv*w;
	}
	void MultpMatrix(D3DXFROMWINEMATRIX w)
	{
		p=p*w;
	}
#endif
	virtual bool supportsR2VB(){return overrideR2VB&&bsupportsR2VB;}
	virtual bool supportsHalf(){return overrideHalf&&bsupportsHalf;}
	bool overrideR2VB,overrideHalf;
	bool bsupportsR2VB,bsupportsHalf;
	unsigned int drawFPS();
	bool debugRuntime()
	{
	// Detect whether the debug runtime is active
	bool debugRuntime = false;
#if 0//defined(_WIN32) && !defined(__MINGW32__) && !defined(__MINGW64__)
	HKEY hKey;
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Direct3D", 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS){
		__DWORD__ size = sizeof(__WORD__);
		__DWORD__ value;
		if (RegQueryValueExA(hKey, "LoadDebugRuntime", 0, 0, (LPBYTE) &value, &size) == ERROR_SUCCESS){
			debugRuntime = (value != 0);
		}
		RegCloseKey(hKey);
	}
#endif
	return debugRuntime;
	}
	virtual void setTextureSlice(const char *textureName, const TextureID texture, const int slice) = 0;


		virtual void Init0()=0;
unsigned int getIndicesCount(const Primitives primitives);
unsigned int getPrimitiveCount(const Primitives primitives, const int count);
unsigned int getPrimitiveCountR(const Primitives primitives, const int count);
unsigned int getVertexCount(const Primitives primitives, const int  NumPrim);
	int WorkDirectory(int argc=0, char *argv[]=0);
	RendererHLSLCg(int argc=0, char *argv[]=0);
	virtual ~RendererHLSLCg();

	void debug(void);
	static void OpenILToGIF(Image3& img, const char* gifbuf);

	virtual D3DXFROMWINEVECTOR4& GetscaleBias2D();
void setViewportVrtl(int x, int y, int viewportWidth, int viewportHeight, float minZ=0.0f, float maxZ=1.0f)
{
  m_viewport.X=x;
  m_viewport.Y=y;
  m_viewport.Width=viewportWidth;
  m_viewport.Height=viewportHeight;
  m_viewport.MinZ=minZ;
  m_viewport.MaxZ=maxZ;
  width=viewportWidth;
  height=viewportHeight;
}
	void apply();

	virtual stx_Effect* addEffectFromFile(const std::string& aFileName, const char* aDefines=0);

	int getFormatSize(const AttributeFormat format) const;
	virtual VertexFormatID addVertexFormatVrtl(const FormatDesc *formatDesc,  const unsigned int nAttribs,  const ShaderID shader ) = 0;

inline VertexFormatID addVertexFormat(const FormatDesc *formatDesc, const unsigned int nAttribs, const ShaderID shader = SHADER_NONE){return addVertexFormatVrtl(formatDesc, nAttribs, shader);}

	virtual VertexBufferID addVertexBufferVrtl(const int size,  const BufferAccess bufferAccess,  const void *data ,  VertexBufferID old)=0;

inline VertexBufferID addVertexBuffer(const int size, const BufferAccess bufferAccess, const void *data = 0, VertexBufferID old=-1){return addVertexBufferVrtl(size, bufferAccess, data, old);}//, const IDataProcessor* p=0) = 0;
	virtual IndexBufferID addIndexBufferVrtl(const unsigned int nIndices,  const unsigned int indexSize,  const BufferAccess bufferAccess,  const void *data ,  IndexBufferID old)=0;

inline IndexBufferID addIndexBuffer(const unsigned int nIndices, const unsigned int indexSize, const BufferAccess bufferAccess, const void *data = 0, IndexBufferID old=-1){return addIndexBufferVrtl(nIndices, indexSize, bufferAccess, data, old);}//, const IDataProcessor* p=0)  = 0;
		TextureID addTextureVrtl(int Width, int Height, bool MipLevels, FORMAT format, const SamplerStateID samplerState = SS_NONE,const unsigned int flags = 0);/*
		{
			//int bpp=getBytesPerPixel(format);
			//int is=getImageSize(format, Width, Height, 1, MipLevels);
			Image33* img;
			//int size = getMipMappedSize(0, MipLevelss);
			img->create(format,Width, Height, 1, MipLevels);
				//.setPixels( new ubyte[is]);
			return addTexture(&img,MipLevels,samplerState,flags);
		}

*/
	virtual SamplerStateID addSamplerStateVrtl(const Filter filter,  const AddressMode s,  const AddressMode t,  const AddressMode r,  const float lod ,  const unsigned int maxAniso ,  const int compareFunc ,  const float *border_color ) = 0;

inline SamplerStateID addSamplerState(const Filter filter, const AddressMode s, const AddressMode t, const AddressMode r, const float lod = 0, const unsigned int maxAniso = 16, const int compareFunc = 0, const float *border_color = 0){return addSamplerStateVrtl(filter, s, t, r, lod, maxAniso, compareFunc, border_color);}

    virtual AlphaStateID addAlphaState(
		const int func=GEQUAL,
		const float ref=0.5f
		)=0;

	BlendStateID addBlendState(
		const int srcFactor,
		const int destFactor,
		const int blendMode = BM_ADD,
		const int mask = ALL,
		const bool alphaToCoverage = false){
		return addBlendState(srcFactor, destFactor, srcFactor, destFactor, blendMode, blendMode, mask, alphaToCoverage);
	}
	virtual BlendStateID addBlendState(
		const int srcFactorRGB,
		const int destFactorRGB,
		const int srcFactorAlpha,
		const int destFactorAlpha,
		const int blendModeRGB,
		const int blendModeAlpha,
		const int mask = ALL,
		const bool alphaToCoverage = false) = 0;
	virtual DepthStateID addDepthState(const bool depthTest, const bool depthWrite, const int depthFunc = IRenderer::LEQUAL, const bool stencilTest = false,
		const uint8 stencilMask = 0xFF, const int stencilFunc = IRenderer::ALWAYS, const int stencilFail = IRenderer::KEEP, const int depthFail = IRenderer::KEEP, const int stencilPass = IRenderer::KEEP) = 0;
	virtual RasterizerStateID addRasterizerStateVrtl(const int cullMode,  const int fillMode ,  const bool multiSample ,  const bool scissor ) = 0;

inline RasterizerStateID addRasterizerState(const int cullMode, const int fillMode = IRenderer::SOLID, const bool multiSample = true, const bool scissor = false){return addRasterizerStateVrtl(cullMode, fillMode, multiSample, scissor);}

	static FontID addFont(const char *textureFile, const char *fontFile, const SamplerStateID samplerState);

	virtual unsigned int DrawPrimitive(Primitives PrimitiveType,
		UINT StartVertex,
		UINT PrimitiveCount, const unsigned int flags=RESET_ALL)=0;
    virtual unsigned int DrawIndexedPrimitive(Primitives PrimitiveType,
		INT BaseVertexIndex,
		UINT MinVertexIndex,
		UINT NumVertices,
		UINT startIndex,
		UINT primCount, const unsigned int flags=RESET_ALL)=0;
    unsigned int DrawPrimitiveUP(Primitives PrimitiveType,
		UINT PrimitiveCount,
		CONST void* pVertexStreamZeroData0,
		CONST void* pVertexStreamZeroData,
		UINT VertexStreamZeroStride, const unsigned int flags=RESET_ALL);

    unsigned int DrawIndexedPrimitiveUPVrtl(Primitives PrimitiveType,
		UINT MinVertexIndex,
		UINT NumVertices,

		UINT PrimitiveCount,
		CONST void* pIndexData0,
		CONST void* pIndexData,
		STX_INDEXFORMAT IndexDataFormat,
		CONST void* pVertexStreamZeroData0,
		CONST void* pVertexStreamZeroData,
		UINT VertexStreamZeroStride, const unsigned int flags=RESET_ALL);



    virtual void BeginSceneVrtl(bool aB)=0;

inline void BeginScene(bool aB=false){BeginSceneVrtl(aB);}
    virtual void EndSceneVrtl(bool aB)=0;

inline void EndScene(bool aB=false){EndSceneVrtl(aB);}
	virtual void Clear(bool color, bool depth, D3DXFROMWINEVECTOR4 colors)=0;
    virtual void PresentVrtl(bool aB)=0;

inline void Present(bool aB=false){PresentVrtl(aB);}


	virtual void reset0ToDefaults();
	virtual void reset0(const unsigned int flags = RESET_ALL);


virtual void RenderTexVrtl(TextureID id, D3DXFROMWINEVECTOR2 aposition, D3DXFROMWINEVECTOR2 asize, D3DXFROMWINEVECTOR2 texposition, D3DXFROMWINEVECTOR2 texsize);

    unsigned int drawTexturedVrtl(const Primitives primitives, TexVertex *vertices, const unsigned int nVertices, const TextureID texture, const BlendStateID blendState, const DepthStateID depthState, const D3DXFROMWINEVECTOR4 *color = 0);


	virtual void applyTextures() = 0;

	virtual void setSamplerState(const char *samplerName, const SamplerStateID samplerState) = 0;
	virtual void applySamplerStates() = 0;

	virtual void applyConstants() = 0;

	void setShader(const ShaderID shader){
		//???//assert(shader>=0);
//LOG_FNLN;
		if(shader<0)
			return;
		STX_FNLN;
		STX_PRINT("selectedShader:%x\n",selectedShader);
		STX_PRINT("shader:%x\n",shader);
		selectedShader = shader;
		//selectedShader = 0; // ????
#if 0
		setShaderFlag(RESET_SHADER);
#endif
#ifdef __X7__
		changeShader(shader);
#endif
		STX_FNLN;
		STX_PRINT("selectedShader:%x\n",selectedShader);
	}

	void setPixelShader(const ShaderID shader){selectedPixelShader = shader;}
	void setVertexShader(const ShaderID shader){selectedVertexShader = shader;}
	void setGeometryShader(const ShaderID shader){selectedGeometryShader = shader;}
	void setHullShader(const ShaderID shader){selectedHullShader = shader;}
	void setDomainShader(const ShaderID shader){selectedDomainShader = shader;}
	void setComputeShader(const ShaderID shader){selectedComputeShader = shader;}

	void setVertexFormat(const VertexFormatID vertexFormat){
//???		//assert(vertexFormat>=0);
		if(vertexFormat<0)
			return;
		selectedVertexFormat = vertexFormat;
#if 0
		setVertexFormatFlag(RESET_VF);
#endif
		STX_FNLN;
		STX_PRINT("selectedVertexFormat:%x\n",selectedVertexFormat);
#ifdef __X7__
		changeVertexFormat(vertexFormat);
#endif
		STX_FNLN;
		STX_PRINT("selectedVertexFormat:%x\n",selectedVertexFormat);
	}
	void setVertexBufferVrtl(const int stream, const VertexBufferID vertexBuffer, const intptr offset = 0){

//???			if(vertexBuffer==-1) DBG_HALT;
//???		//assert(vertexBuffer>=0);
		selectedVertexBuffers[stream] = vertexBuffer;
		selectedOffsets[stream] = offset;
#if 0
		setVertexBufferFlag(RESET_VB);
#endif
#ifdef __X7__
		changeVertexBuffer(stream, vertexBuffer, offset);
#endif
	}
	void setVertexBuffer(const int stream, const void *base){
	//	DBG_HALT;
		selectedVertexBuffers[stream] = VB_NONE;
		selectedOffsets[stream] = (intptr) base;
#if 0
		setVertexBufferFlag(RESET_VB);
#endif
#ifdef __X7__
		changeVertexBuffer(stream, VB_NONE, (intptr) base);
#endif
	}
	void setIndexBuffer(const IndexBufferID indexBuffer){
		//assert(indexBuffer>=0);
		selectedIndexBuffer = indexBuffer;
#if 0
		setIndexBufferFlag(RESET_IB);
#endif
#ifdef __X7__
		changeIndexBuffer(indexBuffer);
#endif
	}

/*	void setSamplerState(const unsigned int samplerUnit, const SamplerStateID samplerState){
		selectedSamplerStates[samplerUnit] = samplerState;
	}*/
	void setBlendStateVrtl(const BlendStateID blendState, const unsigned int sampleMask = ~0){
		//assert(blendState>=0);
		selectedBlendState = blendState;
		selectedSampleMask = sampleMask;
#if 0
		setBlendStateFlag(RESET_BS);
#endif
#ifdef __X7__
		changeBlendState(blendState, sampleMask);
#endif
	}
	void setAlphaState(const AlphaStateID alphaState){

		//assert(alphaState>=0);
		selectedAlphaState = alphaState;
#if 0
		setAlphaStateFlag(RESET_AS);
#endif
#ifdef __X7__
		changeAlphaState(alphaState);
#endif
	}
	void setDepthStateVrtl(const DepthStateID depthState, const unsigned int stencilRef = 0){
		//assert(depthState>=0);
		selectedDepthState = depthState;
		selectedStencilRef = stencilRef;
#if 0
		setDepthStateFlag(RESET_DS);
#endif
#ifdef __X7__
		changeDepthState(depthState);
#endif
	}
	void setRasterizerState(const RasterizerStateID rasterizerState){
		//assert(rasterizerState>=0);
		selectedRasterizerState = rasterizerState;
#if 0
		setRasterizerStateFlag(RESET_RS);
#endif
#ifdef __X7__
		changeRasterizerState(rasterizerState);
#endif
	}

	void setShaderConstant1i(const char *name, const int constant);
	void setShaderConstant1f(const char *name, const float constant);

	void setShaderConstant2f(const char *name, const D3DXFROMWINEVECTOR2 &constant);
	void setShaderConstant3f(const char *name, const D3DXFROMWINEVECTOR3 &constant);
	virtual void setShaderConstant4f(const char *name, const D3DXFROMWINEVECTOR4 &constant);
	virtual void setShaderConstant4x4f(const char *name, const D3DXFROMWINEMATRIX &constant);

	

	void setShaderConstantArray1f(const char *name, const float *constant, const unsigned int count);

	void setShaderConstantArray2f(const char *name, const D3DXFROMWINEVECTOR2  *constant, const unsigned int count);
	void setShaderConstantArray3f(const char *name, const D3DXFROMWINEVECTOR3  *constant, const unsigned int count);
	virtual void setShaderConstantArray4f(const char *name, const D3DXFROMWINEVECTOR4  *constant, const unsigned int count);
	virtual void setShaderConstantArray4x4f(const char *name, const D3DXFROMWINEMATRIX *constant, const unsigned int count);


	virtual void setShaderConstantRaw(const char *name, const void *data, const int size) = 0;

	void changeRenderTargetVrtl(const TextureID colorRT, const TextureID depthRT = TEXTURE_NONE, const int slice = NO_SLICE){
		changeRenderTargets(&colorRT, 1, depthRT, &slice);
	}
	void changeRenderTargets(const TextureID *colorRTs, const unsigned int nRenderTargets, const TextureID depthRT, const int *faces){
		changeRenderTargetsVrtl(colorRTs, nRenderTargets, depthRT, NO_SLICE, faces);
	}
	virtual void changeRenderTargetsVrtl(const TextureID *colorRTs,  const unsigned int nRenderTargets,  const TextureID depthRT ,  const int depthSlice ,  const int *slices ) = 0;

inline void changeRenderTargets(const TextureID *colorRTs, const unsigned int nRenderTargets, const TextureID depthRT = TEXTURE_NONE, const int depthSlice = NO_SLICE, const int *slices = 0){changeRenderTargetsVrtl(colorRTs, nRenderTargets, depthRT, depthSlice, slices);}
	virtual void changeToMainFramebuffer() = 0;

	virtual TextureID addTexture(Image3* img, const bool useMipMaps=false, 
		const SamplerStateID samplerState = SS_NONE, unsigned int flags = 0
	
	) = 0;
	static TextureID addTexture(const char *fileName, const bool useMipMaps, const SamplerStateID samplerState = SS_NONE, unsigned int flags = 0);
	//virtual TextureID addCubemapVrtl(const char **fileNames, const bool useMipMaps, const SamplerStateID samplerState, const int nArraySlices, unsigned int flags);
	static TextureID addNormalMap(const char *fileName, const FORMAT destFormat, const bool useMipMaps, const SamplerStateID samplerState = SS_NONE, float sZ = 1.0f, float mipMapScaleZ = 2.0f, unsigned int flags = 0);

	TextureID addRenderTargetVrtl(const int width, const int height, const FORMAT format, const SamplerStateID samplerState = SS_NONE, unsigned int flags = 0){
		return addRenderTarget(width, height, 1, 1, 1, format, 1, samplerState, flags);
	}
	virtual TextureID addRenderTargetVrtl(const int width,  const int height,  const int depth,  const int mipMapCount,  const int arraySize,  const FORMAT format,  const int msaaSamples ,  const SamplerStateID samplerState ,  unsigned int flags ) = 0;

inline TextureID addRenderTarget(const int width, const int height, const int depth, const int mipMapCount, const int arraySize, const FORMAT format, const int msaaSamples = 1, const SamplerStateID samplerState = SS_NONE, unsigned int flags = 0){return addRenderTargetVrtl(width, height, depth, mipMapCount, arraySize, format, msaaSamples, samplerState, flags);}
	TextureID addRenderDepth(const int width, const int height, const int depthBits);
	virtual TextureID addRenderDepthVrtl(const int width,  const int height,  const int arraySize,  const FORMAT format,  const int msaaSamples ,  const SamplerStateID samplerState ,  unsigned int flags ) = 0;

inline TextureID addRenderDepth(const int width, const int height, const int arraySize, const FORMAT format, const int msaaSamples = 1, const SamplerStateID samplerState = SS_NONE, unsigned int flags = 0){return addRenderDepthVrtl(width, height, arraySize, format, msaaSamples, samplerState, flags);}

	virtual bool resizeRenderTarget(const TextureID renderTarget, const int width, const int height, const int depth, const int mipMapCount, const int arraySize) = 0;
	virtual bool generateMipMaps(const TextureID renderTarget) = 0;



	virtual void changeShader(const ShaderID shader) = 0;
#if 0
	virtual void changePixelShader(const ShaderID shader) = 0;
	virtual void changeVertexShader(const ShaderID shader) = 0;
	virtual void changeGeometryShader(const ShaderID shader) = 0;
	virtual void changeHullShader(const ShaderID shader) = 0;
	virtual void changeDomainShader(const ShaderID shader) = 0;
	virtual void changeComputeShader(const ShaderID shader) = 0;
#endif
	virtual void changeVertexFormat(const VertexFormatID vertexFormat) = 0;
	virtual void changeVertexBufferVrtl(const int stream,  const VertexBufferID vertexBuffer,  const intptr offset ) = 0;

inline void changeVertexBuffer(const int stream, const VertexBufferID vertexBuffer, const intptr offset = 0){changeVertexBufferVrtl(stream, vertexBuffer, offset);}
	void changeVertexBuffer(const void *base){
		changeVertexBuffer(0, VB_NONE, (intptr) base);
	}
	virtual void changeIndexBuffer(const IndexBufferID indexBuffer) = 0;

//	virtual void changeSamplerState(const unsigned int samplerUnit, const SamplerStateID samplerState) = 0;
	virtual void changeBlendStateVrtl(const BlendStateID blendState,  const unsigned int sampleMask ) = 0;

inline void changeBlendState(const BlendStateID blendState, const unsigned int sampleMask = ~0){changeBlendStateVrtl(blendState, sampleMask);}
	virtual void changeAlphaState(const AlphaStateID alphaState)=0;
	virtual void changeDepthStateVrtl(const DepthStateID depthState,  const unsigned int stencilRef ) = 0;

inline void changeDepthState(const DepthStateID depthState, const unsigned int stencilRef = 0){changeDepthStateVrtl(depthState, stencilRef);}
	virtual void changeRasterizerState(const RasterizerStateID rasterizerState) = 0;

    virtual unsigned int DrawFPSVrtl(int x=10, int y=10);

	virtual void setup2DMode(const float left, const float right, const float top, const float bottom)=0;

    virtual void SaveScreenShotVrtl(const char* fn)=0;

inline void SaveScreenShot(const char* fn=0){SaveScreenShotVrtl(fn);}
	virtual void captureScreenshot(Image3& img)=0;
float getStringWidth(FontID font,const char* str, const float charWidth);

	void fillTextBuffer( TexVertex *dest, const char *str, float x, float y, const float charWidth, const float charHeight, const FontID font);
	unsigned int drawText(const char *str, float x, float y, const float charWidth, const float charHeight, const FontID& font, const SamplerStateID samplerState, const BlendStateID blendState, const DepthStateID depthState, const D3DXFROMWINEVECTOR4 col=D3DXFROMWINEVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

    unsigned int getTextQuads(const char *str) const;

    int Getlinear(){return linear;}
    int GetlinearClamp(){return linearClamp;}
    int GetblendSrcAlpha(){return blendSrcAlpha;}
    int GetnoDepthTest(){return noDepthTest;}

	virtual int GetlinearAniso(){return linearAniso;}
    virtual int GetFILTER_LINEAR_ANISO(){return FILTER_LINEAR_ANISO;}
    virtual int Getbilinear(){return bilinear;}
    virtual int GetbilinearAniso(){return bilinearAniso;}
    virtual int GetFILTER_BILINEAR_ANISO(){return FILTER_BILINEAR_ANISO;}
    virtual int Gettrilinear(){return trilinear;}
    virtual int GettrilinearAniso(){return trilinearAniso;}
    virtual int GetFILTER_TRILINEAR_ANISO(){return FILTER_TRILINEAR_ANISO;}
    virtual int Getnearest(){return nearest;}
    virtual int GetnearestClamp(){return nearest;}
    virtual int GetbilinearClamp(){return bilinearClamp;}
    virtual int GettrilinearClamp(){return trilinearClamp;}
    virtual int GetenableAlpha(){return enableAlpha;}
    virtual int GetdisableAlpha(){return disableAlpha;}
    virtual int GetblendOneOne(){return blendOneOne;}
    virtual int GetblendOneOneReverseSubstract(){return blendOneOneReverseSubstract;}
    virtual int GetnoDepthWrite(){return noDepthWrite;}
    virtual int GetcullNone(){return cullNone;}
    virtual int GetcullBack(){return cullBack;}
    virtual int GetcullFront(){return cullFront;}
    virtual int GetfillSolid(){return fillSolid;}
    virtual int GetfillWireFrame(){return fillWireFrame;}





	/*
	virtual void Init(void)=0;

	CGcontext m_CgContext;
	UINT viewportWidth;
	UINT viewportHeight;
*/
protected:

	float time, frameTime;
public:
	SamplerStateID nearest,nearestClamp,linearClamp,bilinearClamp,trilinearClamp;
union {
			SamplerStateID linear;
			SamplerStateID linearAniso;
			SamplerStateID FILTER_LINEAR_ANISO;
		};
		union {
			SamplerStateID bilinear;
			SamplerStateID bilinearAniso;
			SamplerStateID FILTER_BILINEAR_ANISO;
		};
		union {
			SamplerStateID trilinear;
			SamplerStateID trilinearAniso;
			SamplerStateID FILTER_TRILINEAR_ANISO;
		};
	ShaderID& GetplainShader(){return plainShader;}
	VertexFormatID& GetplainVF(){return plainVF;}
	ShaderID& GetplainShader3(){return plainShader3;}
	VertexFormatID& GetplainVF3(){return plainVF3;}
	ShaderID& GettexShader(){return texShader;}
	VertexFormatID& GettexVF(){return texVF;}
	ShaderID& GettexShader3(){return texShader3;}
	VertexFormatID& GettexVF3(){return texVF3;}
	virtual ShaderID GetCurrentShader(){return currentShader;}
	virtual ShaderID GetSelectedShader(){return selectedShader;}

	virtual VertexFormatID GetCurrentVertexFormat(){return currentVertexFormat;}
	virtual VertexFormatID GetSelectedVertexFormat(){return selectedVertexFormat;}
	virtual IndexBufferID GetCurrentIndexBuffer(){return currentIndexBuffer;}
	virtual IndexBufferID GetSelectedIndexBuffer(){return selectedIndexBuffer;}

	virtual DepthStateID GetCurrentDepthState(){return currentDepthState;}
	virtual DepthStateID GetSelectedDepthState(){return selectedDepthState;}
	virtual BlendStateID GetCurrentBlendState(){return currentBlendState;}
	virtual BlendStateID GetSelectedBlendState(){return selectedBlendState;}
	virtual AlphaStateID GetCurrentAlphaState(){return currentAlphaState;}
	virtual AlphaStateID GetSelectedAlphaState(){return selectedAlphaState;}
	virtual RasterizerStateID GetCurrentRasterizerState(){return currentRasterizerState;}
	virtual RasterizerStateID GetSelectedRasterizerState(){return selectedRasterizerState;}

protected:
#if 0
	//VArray <TexFont*> fonts;
	VArray <Texture*> textures;
	VArray <Shader*> shaders;
	VArray <VertexBuffer*> vertexBuffers;
	VArray <IndexBuffer*> indexBuffers;
	VArray <VertexFormat*> vertexFormats;
	VArray <SamplerState*> samplerStates;
	VArray <BlendState*> blendStates;
	VArray <DepthState*> depthStates;
	VArray <RasterizerState*> rasterizerStates;
#endif
	unsigned int nImageUnits, nMRTs;
	int maxAnisotropic;

//	float *textureLod;

	ShaderID currentShader, selectedShader;
	ShaderID currentPixelShader, selectedPixelShader;
	ShaderID currentVertexShader, selectedVertexShader;
	ShaderID currentGeometryShader, selectedGeometryShader;
	ShaderID currentHullShader, selectedHullShader;
	ShaderID currentDomainShader, selectedDomainShader;
	ShaderID currentComputeShader, selectedComputeShader;

	VertexFormatID currentVertexFormat, selectedVertexFormat;
	VertexBufferID currentVertexBuffers[MAX_VERTEXSTREAM], selectedVertexBuffers[MAX_VERTEXSTREAM];
	intptr selectedOffsets[MAX_VERTEXSTREAM], currentOffsets[MAX_VERTEXSTREAM];
	IndexBufferID currentIndexBuffer, selectedIndexBuffer;

	DepthStateID currentDepthState, selectedDepthState;
	unsigned int currentStencilRef, selectedStencilRef;
	BlendStateID currentBlendState, selectedBlendState;
	AlphaStateID currentAlphaState, selectedAlphaState;
	unsigned int currentSampleMask, selectedSampleMask;
	RasterizerStateID currentRasterizerState, selectedRasterizerState;

	TextureID currentColorRT[MAX_MRTS], currentDepthRT;
	int currentColorRTSlice[MAX_MRTS], currentDepthSlice;
	unsigned int nCurrentRenderTargets;

        TextureID currentTextures[MAX_TEXTUREUNIT], selectedTextures[MAX_TEXTUREUNIT];
        SamplerStateID currentSamplerStates[MAX_SAMPLERSTATE], selectedSamplerStates[MAX_SAMPLERSTATE];
        float textureLod[MAX_TEXTUREUNIT];

        VertexFormatID activeVertexFormat[MAX_VERTEXSTREAM];
        int currentMask;
        bool currentBlendEnable;
        bool currentAlphaEnable;
        bool currentAlphaToCoverageEnable;

        int currentDepthFunc;
        bool currentDepthTestEnable;
        bool currentDepthWriteEnable;

        bool currentMultiSampleEnable;
        bool currentScissorEnable;
        int currentCullMode;
        int currentFillMode;

	BlendStateID blendSrcAlpha,blendOneOne,blendOneOneReverseSubstract;

	AlphaStateID enableAlpha,disableAlpha;

	DepthStateID noDepthTest, noDepthWrite;
	//DepthStateID depthTest, depthWrite;
		RasterizerStateID cullNone, cullBack, cullFront,fillSolid,fillWireFrame;
	ShaderID plainShader, texShader;
	VertexFormatID plainVF, texVF;
	ShaderID plainShader3, texShader3;
	VertexFormatID plainVF3, texVF3;

	D3DXFROMWINEVECTOR4 scaleBias2D;
public:
#if 0
	static int viewportWidth;
	static int viewportHeight;
	static int viewportX;
	static int viewportY;
#else
//static VArray <D3DFROMWINEVIEWPORT9> Viewports;
	static D3DFROMWINEVIEWPORT9 m_viewport;
#endif
protected:
	// Statistics counters
	unsigned int nDrawCalls;
private:
	std::vector<TexVertex> fontBuffer;
	unsigned int fontBufferCount;
#ifdef DEBUG
	bool wasReset;
#endif
};
#ifndef SWIG
#ifdef INCLUDEMESH
#ifndef COLORVALUE_DEFINED
typedef struct _COLORVALUE {
	_COLORVALUE()
	{
		r=0.0f;
		g=0.0f;
		b=0.0f;
		a=0.0f;
	}
	virtual ~_COLORVALUE(){}
    float r;
    float g;
    float b;
    float a;
} COLORVALUE;
#define COLORVALUE_DEFINED
#endif
typedef struct _MATERIAL {
	_MATERIAL()
	{
		Power=0.0f;
	}
	virtual ~_MATERIAL(){}
    COLORVALUE	 Diffuse;	 /* Diffuse color RGBA */
    COLORVALUE	 Ambient;	 /* Ambient color RGB */
    COLORVALUE	 Specular;	 /* Specular 'shininess' */
    COLORVALUE	 Emissive;	 /* Emissive color RGB */
    float	    Power;	    /* Sharpness if specular highlight */
} MATERIAL;
struct SUBSET
{
	SUBSET()
	{
		m_Material=MATERIAL();
		m_TextureFileName="";
		m_dNumVertices=0;
		m_dNumFaces=0;
		m_pIndices.resize(0);
		m_pVertices.resize(0);
	}
	virtual ~SUBSET(){}
	MATERIAL  m_Material;
	std::string   m_TextureFileName;
	__DWORD__	      m_dNumVertices;
	__DWORD__	      m_dNumFaces;
	std::vector<__WORD__> m_pIndices;
	std::vector<stVertex2>	  m_pVertices;
};

struct MESH {
	std::vector<SUBSET> m_Subsets;
	MESH()
	{
		m_dNumFaces=0;
		m_dNumVertices=0;
		vf=-1;
		//vid=-1;
		//iid=-1;
		m_pIndices.resize(0);
		m_pVertices.resize(0);
		vertexStride=0;
	}
    virtual void GetMesh(MESH** pMesh)=0;
    virtual bool Create(const char* szFileName )=0;
	virtual ~MESH()
	{
		m_pIndices.clear();
		m_pVertices.clear();
	}
	unsigned int DrawSubset(__DWORD__ n)
	{
		if(n>=m_Subsets.size())return;
		setVertexFormat(vf);
		//setVertexBuffer(vid);
		//setIndexBuffer(iid);
		DrawIndexedPrimitiveUP(PRIM_TRIANGLES, // PRIMITIVETYPE PrimitiveType,
						     0, 				     // UINT MinVertexIndex,
						     m_Subsets[n].m_dNumVertices,			// UINT NumVertices,
						     m_Subsets[n].m_dNumFaces,		   // UINT PrimitiveCount,
						     &m_Subsets[n].m_pIndices[0],		// CONST void * pIndexData,
						     &m_Subsets[n].m_pIndices[0],		// CONST void * pIndexData,
						     CONSTANT_INDEX4,      // FORMAT IndexDataFormat,
						     &m_Subsets[n].m_pVertices[0],			       // CONST void* pVertexStreamZeroData,
						     &m_Subsets[n].m_pVertices[0],			       // CONST void* pVertexStreamZeroData,
						     vertexStride);	 // UINT VertexStreamZeroStride
	}
	unsigned int Draw()
	{
		setVertexFormat(vf);
		//setVertexBuffer(0,vid);
		//setIndexBuffer(iid);
		//DrawIndexedPrimitive(PRIM_TRIANGLES,0,0,m_dNumVertices,0,m_dNumFaces);
		DrawIndexedPrimitiveUP(PRIM_TRIANGLES, // PRIMITIVETYPE PrimitiveType,
						     0, 				     // UINT MinVertexIndex,
						     m_dNumVertices,			// UINT NumVertices,
						     m_dNumFaces,		   // UINT PrimitiveCount,
						     &m_pIndices[0],		// CONST void * pIndexData,
						     &m_pIndices[0],		// CONST void * pIndexData,
						     CONSTANT_INDEX4,      // FORMAT IndexDataFormat,
						     &m_pVertices[0],			       // CONST void* pVertexStreamZeroData,
						     &m_pVertices[0],			       // CONST void* pVertexStreamZeroData,
						     vertexStride);	 // UINT VertexStreamZeroStride
	}
	__DWORD__ GetNumFaces(){return m_dNumFaces;}
	__DWORD__ GetNumVertices(){return m_dNumVertices;}
	//void GetVertexBuffer(VertexBufferID* id){*id=vid;}
	//void GetIndexBuffer(IndexBufferID* id){*id=iid;}
	void CreateMesh( __DWORD__ nf, __DWORD__ nv)
	{
		m_dNumFaces=nf;
		m_dNumVertices=nv;
	m_pIndices.resize(nf*3);
	m_pVertices.resize(nv);
	vertexStride=sizeof(stVertex2);
	FormatDesc Fmt[] = {
		0, TYPE_VERTEX, FORMAT_FLOAT, 3,
		0, TYPE_NORMAL, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(Fmt, elementsOf(Fmt));
	}
	void addSubset(
											__DWORD__ numVerts,
											__DWORD__ numTrisMat,
											__DWORD__* startMatInds,
											void* vertex,
											MATERIAL& material=MATERIAL(),std::string& TextureFileName=std::string("")
	)
	{
		//assert(startMatInds>=m_pIndices);
		//assert(vertex>=m_pVertices);
											SUBSET subset;
											subset.m_Material=material;
											subset.m_TextureFileName=TextureFileName;
											subset.m_dNumVertices=numVerts;
											subset.m_dNumFaces=numTrisMat;
											subset.m_pIndices=startMatInds;
											subset.m_pVertices=vertex;
											m_Subsets.push_back(subset);
	}
	__DWORD__ m_dNumFaces;
	__DWORD__ m_dNumVertices;
	VertexFormatID vf;
	//VertexBufferID vid;
	//IndexBufferID iid;
	std::vector<__WORD__> m_pIndices;
	std::vector<stVertex2> m_pVertices;
	__DWORD__ vertexStride;
};
//typedef int ObjectID;
//ObjectID LoadMeshFromFile(const char* fn);
void LoadMeshFromFile(const char* fn, MESH** ppMesh);
// Transform (x, y, z, 1) by matrix, project result back into w=1.
inline D3DXFROMWINEVECTOR3* RendererHLSLCgVec3TransformCoord
( D3DXFROMWINEVECTOR3 *pOut, CONST D3DXFROMWINEVECTOR3 *pV, CONST D3DXFROMWINEMATRIX *pM ){IMPLEMENT;return pOut;}
inline void RendererHLSLCgMatrixIdentity(D3DXFROMWINEMATRIX* m_mat){IMPLEMENT;}
inline void RendererHLSLCgMatrixScaling(D3DXFROMWINEMATRIX* m_mat, float x, float y, float z){IMPLEMENT;}
inline void RendererHLSLCgMatrixTranslation(D3DXFROMWINEMATRIX* m_mat, float x, float y, float z){IMPLEMENT;}
inline void RendererHLSLCgMatrixRotationX(D3DXFROMWINEMATRIX* m_mat, float a){IMPLEMENT;}
inline void RendererHLSLCgMatrixRotationY(D3DXFROMWINEMATRIX* m_mat, float a){IMPLEMENT;}
inline void RendererHLSLCgMatrixRotationZ(D3DXFROMWINEMATRIX* m_mat, float a){IMPLEMENT;}
inline void RendererHLSLCgMatrixInverse(D3DXFROMWINEMATRIX* out,float a,D3DXFROMWINEMATRIX* in){IMPLEMENT;}
inline void RendererHLSLCgVec3Normalize(D3DXFROMWINEVECTOR3 *pOut,const D3DXFROMWINEVECTOR3 *pIn){IMPLEMENT;}
inline void RendererHLSLCgVec3Cross(D3DXFROMWINEVECTOR3 *pOut,const D3DXFROMWINEVECTOR3 *pIn,const D3DXFROMWINEVECTOR3 *pIn2){IMPLEMENT;}
inline void RendererHLSLCgMatrixRotationAxis(D3DXFROMWINEMATRIX* m, const D3DXFROMWINEVECTOR3 axis, const float rads){IMPLEMENT;}
inline void RendererHLSLCgMatrixRotationYawPitchRoll(D3DXFROMWINEMATRIX* m, const float yaw, const float pitch, const float roll){IMPLEMENT;}
#endif

int GetRegKey(CHAR* key, CHAR *fn, __DWORD__ count);
int SetRegKey(CHAR* key, CHAR *fn);
inline void SetF4Key(const char* n, D3DXFROMWINEVECTOR4 v)
{
/*
	char filename[1024];
	GetRegKey("Software\\C++GameSamples\\LogFile",filename,1024);
	SetRegKey("Software\\C++GameSamples\\LogFile",filename);
*/

	char nbuf[256];
	char buf[256];
        stx_snprintf(nbuf,256,"%s%s",n,"_x");
        stx_snprintf(buf,256,"%f",v.x);
	SetRegKey(nbuf,buf);
        stx_snprintf(nbuf,256,"%s%s",n,"_y");
        stx_snprintf(buf,256,"%f",v.y);
	SetRegKey(nbuf,buf);
        stx_snprintf(nbuf,256,"%s%s",n,"_z");
        stx_snprintf(buf,256,"%f",v.z);
	SetRegKey(nbuf,buf);
}
inline void GetF4Key(const char* n, D3DXFROMWINEVECTOR4& v)
{
/*
	char filename[1024];
	GetRegKey("Software\\C++GameSamples\\LogFile",filename,1024);
	SetRegKey("Software\\C++GameSamples\\LogFile",filename);
*/
	char nbuf[256];
	char buf[256];
        stx_snprintf(nbuf,256,"%s%s",n,"_x");
	GetRegKey(nbuf,buf,256);
    stx_sscanf(buf,"%f",&v.x);
        stx_snprintf(nbuf,256,"%s%s",n,"_y");
	GetRegKey(nbuf,buf,256);
    stx_sscanf(buf,"%f",&v.y);
        stx_snprintf(nbuf,256,"%s%s",n,"_z");
	GetRegKey(nbuf,buf,256);
        stx_sscanf(buf,"%f",&v.z);
}
#ifndef COLOR_DEFINED
typedef __DWORD__ COLOR;
#define COLOR_DEFINED
#endif
// maps unsigned 8 bits/channel to COLOR
#define COLOR_ARGB(a,r,g,b) \
    ((COLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define COLOR_RGBA(r,g,b,a) COLOR_ARGB(a,r,g,b)
#define COLOR_XRGB(r,g,b)   COLOR_ARGB(0xff,r,g,b)
#define COLOR_XYUV(y,u,v)   COLOR_ARGB(0xff,y,u,v)
#define COLOR_AYUV(a,y,u,v) COLOR_ARGB(a,y,u,v)
// maps floating point channels (0.f to 1.f range) to COLOR
#define COLOR_COLORVALUE(r,g,b,a) \
    COLOR_RGBA((__DWORD__)((r)*255.f),(__DWORD__)((g)*255.f),(__DWORD__)((b)*255.f),(__DWORD__)((a)*255.f))
#if 0
inline FLOAT D3DXFROMWINEVECTOR2Dot
( CONST D3DXFROMWINEVECTOR2 *pV1, CONST D3DXFROMWINEVECTOR2 *pV2 ){return dot(*pV1,*pV2);}
/*
inline D3DXFROMWINEVECTOR2* D3DXFROMWINEVECTOR2Cross
    ( D3DXFROMWINEVECTOR2 *pOut, CONST D3DXFROMWINEVECTOR2 *pV1, CONST D3DXFROMWINEVECTOR2 *pV2 )
{*pOut=cross(*pV1,*pV2);return pOut;}
*/
inline D3DXFROMWINEVECTOR2* D3DXFROMWINEVECTOR2Normalize
    ( D3DXFROMWINEVECTOR2 *pOut, CONST D3DXFROMWINEVECTOR2 *pV ){*pOut=normalize(*pV);return pOut;}


inline FLOAT D3DXFROMWINEVECTOR2Length
( CONST D3DXFROMWINEVECTOR2 *pV){return length(*pV);}
inline FLOAT D3DXFROMWINEVECTOR2LengthSq
( CONST D3DXFROMWINEVECTOR2 *pV){return length(*pV)*length(*pV);}

inline FLOAT D3DXFROMWINEVECTOR3Length
( CONST D3DXFROMWINEVECTOR3 *pV){return length(*pV);}
inline FLOAT D3DXFROMWINEVECTOR3LengthSq
( CONST D3DXFROMWINEVECTOR3 *pV){return length(*pV)*length(*pV);}

inline FLOAT D3DXFROMWINEVECTOR4Length
( CONST D3DXFROMWINEVECTOR4 *pV){return length(*pV);}
inline FLOAT D3DXFROMWINEVECTOR4LengthSq
( CONST D3DXFROMWINEVECTOR4 *pV){return length(*pV)*length(*pV);}


inline FLOAT D3DXFROMWINEVECTOR3Dot
( CONST D3DXFROMWINEVECTOR3 *pV1, CONST D3DXFROMWINEVECTOR3 *pV2 ){return dot(*pV1,*pV2);}
inline D3DXFROMWINEVECTOR3* D3DXFROMWINEVECTOR3Cross
    ( D3DXFROMWINEVECTOR3 *pOut, CONST D3DXFROMWINEVECTOR3 *pV1, CONST D3DXFROMWINEVECTOR3 *pV2 )
{*pOut=cross(*pV1,*pV2);return pOut;}
inline D3DXFROMWINEVECTOR3* D3DXFROMWINEVECTOR3Normalize
    ( D3DXFROMWINEVECTOR3 *pOut, CONST D3DXFROMWINEVECTOR3 *pV ){*pOut=normalize(*pV);return pOut;}
inline FLOAT D3DXFROMWINEVECTOR4Dot
( CONST D3DXFROMWINEVECTOR4 *pV1, CONST D3DXFROMWINEVECTOR4 *pV2 ){return dot(*pV1,*pV2);}
inline D3DXFROMWINEVECTOR4* D3DXFROMWINEVECTOR4Cross
    ( D3DXFROMWINEVECTOR4 *pOut, CONST D3DXFROMWINEVECTOR4 *pV1, CONST D3DXFROMWINEVECTOR4 *pV2 )
{*pOut=cross(*pV1,*pV2);return pOut;}
inline D3DXFROMWINEVECTOR4* D3DXFROMWINEVECTOR4Normalize
    ( D3DXFROMWINEVECTOR4 *pOut, CONST D3DXFROMWINEVECTOR4 *pV ){*pOut=normalize(*pV);return pOut;}

// Transform (x, y, z, 0) by matrix.  If you transforming a normal by a
// non-affine matrix, the matrix you pass to this function should be the
// transpose of the inverse of the matrix you would use to transform a coord.
D3DXFROMWINEVECTOR3* D3DXFROMWINEVECTOR3TransformNormal
    ( D3DXFROMWINEVECTOR3 *pOut, CONST D3DXFROMWINEVECTOR3 *pV, CONST D3DXFROMWINEMATRIX *pM );
#endif

//--------------------------------------------------------------------------------------
#if 0
#define _11 elem(0,0)
#define _12 elem(0,1)
#define _13 elem(0,2)
#define _14 elem(0,3)

#define _21 elem(1,0)
#define _22 elem(1,1)
#define _23 elem(1,2)
#define _24 elem(1,3)

#define _31 elem(2,0)
#define _32 elem(2,1)
#define _33 elem(2,2)
#define _34 elem(2,3)

#define _41 elem(3,0)
#define _42 elem(3,1)
#define _43 elem(3,2)
#define _44 elem(3,3)
#endif
#if 0
#include <opentree/utils/otvector3.h>
#include <opentree/utils/otmatrix.h>
using namespace opentree;
inline void D3DXFROMWINEVECTOR3TootVector3(D3DXFROMWINEVECTOR3* in, otVector3* out)
{
    stx_memcpy(out,in,sizeof(float)*3);
}
inline void otVector3Tovec3(otVector3* in, D3DXFROMWINEVECTOR3* out)
{
    stx_memcpy(out,in,sizeof(float)*3);
}
inline void D3DXFROMWINEMATRIXTootMatrix(D3DXFROMWINEMATRIX* in, otMatrix* out)
{
    stx_memcpy(out,in,sizeof(float)*16);
}
inline void otMatrixTomat4(otMatrix* in, D3DXFROMWINEMATRIX* out)
{
    stx_memcpy(out,in,sizeof(float)*16);
}

#include <aiVector3D.h>
#include "aiVector2D.h"
#include "aiMatrix3x3.h"
#include "aiMatrix4x4.h"
#include "aiQuaternion.h"
inline void D3DXFROMWINEVECTOR3ToaiVector3D(D3DXFROMWINEVECTOR3* in, aiVector3D* out)
{
    stx_memcpy(out,in,sizeof(float)*3);
}
inline void aiVector3DTovec3(aiVector3D* in, D3DXFROMWINEVECTOR3* out)
{
    stx_memcpy(out,in,sizeof(float)*3);
}
inline void D3DXFROMWINEVECTOR2ToaiVector2D(D3DXFROMWINEVECTOR2* in, aiVector2D* out)
{
    stx_memcpy(out,in,sizeof(float)*2);
}
inline void aiVector2DTovec2(aiVector2D* in, D3DXFROMWINEVECTOR2* out)
{
    stx_memcpy(out,in,sizeof(float)*2);
}
inline void D3DXFROMWINEMATRIXToaiMatrix4x4(D3DXFROMWINEMATRIX* in, aiMatrix4x4* out)
{
    stx_memcpy(out,in,sizeof(float)*16);
}
inline void aiMatrix4x4Tomat4(aiMatrix4x4* in, D3DXFROMWINEMATRIX* out)

{
    stx_memcpy(out,in,sizeof(float)*16);
}
#endif

inline void D3DXFROMWINEVECTOR2ToD3DXFROMWINEVECTOR2(D3DXFROMWINEVECTOR2* in, D3DXFROMWINEVECTOR2* out)
{
    stx_memcpy(out,in,sizeof(float)*2);
}
inline void D3DXFROMWINEVECTOR2Tovec2(D3DXFROMWINEVECTOR2* in, D3DXFROMWINEVECTOR2* out)
{
    stx_memcpy(out,in,sizeof(float)*2);
}
inline void D3DXFROMWINEVECTOR3ToD3DXFROMWINEVECTOR3(D3DXFROMWINEVECTOR3* in, D3DXFROMWINEVECTOR3* out)
{
    stx_memcpy(out,in,sizeof(float)*3);
}
inline void D3DXFROMWINEVECTOR3Tovec3(D3DXFROMWINEVECTOR3* in, D3DXFROMWINEVECTOR3* out)
{
    stx_memcpy(out,in,sizeof(float)*3);
}
inline void D3DXFROMWINEVECTOR4ToD3DXFROMWINEVECTOR4(D3DXFROMWINEVECTOR4* in, D3DXFROMWINEVECTOR4* out)
{
    stx_memcpy(out,in,sizeof(float)*4);
}
inline void D3DXFROMWINEVECTOR4Tovec4(D3DXFROMWINEVECTOR4* in, D3DXFROMWINEVECTOR4* out)
{
    stx_memcpy(out,in,sizeof(float)*4);
}
inline void D3DXFROMWINEMATRIXToD3DXFROMWINEMATRIX(D3DXFROMWINEMATRIX* in, D3DXFROMWINEMATRIX* out)
{
    stx_memcpy(out,in,sizeof(float)*16);
}
inline void D3DXFROMWINEMATRIXTomat4(D3DXFROMWINEMATRIX* in, D3DXFROMWINEMATRIX* out)
{
    stx_memcpy(out,in,sizeof(float)*16);
}

void D3DXFROMWINECreateTextureFromFile(
	const char* n,
	TextureID* id,
	bool cm=true,
	SamplerStateID si=-1);/*IRenderer::GetRendererInstance()->linear())
{
	*id=addTexture(
		n,
		cm,
		si);

}*/

FORMAT FORMAT3FromD3D (UINT Format);
#endif
#endif
#endif
#endif

