/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//
//  Vertex Shader Constants
//
#define VS_WORLD_MATRIX                                    0
#define VS_WORLD_VIEW_MATRIX                               4
#define VS_WORLD_VIEW_PROJECTION_MATRIX                    8
#define VS_PROJECTION_MATRIX                               12
#define VS_VIEW_PROJECTION_MATRIX                          16
#define VS_WORLD_INVERSE_TRANSPOSE_MATRIX                  20
#define VS_LIGHT_POSITION_WORLD                            32
#define VS_EYE_POSITION_WORLD                              33
#define VS_MATERIAL_DIFFUSE                                34
#define VS_MATERIAL_AMBIENT                                35
#define VS_LIGHT_DIFFUSE                                   36
#define VS_LIGHT_AMBIENT                                   37
#define VS_ETA_RATIO                                       51
#define VS_REFRACTION_TEXCOORD_SCALE_OFFSET                52
#define VS_REFLECTION_TEXCOORD_SCALE_OFFSET                53
#define VS_NORMAL_SIGN                                     54
#define VS_SCALE_BIAS_CONSTANTS                            94
#define VS_MATH_CONSTANTS                                  95




//
//  Pixel Shader Constants
//
#define PS_SIMULATION_TEX_COORD_DELTA_X0Y1                 10
#define PS_SIMULATION_TEX_COORD_DELTA_X2Y1                 11
#define PS_SIMULATION_TEX_COORD_DELTA_X1Y0                 12
#define PS_SIMULATION_TEX_COORD_DELTA_X1Y2                 13
#define PS_SIMULATION_POSITION_WEIGHTING                   14
#define PS_SIMULATION_WAVE_SPEED_SQUARED                   15
#define PS_SIMULATION_ONE_HALF_TIMES_DELTA_TIME_SQUARED    16
#define PS_SIMULATION_GRID_SIZE                            17
#define PS_TOTAL_INTERNAL_REFLECTION_SLOPE_BIAS            18
#define PS_TIME                                            20
#define PS_DELTA_TIME                                      21
#define PS_FRESNEL_R0                                      22
#define PS_EYE_POSITION_WORLD                              23
#define PS_UNDER_WATER                                     24




