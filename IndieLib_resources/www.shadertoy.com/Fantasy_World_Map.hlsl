/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

struct VsIn2 {
    float2 position	: POSITION;
    float2 uv		: TEXCOORD0;
};
struct VsIn3 {
    float3 position	: POSITION;
    float3 Normal	: NORMAL;
    float3 Binormal	: BINORMAL;
    float3 Tangent	: TANGENT;
    float3 Color	: TEXCOORD0;
    float2 uv		: TEXCOORD1;
};
struct VsOut {
    float4 position	: POSITION;
    float2 uv		: TEXCOORD0;
};

[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut main2(VsIn2 In)
{
	VsOut Out=(VsOut)0;
	Out.position = float4(In.position.x, In.position.y, 0.0 , 1.0);
	Out.uv.x = In.uv.x;Out.uv.y = 1.0-In.uv.y;
	return Out;
}
VsOut main3(VsIn3 In)
{
	VsOut Out=(VsOut)0;
	Out.position = mul(worldViewProj, float4(In.position, 1.0));
	Out.uv.x = In.uv.x;Out.uv.y = 1.0-In.uv.y;
	return Out;
}

[Fragment shader]
float mod(float x, float y)
{
  return x - y * floor(x / y);
}
float2 mod(float2 x, float2 y)
{
  return x - y * floor(x / y);
}
float3 mod(float3 x, float3 y)
{
  return x - y * floor(x / y);
}
float iTime=0.0;
float2 iMouse=float2(1, 1);
float2 iResolution=float2(1, 1);
//sampler2D iChannel0;

// https://www.shadertoy.com/view/3d3cR2
// Fantasy World Map
#define DRAW_TOWNS
//#define GO_CRAZY

float lineWidth = 0.5;

#define terrainScale 80.0
float3 landColor = float3(0.9, 0.8, 0.7);
float3 shoreLineColor = float3(0,0,0);

float waterThreshold = -0.04;
float3 waterDeepColor = float3(0.7, 0.8, 0.9);
float3 waterShallowColor = float3(0.5, 0.6, 0.9);
float3 waterLineColor = float3(0.0, 0.1, 0.3);

#define mountainWidth 20.0
#define mountainHeight 16.0
float mountainThreshold = 0.23;
float3 mountainColor = float3(0.5, 0.3, 0.1);
float3 mountainShadowColor = float3(0.3, 0.2, 0.0);
float3 mountainLineColor = float3(0.3, 0.1, 0.0);

#define treeScale 3.5
float treeGrow = 1.5;
#define treeGrowthScale 100.0
float treeGrowthThreshold = 0.08;
float treeTerrainThresholdLow = 0.02;
float treeTerrainThresholdHigh = 0.12;
float3 treeColor = float3(0.3, 0.8, 0.4);
float3 treeShadowColor = float3(0.0, 0.4, 0.2);
float3 treeLineColor = float3(0.0, 0.3, 0.1);

#define townSpacing 80.0
float buildingLineWidth = 0.3;
#define buildingSize 5.0
float roofSize = 0.3;
float urbanSprawl = 8.0;
float townTerrainThresholdLow = 0.0;
float townTerrainThresholdHigh = 0.18;
float townTreeThreshold = 0.04;

float paperTexture = 3.0;
float dirt = 0.5;
float3 dirtColor = float3(0.6, 0.3, 0.1);

float vignetting = 0.4;
float3 vignettingColor = float3(0.3, 0.2, 0.0);


float pi = 3.14159265;

float terrainScaleInv = 1.0 / terrainScale;
float mountainWidthInv = 1.0 / mountainWidth;
float mountainHeightInv = 1.0 / mountainHeight;
float treeScaleInv = 1.0 / treeScale;
float treeGrowthScaleInv = 1.0 / treeGrowthScale;
float townSpacingInv = 1.0 / townSpacing;
float buildingSizeInv = 1.0 / buildingSize;

float aa;
float lw;
float blw;
float bsdf;


float hash1(float2 p) {
    float3 p3  = frac(float3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return frac((p3.x + p3.y) * p3.z);
}

float2 hash2(float2 p) {
    float3 p3 = frac(float3(p.xyx) * float3(0.1031, 0.1030, 0.0973));
    p3 += dot(p3, p3.yzx + 33.33);
    return frac((p3.xx + p3.yz) * p3.zy);
}

float3 hash3(float2 p) {
    float3 p3 = frac(float3(p.xyx) * float3(0.1031, 0.1030, 0.0973));
    p3 += dot(p3, p3.yxz + 33.33);
    return frac((p3.xxy + p3.yzz) * p3.zyx);
}

float noise1(float2 p) {
    float2 i = floor(p);
    float2 f = frac(p);
    float2 u = f * f * (3.0 - 2.0 * f);
    return lerp(lerp(hash1(i + float2(0.0, 0.0)), 
                   hash1(i + float2(1.0, 0.0)), u.x),
               lerp(hash1(i + float2(0.0, 1.0)), 
                   hash1(i + float2(1.0, 1.0)), u.x), u.y);
}

float2x2 m = float2x2(1.616, 1.212, -1.212, 1.616);

float fbm1(float2 p) {
    float f = noise1(p) - 0.5; p = mul(m,  p);
    f += 0.5 * (noise1(p) - 0.5); p = mul(m,  p);
    f += 0.25 * (noise1(p) - 0.5);
    return f / 1.75;
}

float fbm1high(float2 p) {
    float f = noise1(p) - 0.5; p = mul(m,  p);
    f += 0.5 * (noise1(p) - 0.5); p = mul(m,  p);
    f += 0.25 * (noise1(p) - 0.5); p = mul(m,  p);
    f += 0.125 * (noise1(p) - 0.5); p = mul(m,  p);
    f += 0.0625 * (noise1(p) - 0.5);
    return f / 1.9375;
}

float sdBox(float2 p, float2 b) {
    float2 d = abs(p) - b;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

float sdTriangleIsosceles(float2 p, float2 q) {
    p.x = abs(p.x);
    float2 a = p - q * clamp(dot(p, q) / dot(q, q), 0.0, 1.0);
    float2 b = p - q * float2(clamp(p.x / q.x, 0.0, 1.0 ), 1.0);
    float s = -sign(q.y);
    float2 d = min(float2(dot(a, a), s * (p.x * q.y - p.y * q.x)), float2(dot(b, b), s * (p.y - q.y)));
    return -sqrt(d.x) * sign(d.y);
}

float3 water(float3 color, float2 pos) {
    float terrain = fbm1high(terrainScaleInv * pos) - waterThreshold;

    float3 waterColor = lerp(waterDeepColor, waterShallowColor, exp(20.0 * terrain));
    color = lerp(color, waterColor, step(terrain, 0.0));

    float offset = waterThreshold + terrain;
    float dx = fbm1high(terrainScaleInv * (pos + float2(0.01, 0.0))) - offset;
    float dy = fbm1high(terrainScaleInv * (pos + float2(0.0, 0.01))) - offset;
    float grad = 0.01 / length(float2(dx, dy));
    float t0 = terrain * grad;
    float t1 = (terrain + 0.02) * grad;
    float t2 = (terrain + 0.04) * grad;
    float t3 = (terrain + 0.06) * grad;
    float t4 = (terrain + 0.08) * grad;
    float t5 = (terrain + 0.1) * grad;

    color = lerp(color, waterLineColor, 0.5 * clamp((lw - abs(t1)) * aa, 0.0, 1.0));
    color = lerp(color, waterLineColor, 0.4 * clamp((lw - abs(t2)) * aa, 0.0, 1.0));
    color = lerp(color, waterLineColor, 0.3 * clamp((lw - abs(t3)) * aa, 0.0, 1.0));
    color = lerp(color, waterLineColor, 0.2 * clamp((lw - abs(t4)) * aa, 0.0, 1.0));
    color = lerp(color, waterLineColor, 0.1 * clamp((lw - abs(t5)) * aa, 0.0, 1.0));

    return lerp(color, shoreLineColor, clamp((lw - abs(t0)) * aa, 0.0, 1.0));
}

float mountainContour(float2 p, float y, float squash, float offset) {
    float contour = -0.2;
    contour -= 0.4 * (1.0 - pow(abs(cos((mountainWidthInv * p.x + offset) * pi)), 1.2));
    contour += 0.5 * fbm1(float2(0.15 * p.x, y));
    contour *= squash;
    return p.y - y + mountainHeight * (contour + 0.4);
}

float3 mountains(float3 color, float2 pos, float offset) {
    float x = mountainWidthInv * pos.x + offset;
    float range = frac(x);
    float antiRange = 1.0 - range;
    x = mountainWidth * ((floor(x) - offset) + 0.5);
    
    float y = mountainHeightInv * pos.y + offset;
    float altitude = frac(y);
    float antiAltitude = 1.0 - altitude;
   	y = mountainHeight * ((floor(y) - offset) + 0.5);
    
    float draw = step(mountainThreshold, fbm1(terrainScaleInv * float2(x, y)));
    float drawLeft = step(mountainThreshold, fbm1(terrainScaleInv * float2(x - mountainWidth, y)));
    float drawRight = step(mountainThreshold, fbm1(terrainScaleInv * float2(x + mountainWidth, y)));
    
    float squash = clamp(4.0 * range * antiRange + drawLeft * step(range, 0.5) + drawRight * step(0.5, range), 0.0, 1.0);
    squash *= squash;
    draw *= clamp(mountainWidth * range * aa + drawLeft, 0.0, 1.0);
    draw *= clamp(mountainWidth * antiRange * aa + drawRight, 0.0, 1.0);
    
   	float ter = mountainContour(pos, y, squash, offset);
    float tdx = mountainContour(pos + float2(0.01, 0.0), y, squash, offset) - ter;
    float tdy = mountainContour(pos + float2(0.0, 0.01), y, squash, offset) - ter;
    float tgrad = 0.01 / length(float2(tdx, tdy));
    float t0 = ter * tgrad;

    float3 fillColor = lerp(landColor, mountainColor, altitude);
    fillColor = lerp(fillColor, mountainLineColor, clamp(8.0 * (0.1 - abs(fbm1(0.25 * pos))), 0.0, 1.0));
    fillColor = lerp(color, fillColor, smoothstep(0.0, 0.3, altitude)); 
    color = lerp(color, fillColor, step(ter, 0.0) * draw);
    
    float shadow = step(0.0, t0) * antiAltitude * antiAltitude * draw * squash;
    color = lerp(color, mountainShadowColor, shadow);
    return lerp(color, mountainLineColor, clamp((lw - abs(t0)) * aa, 0.0, 1.0) * draw);
}

float treeSDF(float2 pos, float2 trunk) {
    float3 h = hash3(trunk);
    float r = treeScale * (0.9 + 0.35 * h.z);
    trunk += treeScale * 0.5 * (h.xy - 0.5);
    float terrain = fbm1(terrainScaleInv * trunk);
    float plant = step(treeTerrainThresholdLow, terrain) * step(terrain, treeTerrainThresholdHigh);
    plant *= step(treeGrowthThreshold, fbm1(treeGrowthScaleInv * trunk + 100.0));
    return r - distance(pos, trunk) - 100.0 * (1.0 - plant);
}

float3 trees(float3 color, float2 pos) {
    float2 center = treeScale * (floor(treeScaleInv * pos) + 0.5);
    float shadow = treeSDF(pos, center);
    shadow = max(shadow, treeSDF(pos, center + float2(treeScale, 0.0)));
    shadow = max(shadow, treeSDF(pos, center + float2(treeScale, treeScale)));
    shadow = max(shadow, treeSDF(pos, center + float2(0.0, treeScale)));
    shadow = max(shadow, treeSDF(pos, center + float2(-treeScale, treeScale)));
    shadow = max(shadow, treeSDF(pos, center + float2(-treeScale, 0.0)));
    
    color = lerp(color, treeShadowColor, step(0.0, shadow));
    color = lerp(color, treeLineColor, clamp((lw - abs(shadow)) * aa, 0.0, 1.0));
    
    pos.y -= treeGrow;
    float2 trunk = treeScale * (floor(treeScaleInv * pos) + 0.5);
    float forrest = treeSDF(pos, trunk);
    forrest = max(forrest, treeSDF(pos, trunk + float2(treeScale, 0.0)));
    forrest = max(forrest, treeSDF(pos, trunk + float2(treeScale, treeScale)));
    forrest = max(forrest, treeSDF(pos, trunk + float2(0.0, treeScale)));
    forrest = max(forrest, treeSDF(pos, trunk + float2(-treeScale, treeScale)));
    forrest = max(forrest, treeSDF(pos, trunk + float2(-treeScale, 0.0)));
    forrest = max(forrest, treeSDF(pos, trunk + float2(-treeScale, -treeScale)));
    forrest = max(forrest, treeSDF(pos, trunk + float2(0.0, -treeScale)));
    forrest = max(forrest, treeSDF(pos, trunk + float2(treeScale, -treeScale)));

    color = lerp(color, treeColor * (1.0 - exp(-0.5 - clamp(forrest, 0.0, 10.0))), step(0.0, forrest));
    return lerp(color, treeLineColor, clamp((lw - abs(forrest)) * aa, 0.0, 1.0));
}

#ifdef DRAW_TOWNS
float3 building(float3 color, float2 pos, float2 county, float population, float3 roofColor, float2 block) {
    float2 b = buildingSize * block;
    float terrain = fbm1(terrainScaleInv * b);
    float free = step(townTerrainThresholdLow, terrain) * step(terrain, townTerrainThresholdHigh);
    free *= step(fbm1(treeGrowthScaleInv * b + 100.0), townTreeThreshold);
    free *= step(hash1(block), 1.2 - distance(b, county) / ((1.0 + population) * urbanSprawl));
    
    float3 buildingHash = hash3(block);
    float2 size = float2(0.3, 0.1) + float2(0.25, 0.25) * buildingHash.xy;
    float2 center = float2(1.2 * (buildingHash.z - 0.5), size.y - 0.6);
    float2 p = pos - block - center;
    
    float2 roofHash = hash2(block);
    float roofWidth = size.x * (0.4 + 0.6 * roofHash.x);
    float roofside = 1.0 - 2.0 * step(roofHash.y, 0.5);
    float _triangle_ = sdTriangleIsosceles(p - float2(roofside * (roofWidth - size.x), 0.4 + size.y), float2(roofWidth, -0.4));
    float sdf = sdBox(p, size);
    sdf = min(sdf, _triangle_);
    sdf -= bsdf;
    
    float roofSdf = min(_triangle_, sdTriangleIsosceles(p - float2(roofside * (size.x - roofWidth), 0.4 + size.y), float2(roofWidth, -0.4)));
    roofSdf = min(roofSdf, sdBox(p - float2(0.0, 0.2 + size.y), float2(size.x - roofWidth, 0.2)));
    roofSdf -= roofSize;
    roofSdf = max(roofSdf, size.y - p.y + bsdf);
    color = lerp(color, roofColor, step(roofSdf, 0.0) * free);
    color = lerp(color, float3(0,0,0), clamp((blw - abs(buildingSize * roofSdf)) * aa, 0.0, 1.0) * free);
    
    float3 sidingHash = hash3(block + 0.1);
    float f=0.2 + 0.3 * sidingHash.x + 0.5 * noise1(float2(6.0, 10.0) * pos);
    float3 siding = float3(f,f,f) + sidingHash.y * float3(0.2, 0.1 + 0.1 * sidingHash.z, 0.0);
    color = lerp(color, siding, step(sdf, 0.0) * free);
    
    float3 windowHash = hash3(block + 0.2);
    float2 windowSize = float2(0.2 + 0.1 * windowHash.x, 0.4 + 0.4 * step(windowHash.y, 0.5));
    float2 windowCenter = float2(0.8 * (0.5 - windowHash.z), 0.4 - windowSize.y);
    float windowSdf = sdBox(p - size * windowCenter, size * windowSize);
    color = lerp(color, 0.5 * roofColor, 0.3 * step(windowSdf, 0.0) * free);
    color = lerp(color, float3(0,0,0), clamp((blw - abs(buildingSize * windowSdf)) * aa, 0.0, 1.0) * free);
    
    return lerp(color, float3(0,0,0), clamp((blw - abs(buildingSize * sdf)) * aa, 0.0, 1.0) * free);
}

float3 town(float3 color, float2 pos) {
    float2 county = townSpacing * (floor(townSpacingInv * pos) + 0.5);
    float3 h = hash3(county);
    float3 roofColor = 0.5 + 0.2 * cos(6.2831853 * (float3(0.0, 0.33, 0.67) + h.x));
    county += townSpacing * 0.6 * (h.xy - 0.5);
    
    float2 grid = buildingSizeInv * pos;
    float2 block = floor(grid) + 0.5;
    float offset = mod(block.x, 2.0) - 0.5;
    block.y += offset * (step(0.5, frac(grid.y)) - 0.5);
    
    float2 p = buildingSizeInv * pos;
    color = building(color, p, county, h.z, roofColor, block + float2(0.0, 1.0));
    color = building(color, p, county, h.z, roofColor, block + float2(-1.0, 0.5));
    color = building(color, p, county, h.z, roofColor, block + float2(1.0, 0.5));
    color = building(color, p, county, h.z, roofColor, block);
    color = building(color, p, county, h.z, roofColor, block + float2(-1.0, -0.5));
    color = building(color, p, county, h.z, roofColor, block + float2(1.0, -0.5));
    color = building(color, p, county, h.z, roofColor, block + float2(0.0, -1.0));
    
    return color;
}
#endif

float4 main(float2 fragCoord : TEXCOORD0) : COLOR0 {
	float4 fragColor;
    float3 center = float3(40.0 * iTime, 0.0, 0.0);
    float3 eye = center + float3(0.0, -40.0 + 30.0 * cos(0.15 * iTime), 120.0 - 50.0 * cos(0.15 * iTime));

#ifdef GO_CRAZY
    center = float3(250.0 * iTime, 0.0, 0.0);
    eye = center + float3(70.0 * sin(iTime), 70.0 * cos(iTime), 150.0 + 100.0 * cos(1.4 * iTime));
#endif

    float zoom = 2.0;
    
    float3 forward = normalize(center - eye);
    float3 right = normalize(cross(forward, float3(0.0, 0.0, 1.0)));
    float3 up = cross(right, forward);
    float2 xy = 2.0 * fragCoord - iResolution.xy;
    float3 ray = normalize(xy.x * right + xy.y * up + zoom * forward * iResolution.y);
    
    float t = -eye.z / ray.z;
    float2 pos = eye.xy + t * ray.xy;
    
    aa = iResolution.y / t;
    lw = lineWidth + 0.5 / aa;
    blw = buildingLineWidth + 0.5 / aa;
    bsdf = blw * buildingSizeInv;
    
    float3 color = water(landColor, pos);

    float mountainRange = 0.5 * step(frac(mountainHeightInv * pos.y), 0.5);
    color = mountains(color, pos, 0.5 - mountainRange);
    color = mountains(color, pos, mountainRange);
    
    color = trees(color, pos);
    
#ifdef DRAW_TOWNS
    color = town(color, pos);
#endif
    
    color = lerp(color, dirtColor, dirt * pow(clamp(fbm1(float2(0.01, 0.02) * pos), 0.0, 1.0), 1.5));
    
    color *= 1.0 + paperTexture * (fbm1high(float2(0.05, 0.1) * pos) - fbm1high(float2(0.05, 0.1) * (pos + float2(0.1, 0.1))));

    float2 uv = 2.0 * fragCoord / iResolution.xy - 1.0;
    color = lerp(vignettingColor, color, vignetting * pow((1.0 - uv.x * uv.x) * (1.0 - uv.y * uv.y), 0.3) + 1.0 - vignetting);
    
    return float4(color, 1.0);
}

