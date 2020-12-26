//----------------------------------------------------------------------------------
// File:   VolumeRenderer.fx
// Author: Ignacio Llamas and Chris (Wei-Tae) Kim and Sarah Tariq 
// Email:  sdkfeedback@nvidia.com
// 
// Copyright (c) 2007 NVIDIA Corporation. All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA OR ITS SUPPLIERS
// BE  LIABLE  FOR  ANY  SPECIAL,  INCIDENTAL,  INDIRECT,  OR  CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
//
//----------------------------------------------------------------------------------

#define RM_SMOKE    0
#define RM_FIRE     1
#define RM_LEVELSET 2
//--------------------------------------------------------------------------------------
// Textures
//--------------------------------------------------------------------------------------
Texture3D   volumeTex;
Texture2D   rayDataTex;
Texture2D   rayDataTexSmall;
Texture2D   rayCastTex;
Texture2D   sceneDepthTex;
Texture2D   edgeTex;
Texture2D   jitterTex;
Texture2D   fireTransferFunction;
Texture2D   glowTex;
TextureCube envMapTex;
//--------------------------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------------------------
float       RTWidth;
float       RTHeight;

float4x4    WorldViewProjection;
float4x4    InvWorldViewProjection; 
float4x4    WorldView;

float4x4    Grid2World;

float       ZNear = 0.05f;
float       ZFar = 1000.0f;

float3      gridDim;
float3      recGridDim;
float       maxGridDim;
float       gridScaleFactor = 1.0;
float3      eyeOnGrid;

float       edgeThreshold = 0.2;

float       tan_FovXhalf;
float       tan_FovYhalf;

//gaussian with a sigma of 3, and a miu of 0
float gaussian_3[5] =
{
    0.132981, 0.125794, 0.106483, 0.080657, 0.05467,
};

bool useGlow               = true;
float glowContribution     = 0.81f;
float finalIntensityScale  = 22.0f;
float finalAlphaScale      = 0.95f;
float smokeColorMultiplier = 2.0f;   
float smokeAlphaMultiplier = 0.1f; 
float fireAlphaMultiplier  = 0.4; 
int   rednessFactor        = 5; 

bool        g_bRaycastBisection      = true; // true: compute more accurate ray-surface intersection; false: use first hit position
bool        g_bRaycastFilterTricubic = true; // true: tricubic; false: trilinear
bool        g_bRaycastShadeAsWater   = true; // true: shade using reflection+refraction from environment map; false: output gradient


//--------------------------------------------------------------------------------------
// Pipeline State definitions
//--------------------------------------------------------------------------------------
SamplerState samPointClamp
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
};             

SamplerState samLinearClamp
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
};

SamplerState samLinearWrap
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};


BlendState FireBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = ONE;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = ONE;
    DestBlendAlpha = ONE;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
}; 

BlendState AlphaBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = SRC_ALPHA;
    DestBlendAlpha = INV_SRC_ALPHA;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

BlendState NoBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
    RenderTargetWriteMask[0] = 0x0F;
};

BlendState SubtractiveBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = ONE;
    DestBlend = ZERO;
    BlendOp = REV_SUBTRACT;         // DST - SRC
    SrcBlendAlpha = ONE;
    DestBlendAlpha = ONE;
    BlendOpAlpha = REV_SUBTRACT;    // DST - SRC
    RenderTargetWriteMask[0] = 0x0F;
};

RasterizerState CullNone
{
    MultiSampleEnable = False;
    CullMode = None;
};

RasterizerState CullFront
{
    MultiSampleEnable = False;
    CullMode = Front;
};

RasterizerState CullBack
{
    MultiSampleEnable = False;
    CullMode = Back;
};


//--------------------------------------------------------------------------------------
// Structs
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 pos      : POSITION;
};

struct PS_INPUT_RAYDATA_BACK
{
    float4 pos      : POSITION;
    float3 worldViewPos : TEXCOORD0;

};

struct PS_INPUT_RAYDATA_FRONT
{
    float4 pos      : POSITION;
    float3 posInGrid: POSITION;
    float3 worldViewPos : TEXCOORD0;
};

struct PS_INPUT_RAYCAST
{
    float4 pos      : POSITION;
    float3 posInGrid: POSITION;
};


struct VS_OUTPUT_EDGE
{
    // There's no textureUV11 because its weight is zero.
    float4 position      : POSITION;   // vertex position
    float2 textureUV00   : TEXCOORD0;  // kernel tap texture coords 
    float2 textureUV01   : TEXCOORD1;  // kernel tap texture coords 
    float2 textureUV02   : TEXCOORD2;  // kernel tap texture coords 
    float2 textureUV10   : TEXCOORD3;  // kernel tap texture coords 
    float2 textureUV12   : TEXCOORD4;  // kernel tap texture coords 
    float2 textureUV20   : TEXCOORD5;  // kernel tap texture coords 
    float2 textureUV21   : TEXCOORD6;  // kernel tap texture coords 
    float2 textureUV22   : TEXCOORD7;  // kernel tap texture coords 
};

struct VS_OUTPUT_GLOW_9
{
    float4 position    : POSITION;   // vertex position
    float2 textureM4   : TEXCOORD0;  // kernel tap texture coords 
    float2 textureM3   : TEXCOORD1;  // kernel tap texture coords 
    float2 textureM2   : TEXCOORD2;  // kernel tap texture coords 
    float2 textureM1   : TEXCOORD3;  // kernel tap texture coords 
    float2 texture0    : TEXCOORD4;  // kernel tap texture coords 
    float2 textureP1   : TEXCOORD5;  // kernel tap texture coords 
    float2 textureP2   : TEXCOORD6;  // kernel tap texture coords 
    float2 textureP3   : TEXCOORD7;  // kernel tap texture coords
    float2 textureP4   : TEXCOORD8;  // kernel tap texture coords  
};

#define OCCLUDED_PIXEL_RAYVALUE     float4(1, 0, 0, 0)
#define NEARCLIPPED_PIXEL_RAYPOS    float3(0, -1, 0)


/////////////////////////////////
// BEGIN Custom Sampling Functions

// cubic b-spline
float bsW0(float a)
{
    return (1.0/6.0 * ( -(a*a*a) + (3.0 * a*a) - (3.0 * a) + 1.0));
}

float bsW1(float a)
{
    return (1.0/6.0 * ( (3.0 * a*a*a) - (6.0 * a*a) + 4.0 ));
}

float bsW2(float a)
{
    return (1.0/6.0 * ( -(3.0 * a*a*a) + (3.0 * a*a) + (3.0*a) + 1.0));
}

float bsW3(float a)
{
    return (1.0/6.0 * a*a*a);
}

float g0(float a)
{
    return (bsW0(a) + bsW1(a));
}

float g1(float a)
{
    return (bsW2(a) + bsW3(a));
}

float h0texels(float a)
{
    return (1.0 + a - (bsW1(a)/(bsW0(a)+bsW1(a))));
}

float h1texels(float a)
{
    return (1.0 - a + (bsW3(a)/(bsW2(a)+bsW3(a))));
}
/// end cubic-bspline

// first derivative of cubic b-spline
float bsfdW0(float a)
{
    return (1.0/6.0 * ( -(3.0 * a*a) + (6.0 * a) - 3.0));
}

float bsfdW1(float a)
{
    return (1.0/6.0 * ( (9.0 * a*a) - (12.0 * a) ));
}

float bsfdW2(float a)
{
    return (1.0/6.0 * ( -(9.0 * a*a) + (6.0 * a) + 3.0));
}

float bsfdW3(float a)
{
    return (1.0/6.0 * 3.0 * a*a);
}

float gfd0(float a)
{
    return (bsfdW0(a) + bsfdW1(a));
}

float gfd1(float a)
{
    return (bsfdW2(a) + bsfdW3(a));
}

float hfd0texels(float a)
{
    return (1.0 + a - (bsfdW1(a)/(bsfdW0(a)+bsfdW1(a))));
}

float hfd1texels(float a)
{
    return (1.0 - a + (bsfdW3(a)/(bsfdW2(a)+bsfdW3(a))));
}
/// end first derivative of cubic b-spline

float4 getHHGG( float xTexels)
{
    float a = frac(xTexels);
    return float4( -h0texels(a), h1texels(a), 1.0-g0(a), g0(a) );
}

float4 getfdHHGG( float xTexels)
{
    float a = frac(xTexels);
    return float4( -hfd0texels(a), hfd1texels(a), gfd1(a), -gfd1(a) );
}


float4 SampleTricubicGeneric(Texture3D tex, float3 tc, float4 hg_x, float4 hg_y, float4 hg_z)
{   
    float3  tc100, tc000, tc110, tc010,
            tc101, tc001, tc111, tc011;
    
    tc100 = tc;
    tc000 = tc;
    tc100.x += (hg_x.x * recGridDim.x);
    tc000.x += (hg_x.y * recGridDim.x);
    
    tc110 = tc100;
    tc010 = tc000;
    tc110.y += (hg_y.x * recGridDim.y);
    tc010.y += (hg_y.x * recGridDim.y);
    tc100.y += (hg_y.y * recGridDim.y);
    tc000.y += (hg_y.y * recGridDim.y);

    tc111 = tc110;
    tc011 = tc010;
    tc101 = tc100;
    tc001 = tc000;
    tc111.z += (hg_z.x * recGridDim.z);
    tc011.z += (hg_z.x * recGridDim.z);
    tc101.z += (hg_z.x * recGridDim.z);
    tc001.z += (hg_z.x * recGridDim.z);

    float4 v001 = tex.SampleLevel(samLinearClamp, tc001, 0);
    float4 v011 = tex.SampleLevel(samLinearClamp, tc011, 0);
    float4 v101 = tex.SampleLevel(samLinearClamp, tc101, 0);
    float4 v111 = tex.SampleLevel(samLinearClamp, tc111, 0);

    float4 v0Y1 = (v001 * hg_y.z) + (v011 * hg_y.w);
    float4 v1Y1 = (v101 * hg_y.z) + (v111 * hg_y.w);

    float4 vXY1 = (v0Y1 * hg_x.z) + (v1Y1 * hg_x.w);
    
    tc110.z += (hg_z.y * recGridDim.z);
    tc010.z += (hg_z.y * recGridDim.z);
    tc100.z += (hg_z.y * recGridDim.z);
    tc000.z += (hg_z.y * recGridDim.z);

    
    float4 v000 = tex.SampleLevel(samLinearClamp, tc000, 0);
    float4 v010 = tex.SampleLevel(samLinearClamp, tc010, 0);
    float4 v100 = tex.SampleLevel(samLinearClamp, tc100, 0);
    float4 v110 = tex.SampleLevel(samLinearClamp, tc110, 0);

    float4 v0Y0 = (v000 * hg_y.z) + (v010 * hg_y.w);
    float4 v1Y0 = (v100 * hg_y.z) + (v110 * hg_y.w);

    float4 vXY0 = (v0Y0 * hg_x.z) + (v1Y0 * hg_x.w);


    float4 vXYZ = (vXY0 * hg_z.z) + (vXY1 * hg_z.w);

    return vXYZ;
}


float4 SampleTricubic(Texture3D tex, float3 tc)
{
    float3 tcTexels = (tc * gridDim) - 0.49;

    float4 hg_x = getHHGG(tcTexels.x);
    float4 hg_y = getHHGG(tcTexels.y);
    float4 hg_z = getHHGG(tcTexels.z);

    return SampleTricubicGeneric(tex, tc, hg_x, hg_y, hg_z);
}

float4 SampleGradientTricubic(Texture3D tex, float3 tc)
{
    float3 tcTexels = (tc * gridDim) - 0.49;

    float4 hg_x   = getHHGG(tcTexels.x);
    float4 hg_y   = getHHGG(tcTexels.y);
    float4 hg_z   = getHHGG(tcTexels.z);
    float4 hgfd_x = getfdHHGG(tcTexels.x);
    float4 hgfd_y = getfdHHGG(tcTexels.y);
    float4 hgfd_z = getfdHHGG(tcTexels.z);

    return float4(  SampleTricubicGeneric(tex, tc, hgfd_x, hg_y, hg_z).r,
                    SampleTricubicGeneric(tex, tc, hg_x, hgfd_y, hg_z).r,
                    SampleTricubicGeneric(tex, tc, hg_x, hg_y, hgfd_z).r, 1.0 );
}


float4 SampleTrilinear(Texture3D tex, float3 tc)
{
    return tex.SampleLevel(samLinearClamp, tc, 0);
}

float4 SampleGradientTrilinear(Texture3D tex, float3 tc)
{
    #define LEFTCELL    float3 (tc.x-(1.0/gridDim.x), tc.y, tc.z)
    #define RIGHTCELL   float3 (tc.x+(1.0/gridDim.x), tc.y, tc.z)
    #define BOTTOMCELL  float3 (tc.x, (tc.y-(1.0/gridDim.y)), tc.z)
    #define TOPCELL     float3 (tc.x, (tc.y+(1.0/gridDim.y)), tc.z)
    #define DOWNCELL    float3 (tc.x, tc.y, tc.z - (1.0/gridDim.z))
    #define UPCELL      float3 (tc.x, tc.y, tc.z + (1.0/gridDim.z))

    float4 texL = tex.SampleLevel( samLinearClamp, LEFTCELL, 0 );
    float4 texR = tex.SampleLevel( samLinearClamp, RIGHTCELL, 0 );
    float4 texB = tex.SampleLevel( samLinearClamp, BOTTOMCELL, 0 );
    float4 texT = tex.SampleLevel( samLinearClamp, TOPCELL, 0 );
    float4 texU = tex.SampleLevel( samLinearClamp, UPCELL, 0 );
    float4 texD = tex.SampleLevel( samLinearClamp, DOWNCELL, 0 );
    return float4(  texR.r - texL.r, texT.r - texB.r, texU.r - texD.r, 1 );
}


float4 Sample(Texture3D tex, float3 tc)
{
    if( g_bRaycastFilterTricubic )
        return SampleTricubic(tex, tc);
    else
        return SampleTrilinear(tex, tc);
}

float4 SampleGradient(Texture3D tex, float3 tc)
{
    if( g_bRaycastFilterTricubic )
        return SampleGradientTricubic(tex, tc);
    else
        return SampleGradientTrilinear(tex, tc);
}
// END Custom Sampling Functions
/////////////////////////////////

#define OBSTACLE_MAX_HEIGHT 4
uniform bool renderFire;
void SampleSmokeOrFire(float weight, float3 O, inout float4 color)
{
    
    float3 texcoords = O;
    
    if(!renderFire)
    {    
        //render smoke with front to back blending
        float t;
        float4 sample = weight * abs(volumeTex.SampleLevel(samLinearClamp, texcoords, 0));
        //float4 sample = weight *abs(SampleTricubic(volumeTex, texcoords));
        sample.a = (sample.r) * 0.1;
        t = sample.a * (1.0-color.a);
        color.rgb += t * sample.r;
        color.a += t;
    }
    else
    {
        //render fire and smoke with back to front blending 
        
        //dont render the area below where the fire originates
        if(O.z < OBSTACLE_MAX_HEIGHT/gridDim.z)
            return;
        
        //this is the threshold at which we decide whether to render fire or smoke
        float threshold = 1.4;
        float maxValue = 3;
        
        float s = volumeTex.SampleLevel(samLinearClamp, texcoords, 0).x;
        s = clamp(s,0,maxValue);
          
        if(s>threshold)   
        {   
            //render fire
            float lookUpVal = ( (s-threshold)/(maxValue-threshold) );
            lookUpVal = 1.0 - pow(lookUpVal, rednessFactor);
            lookUpVal = clamp(lookUpVal,0,1);
            float3 interpColor = fireTransferFunction.SampleLevel(samLinearClamp, float2(lookUpVal,0),0); 
            float mult = (s-threshold);
            color += float4(weight*interpColor.rgb, weight*mult*mult*fireAlphaMultiplier); 
        }
        else    
        {    
             //render smoke
             float4 sample = weight*s;
             sample.a = sample.r*0.1*smokeAlphaMultiplier;
             float3 smokeColor = float3(0.9,0.35,0.055);
             color.rgb = (1 - sample.a) * color.rgb + sample.a * sample.rrr * smokeColor * smokeColorMultiplier * 5.0; 
             color.a = (1 - sample.a) * color.a + sample.a;
        }        
    }
}

float EdgeDetectScalar(float sx, float sy, float threshold)
{
    float dist = (sx*sx+sy*sy);
    float e = (dist > threshold*ZFar)? 1: 0;
    return e;
}



uniform int raycastMode;
uniform float sampleFactor;
uniform bool allowGlow;


[Vertex shader]
//--------------------------------------------------------------------------------------
// Vertex Shaders
//--------------------------------------------------------------------------------------
PS_INPUT_RAYDATA_BACK VS_RAYDATA_BACK(VS_INPUT input)
{
    PS_INPUT_RAYDATA_BACK output = (PS_INPUT_RAYDATA_BACK)0;
    output.pos = mul(float4(input.pos,1), WorldViewProjection);
    output.worldViewPos = mul(float4(input.pos,1), WorldView).xyz;
    return output;
}

PS_INPUT_RAYDATA_FRONT VS_RAYDATA_FRONT(VS_INPUT input)

{
    PS_INPUT_RAYDATA_FRONT output = (PS_INPUT_RAYDATA_FRONT)0;
    output.pos =  mul(float4(input.pos,1), WorldViewProjection);
    output.posInGrid = input.pos;
    output.worldViewPos = mul(float4(input.pos,1), WorldView).xyz;
    return output;
}

PS_INPUT_RAYCAST VS_RAYCAST_QUAD (VS_INPUT input)
{
    PS_INPUT_RAYCAST output = (PS_INPUT_RAYCAST)0;
    output.pos = float4(input.pos,1);
    output.posInGrid = mul( float4( input.pos.xy*ZNear, 0, ZNear ), InvWorldViewProjection );
    return output;
}


// A full-screen edge detection pass to locate artifacts
VS_OUTPUT_EDGE VS_EDGE_DETECT( VS_INPUT input )
{
    VS_OUTPUT_EDGE output = (VS_OUTPUT_EDGE)0;
    output.position = float4(input.pos,1);

    float2 texelSize = 1.0 / float2(RTWidth,RTHeight);
    float2 center = float2( (input.pos.x+1)/2.0 , 1.0 - (input.pos.y+1)/2.0 );

    // Eight nearest neighbours needed for Sobel.
    output.textureUV00 = center + float2(-texelSize.x, -texelSize.y);
    output.textureUV01 = center + float2(-texelSize.x,  0);
    output.textureUV02 = center + float2(-texelSize.x,  texelSize.y);

    output.textureUV10 = center + float2(0, -texelSize.y);
    output.textureUV12 = center + float2(0,  texelSize.y);

    output.textureUV20 = center + float2(texelSize.x, -texelSize.y);
    output.textureUV21 = center + float2(texelSize.x,  0);
    output.textureUV22 = center + float2(texelSize.x,  texelSize.y);

    return output;
}


// A full-screen glow pass in the horizontal direction
VS_OUTPUT_GLOW_9 VS_GLOW_H_9( VS_INPUT input )
{
    VS_OUTPUT_GLOW_9 output;
    output.position = float4(input.pos,1);

    float texelSize = 1.0 / RTWidth;
    float2 center = float2( (input.pos.x+1)/2.0 , 1.0 - (input.pos.y+1)/2.0 );

    // 9 taps for a guassian filter with sigma of 3
    output.textureM4 = center + float2(-texelSize*4.0f, 0);
    output.textureM3 = center + float2(-texelSize*3.0f, 0);
    output.textureM2 = center + float2(-texelSize*2.0f, 0);
    output.textureM1 = center + float2(-texelSize, 0);
    output.texture0  = center;
    output.textureP1 = center + float2(texelSize, 0);
    output.textureP2 = center + float2(texelSize*2.0f, 0);
    output.textureP3 = center + float2(texelSize*3.0f, 0);
    output.textureP4 = center + float2(texelSize*4.0f, 0);

    
    return output;
}

// A full-screen glow pass in the vertical direction
VS_OUTPUT_GLOW_9 VS_GLOW_V_9( VS_INPUT input )
{
    VS_OUTPUT_GLOW_9 output;
    output.position = float4(input.pos,1);

    float texelSize = 1.0 / RTHeight;
    float2 center = float2( (input.pos.x+1)/2.0 , 1.0 - (input.pos.y+1)/2.0 );

    // 9 taps for a guassian filter with sigma of 3
    output.textureM4 = center + float2(0,-texelSize*4.0f);
    output.textureM3 = center + float2(0,-texelSize*3.0f);
    output.textureM2 = center + float2(0,-texelSize*2.0f);
    output.textureM1 = center + float2(0,-texelSize);
    output.texture0  = center;
    output.textureP1 = center + float2(0,texelSize);
    output.textureP2 = center + float2(0,texelSize*2.0f);
    output.textureP3 = center + float2(0,texelSize*3.0f);
    output.textureP4 = center + float2(0,texelSize*4.0f);
    
    return output;
}

[Fragment shader]
//--------------------------------------------------------------------------------------
// Pixel Shaders
//--------------------------------------------------------------------------------------


float4 PS_RAYDATA_BACK(PS_INPUT_RAYDATA_BACK input) : COLOR0
{
    float4 output;
    
    //get the distance from the eye to the scene point
    //we do this by unprojecting the scene point to view space and then taking the length of the vector
    float2 normalizedInputPos = float2(input.pos.x/RTWidth, input.pos.y/RTHeight);
    float sceneZ = sceneDepthTex.SampleLevel( samLinearClamp, normalizedInputPos ,0).r;
    float2 inputPos = float2((normalizedInputPos.x*2.0)-1.0,(normalizedInputPos.y*2.0)-1.0);
    sceneZ = length(float3( inputPos.x * sceneZ * tan_FovXhalf, inputPos.y * sceneZ * tan_FovYhalf, sceneZ ));
    
    float inputDepth = length(input.worldViewPos);

    // This value will only remain if no fragments get blended on top in the next pass (front-faces)
    //  which would happen if the front faces of the box get clipped by the near plane of the camera
    output.xyz = NEARCLIPPED_PIXEL_RAYPOS;

    output.w = min(inputDepth, sceneZ);
    return output;
}

float4 PS_RAYDATA_FRONT(PS_INPUT_RAYDATA_FRONT input) : COLOR0
{
    float4 output;
    
    float2 normalizedInputPos = float2(input.pos.x/RTWidth, input.pos.y/RTHeight);
    float sceneZ = sceneDepthTex.SampleLevel( samLinearClamp, normalizedInputPos, 0).r;
    float2 inputPos = float2((normalizedInputPos.x*2.0)-1.0,(normalizedInputPos.y*2.0)-1.0);
    sceneZ = length(float3( inputPos.x * sceneZ * tan_FovXhalf, inputPos.y * sceneZ * tan_FovYhalf, sceneZ ));
    
    float inputDepth = length(input.worldViewPos);
    
    if(sceneZ < inputDepth)
    {
        // If the scene occludes intersection point we want to kill the pixel early in PS
        return OCCLUDED_PIXEL_RAYVALUE;
    }
    // We negate input.posInGrid because we use subtractive blending in front faces
    //  Note that we set xyz to 0 when rendering back faces
    output.xyz = -input.posInGrid;
    output.w = inputDepth;
    return output;
}

float4 PS_RAYDATA_FRONT_NOBLEND(PS_INPUT_RAYDATA_FRONT input) : COLOR0
{
    float4 output;
    
    float2 normalizedInputPos = float2(input.pos.x/RTWidth, input.pos.y/RTHeight);
    float sceneZ = sceneDepthTex.SampleLevel(samLinearClamp, normalizedInputPos,0).r;
    float2 inputPos = float2((normalizedInputPos.x*2.0)-1.0,(normalizedInputPos.y*2.0)-1.0);
    sceneZ = length(float3( inputPos.x * sceneZ * tan_FovXhalf, inputPos.y * sceneZ * tan_FovYhalf, sceneZ ));
    
    float inputDepth = length(input.worldViewPos);
    
    if(sceneZ < inputDepth)
    {
        // If the scene occludes intersection point we want to kill the pixel early in PS
        return OCCLUDED_PIXEL_RAYVALUE;
    }

    float4 rayDataBackDepth = rayDataTex.SampleLevel(samLinearClamp, float2(input.pos.x/RTWidth, input.pos.y/RTHeight),0).w;

    // We negate input.posInGrid because we use subtractive blending in front faces
    //  Note that we set xyz to 0 when rendering back faces
    output.xyz = input.posInGrid;
    output.w = rayDataBackDepth - inputDepth;

    return output;
}

float4 Raycast( PS_INPUT_RAYCAST input )
{
    float4 color = 0;
    float2 normalizedInputPos = float2(input.pos.x/RTWidth,input.pos.y/RTHeight);
    float4 rayData = rayDataTex.Sample(samLinearClamp, normalizedInputPos);

    // Don't raycast if the starting position is negative 
    //   (see use of OCCLUDED_PIXEL_RAYVALUE in PS_RAYDATA_FRONT)
    if(rayData.x < 0)
        return color;

    // If the front face of the box was clipped here by the near plane of the camera
    //   (see use of NEARCLIPPED_PIXEL_RAYPOS in PS_RAYDATA_BACK)
    if(rayData.y < 0)
    {
       // Initialize the position of the fragment and adjust the depth
       rayData.xyz = input.posInGrid;
       float2 inputPos = float2((normalizedInputPos.x*2.0)-1.0,(normalizedInputPos.y*2.0)-1.0);
       float distanceToNearPlane = length(float3( inputPos.x * ZNear * tan_FovXhalf, inputPos.y * ZNear * tan_FovYhalf, ZNear ));
       rayData.w = rayData.w - distanceToNearPlane;
    }

    float3 rayOrigin = rayData.xyz;
    float rayLength = rayData.w;

    // Sample twice per voxel
    float fSamples = ( rayLength / gridScaleFactor * maxGridDim ) * sampleFactor;
    int nSamples = floor(fSamples);
    float3 stepVec = normalize( (rayOrigin - eyeOnGrid) * gridDim ) * recGridDim * (1.0/sampleFactor);
   
    float3 O = rayOrigin;

    if(raycastMode == RM_LEVELSET)
    {
        float rho = 0;
        float levelSet;
        int i;

        // Use lsFactor to negate the sampled value if we start inside the levelSet (i.e. first sample < rho)
        //  this is used to find the intersection when we start raycasting inside the volume
        float lsFactor = 1.0;     
        levelSet = SampleTrilinear( volumeTex, O-1*stepVec ).r;
        if( levelSet < rho )
            lsFactor = -1.0;
        
        O += stepVec;
        for( i=0; i<nSamples; i++, O += stepVec )
        {
        #if 1
            levelSet = Sample( volumeTex, O ).r * lsFactor;
            if(levelSet<rho)
                break;
        #else
            // adaptive sampling test

            levelSet = SampleTrilinear( volumeTex, O ).r * lsFactor;
            if(levelSet<(rho+0.05))
            {
                // go backwards a bit and increase sampling rate near detected intersection
                int backStepCount = 1;
                O -= (backStepCount*stepVec);

                float incSamplingFactor = 2.0;
                backStepCount *= incSamplingFactor;
                stepVec /= incSamplingFactor;
                for( int j=0; j<=backStepCount; j++, O += stepVec )
                {
                    levelSet = Sample( volumeTex, O ).r * lsFactor;
                    if(levelSet<rho)
                        break;
                }
                stepVec *= incSamplingFactor;
                if(levelSet<rho)
                    break;
            }
        #endif
        }

        if((i == nSamples) || (levelSet > rho) )

            return float4(0,0,0,0);


        if( g_bRaycastBisection )
        {
            // use bisection method to refine the intersection point
            float3 a = O - stepVec;
            float3 b = O;
            float3 m;
            
            // starting assumption is (ls_a > rho) && (ls_b < rho)
            for( int i=0; i<3; i++)
            {
                m = (a+b) * 0.5;

                float ls_m = Sample(volumeTex, m).r  * lsFactor;

                if( ls_m < rho )
                {
                    b = m;
                }
                else
                {
                    a = m;
                }

            }

            O =  (a+b) * 0.5;
        }

        float3 normal = SampleGradient(volumeTex, O).xyz;
        normal = normalize( normal);

        float3 shadedColor = 0;
        if( g_bRaycastShadeAsWater )
        {

            float waterIR = 1.3333;
            float airIR   = 1.0003;
            float etaRatio = waterIR / airIR;

            // if going from air to water
            if( lsFactor > 0 )
            {
                normal.y = -normal.y;
                etaRatio = 1.0 / etaRatio;
            }
            
            // Render using cubemap reflection and refraction
            float4 eyeVec = float4(normalize(O - eyeOnGrid), 0);
            float3 eyeReflected = mul(reflect(eyeVec, normal), Grid2World).xyz;
            float3 eyeRefracted = mul(refract(eyeVec, normal, etaRatio), Grid2World).xyz;
                
            float3 refractColor = envMapTex.SampleLevel( samLinearWrap, eyeRefracted, 0);
            //float3 refractColor = 0;
            float3 reflectColor = envMapTex.SampleLevel( samLinearWrap, eyeReflected, 0);
            float fresnelReflectionCoeff = max(0, min(1, pow(1+dot(eyeVec,normal),2)) );
            
            shadedColor += refractColor * (1 - fresnelReflectionCoeff);
            shadedColor += reflectColor * fresnelReflectionCoeff;
        }
        else
        {
            shadedColor = (normal+1.0)/2.0;
        }
    
        color = float4(shadedColor, 1);
    }
    else
    {
        float Offset = jitterTex.Sample( samLinearWrap, input.pos.xy / 256.0 ).r;
        O += stepVec*Offset;

        if(raycastMode == RM_FIRE)
        {
            // we render fire with back to front ray marching 
            // In back-to-front blending we start raycasting from the surface point and step towards the eye
            O += fSamples * stepVec;
            stepVec = -stepVec;
        }

        for( int i=0; i<nSamples ; i++ )
        {
            SampleSmokeOrFire(1, O, color, (raycastMode == RM_FIRE));
            O += stepVec;

            if(!(raycastMode == RM_FIRE))
            {
                // If doing front-to-back blending we can do early exit when opacity saturates
                if( color.a > 0.99 )
                    break;
            }
        }

        // The last sample is weighted by the fractional part of the ray length in voxel 
        //  space (fSamples), thus avoiding banding artifacts when the smoke is blended against the scene
        if( i == nSamples )
        {
            SampleSmokeOrFire(frac(fSamples), O, color, (raycastMode == RM_FIRE));
        }
    }
        
    return color;
}


float4 PS_RAYDATACOPY_QUAD(PS_INPUT_RAYCAST input) : COLOR0
{
    return rayDataTex.Sample(samPointClamp, float2(input.pos.x/RTWidth,input.pos.y/RTHeight));
}
float4 PS_RAYCAST_QUAD(PS_INPUT_RAYCAST input) : COLOR0
{
    return Raycast(input, raycastMode, sampleFactor);
}

//
// A full-screen edge detection pass to locate artifacts
//  these artifacts are located on a downsized version of the rayDataTexture
// We use a smaller texture both to accurately find all the depth artifacts 
//  when raycasting to this smaller size and to save on the cost of this pass
// Use col.a to find depth edges of objects occluding the smoke
// Use col.g to find the edges where the camera near plane cuts the smoke volume
//
float4 PS_EDGE_DETECT(VS_OUTPUT_EDGE vIn) : COLOR0
{

    // We need eight samples (the centre has zero weight in both kernels).
    float4 col;
    col = rayDataTexSmall.Sample(samPointClamp, vIn.textureUV00); 
    float g00 = col.a;
    if(col.g < 0)
        g00 *= -1;
    col = rayDataTexSmall.Sample(samPointClamp, vIn.textureUV01); 
    float g01 = col.a;
    if(col.g < 0)
        g01 *= -1;
    col = rayDataTexSmall.Sample(samPointClamp, vIn.textureUV02); 
    float g02 = col.a;
    if(col.g < 0)
        g02 *= -1;
    col = rayDataTexSmall.Sample(samPointClamp, vIn.textureUV10); 
    float g10 = col.a;
    if(col.g < 0)
        g10 *= -1;
    col = rayDataTexSmall.Sample(samPointClamp, vIn.textureUV12); 
    float g12 = col.a;
    if(col.g < 0)
        g12 *= -1;
    col = rayDataTexSmall.Sample(samPointClamp, vIn.textureUV20); 
    float g20 = col.a;
    if(col.g < 0)
        g20 *= -1;
    col = rayDataTexSmall.Sample(samPointClamp, vIn.textureUV21); 
    float g21 = col.a;
    if(col.g < 0)
        g21 *= -1;
    col = rayDataTexSmall.Sample(samPointClamp, vIn.textureUV22); 
    float g22 = col.a;
    if(col.g < 0)
        g22 *= -1;
    	
    // Sobel in horizontal dir.
    float sx = 0;
    sx -= g00;
    sx -= g01 * 2;
    sx -= g02;
    sx += g20;
    sx += g21 * 2;
    sx += g22;
    // Sobel in vertical dir - weights are just rotated 90 degrees.
    float sy = 0;
    sy -= g00;
    sy += g02;
    sy -= g10 * 2;
    sy += g12 * 2;
    sy -= g20;
    sy += g22;

    float e = EdgeDetectScalar(sx, sy, edgeThreshold);
    return float4(e,e,e,1);

}
float4 PS_GLOW_9(VS_OUTPUT_GLOW_9 vIn) : COLOR0
{
    float4 col = float4(0,0,0,0);
    float4 tex;
    float threshold = 1.4;

    tex = glowTex.Sample(samPointClamp, vIn.textureM4); 
    if(tex.r > threshold)
        col += tex*gaussian_3[4];
    
    tex = glowTex.Sample(samPointClamp, vIn.textureM3); 
    if(tex.r > threshold)
        col += tex*gaussian_3[3];
        
    tex = glowTex.Sample(samPointClamp, vIn.textureM2); 
    if(tex.r > threshold)
        col += tex*gaussian_3[2]; 
        
    tex = glowTex.Sample(samPointClamp, vIn.textureM1); 
    if(tex.r > threshold)
        col += tex*gaussian_3[1]; 
        
    tex = glowTex.Sample(samPointClamp, vIn.texture0); 
    if(tex.r > threshold)
        col += tex*gaussian_3[0];
        
    tex = glowTex.Sample(samPointClamp, vIn.textureP1); 
    if(tex.r > threshold)
        col += tex*gaussian_3[1]; 
      
    tex = glowTex.Sample(samPointClamp, vIn.textureP2); 
    if(tex.r > threshold)
        col += tex*gaussian_3[2]; 

    tex = glowTex.Sample(samPointClamp, vIn.textureP3); 

    if(tex.r > threshold)
        col += tex*gaussian_3[3];
       
    tex = glowTex.Sample(samPointClamp, vIn.textureP4); 
    if(tex.r > threshold)
        col += tex*gaussian_3[4];
       
    return col;
}
float4 RaycastUpsample(PS_INPUT_RAYCAST input)
{
    float edge = edgeTex.Sample(samLinearClamp, float2(input.pos.x/RTWidth,input.pos.y/RTHeight)).r;
    float4 tex = rayCastTex.Sample(samLinearClamp, float2(input.pos.x/RTWidth,input.pos.y/RTHeight));
    
    if(edge > 0 && tex.a > 0)
        return Raycast(input, raycastMode, sampleFactor );
    else
        return tex;
}
float4 PS_RAYCAST_UPSAMPLE_QUAD(PS_INPUT_RAYCAST input) : COLOR0
{
    float4 color = RaycastUpsample(input, raycastMode, sampleFactor);
    
    if( allowGlow )
    {
        if(useGlow)
        {
            float4 glow = glowTex.Sample(samLinearClamp, float2(input.pos.x/RTWidth,input.pos.y/RTHeight));
            color.rgba += glowContribution*glow.rgba;
        }

        //tone map
        color.rgb /= finalIntensityScale;

        color.a = clamp(color.a,0,1);
        color.rgb *= color.a; 
        color.a = color.a*finalAlphaScale;
    }

    return color;
}
/*
//------------------------------------------------------------------------------------------------------
//techniques
//------------------------------------------------------------------------------------------------------
 
technique10 VolumeRenderer
{
    pass CompRayData_Back
    {
        SetVertexShader(CompileShader( vs_4_0, VS_RAYDATA_BACK() ));
        SetGeometryShader ( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_RAYDATA_BACK() ));
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState(CullFront);
        SetDepthStencilState( DisableDepth, 0 );
    }

    pass CompRayData_Front
    {
        SetVertexShader(CompileShader( vs_4_0, VS_RAYDATA_FRONT() ));
        SetGeometryShader ( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_RAYDATA_FRONT() ));
        SetBlendState (SubtractiveBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetRasterizerState(CullBack);
        SetDepthStencilState( DisableDepth, 0 );
    }

    pass CompRayData_FrontNOBLEND
    {
        SetVertexShader(CompileShader( vs_4_0, VS_RAYDATA_FRONT() ));
        SetGeometryShader ( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_RAYDATA_FRONT_NOBLEND() ));
        SetBlendState (NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetRasterizerState(CullBack);
        SetDepthStencilState( DisableDepth, 0 );
    }
        
    pass QuadDownSampleRayDataTexture
    {
        SetVertexShader(CompileShader( vs_4_0, VS_RAYCAST_QUAD() ));
        SetGeometryShader ( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_RAYDATACOPY_QUAD() ));
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState(CullBack);
        SetDepthStencilState( DisableDepth, 0 );
    }

    pass QuadRaycastSmoke
    {
        SetVertexShader(CompileShader( vs_4_0, VS_RAYCAST_QUAD() ));
        SetGeometryShader ( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_RAYCAST_QUAD(RM_SMOKE, 2.0) ));
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState(CullBack);
        SetDepthStencilState( DisableDepth, 0 );
    }  
    
    pass QuadRaycastFire
    {
        SetVertexShader(CompileShader( vs_4_0, VS_RAYCAST_QUAD() ));
        SetGeometryShader ( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_RAYCAST_QUAD(RM_FIRE, 2.0) ));
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState(CullBack);
        SetDepthStencilState( DisableDepth, 0 );
    }

    pass QuadRaycastLevelSet
    {
        SetVertexShader(CompileShader( vs_4_0, VS_RAYCAST_QUAD() ));
        SetGeometryShader ( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_RAYCAST_QUAD(RM_LEVELSET, 1.0) ));
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState(CullBack);
        SetDepthStencilState( DisableDepth, 0 );
    }
         
    pass QuadEdgeDetect
    {              
        SetVertexShader(CompileShader( vs_4_0, VS_EDGE_DETECT() ));
        SetGeometryShader ( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_EDGE_DETECT() ));
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState(CullNone);
        SetDepthStencilState( DisableDepth, 0 );

    }
    
     pass GlowHorizontal
    {
        SetVertexShader(CompileShader( vs_4_0, VS_GLOW_H_9() ));
        SetGeometryShader ( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_GLOW_9() ));
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState(CullBack);
        SetDepthStencilState( DisableDepth, 0 );
    } 
    
     pass GlowVertical
    {
        SetVertexShader(CompileShader( vs_4_0, VS_GLOW_V_9() ));
        SetGeometryShader ( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_GLOW_9() ));
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState(CullBack);
        SetDepthStencilState( DisableDepth, 0 );
    } 
    
    pass QuadRaycastUpsampleSmoke
    {
        SetVertexShader(CompileShader( vs_4_0, VS_RAYCAST_QUAD() ));
        SetGeometryShader ( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_RAYCAST_UPSAMPLE_QUAD(RM_SMOKE, 2.0, false) ));
        SetBlendState( AlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState(CullBack);
        SetDepthStencilState( DisableDepth, 0 );
    }
 
    pass QuadRaycastUpsampleFire
    {
        SetVertexShader(CompileShader( vs_4_0, VS_RAYCAST_QUAD() ));
        SetGeometryShader ( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_RAYCAST_UPSAMPLE_QUAD(RM_FIRE, 2.0, true) ));
        SetBlendState( FireBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState(CullBack);
        SetDepthStencilState( DisableDepth, 0 );
    }

    pass QuadRaycastUpsampleLevelSet
    {
        SetVertexShader(CompileShader( vs_4_0, VS_RAYCAST_QUAD() ));
        SetGeometryShader ( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_RAYCAST_UPSAMPLE_QUAD(RM_LEVELSET, 1.0, false) ));
        SetBlendState( AlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState(CullBack);
        SetDepthStencilState( DisableDepth, 0 );
    }
}
*/
