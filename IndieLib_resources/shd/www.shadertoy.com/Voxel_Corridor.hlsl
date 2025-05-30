/*
  Copyright (c) 2022 Tuomas Roenty   http://www.tuomasronty.fi/
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

// https://www.shadertoy.com/view/MdVSDh
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
float3      iResolution=float3(1,1,1);                  // viewport resolution (in pixels)
float     iTime=0;                        // shader playback time (in seconds)
float4      iMouse=float4(1,1,1,1);                       // mouse pixel coords
sampler2D iChannel3;

/*

	Voxel Corridor
	--------------

	I love the voxel aesthetic, so after looking at some of Akohdr's examples, I went on a bit 
	of a voxel trip and put this simple scene together... Although, "scene" would  be putting 
	it loosely. :)

	Quasi-discreet distance calculations sound simple enough to perform in theory, but are just 
	plain fiddly to code, so I was very thankful to have fb39ca4's, IQ's, Reinder's, and everyone 
	elses voxel examples to refer to.

	The code is pretty straight forward. I tried my best to write it in such way that enables
	someone to plug in any normal distance function and have it render the voxelized version.

	Mainly based on the following:

	Voxel Ambient Occlusion - fb39ca4
    https://www.shadertoy.com/view/ldl3DS

	Minecraft - Reinder
    https://www.shadertoy.com/view/4ds3WS

	Other examples:
	Rounded Voxels - IQ
    https://www.shadertoy.com/view/4djGWR

	Sampler - w23
	https://www.shadertoy.com/view/MlfGRM

	Text In Space - akohdr
	https://www.shadertoy.com/view/4d3SWB

*/

#define PI 3.14159265
#define FAR 60.

// 2x2 matrix rotation. Note the absence of "cos." It's there, but in disguise, and comes courtesy
// of Fabrice Neyret's "ouside the box" thinking. :)
float2x2 rot2( float a ){ float2 v = sin(float2(1.570796, 0) + a);	return float2x2(v, -v.y, v.x); }

// Tri-Planar blending function. Based on an old Nvidia tutorial.
float3 tex3D( sampler2D tex, in float3 p, in float3 n ){
  
    n = max(abs(n), 0.001);//n = max((abs(n) - 0.2)*7., 0.001); //  etc.
    n /= (n.x + n.y + n.z ); 
	p = (tex2D(tex, p.yz)*n.x + tex2D(tex, p.zx)*n.y + tex2D(tex, p.xy)*n.z).xyz;
    return p*p;
}

// The path is a 2D sinusoid that varies over time, depending upon the frequencies, and amplitudes.
float2 path(in float z){ 
    //return float2(0); // Straight.
    float a = sin(z * 0.11);
    float b = cos(z * 0.14);
    return float2(a*4. -b*1.5, b*1.7 + a*1.5); 
    //return float2(a*4. -b*1.5, 0.); // Just X.
    //return float2(0, b*1.7 + a*1.5); // Just Y.
}

/*
// Alternate distance field -- Twisted planes. 
float map(float3 p){
    
     // You may need to reposition the light to work in with the shadows, but for
     // now, I'm repositioning the scene up a bit.
     p.y -= .75;
     p.xy -= path(p.z); // Move the scene around a sinusoidal path.
     p.xy = rot2(p.z/8.)*p.xy; // Twist it about XY with respect to distance.
    
     float n = dot(sin(p*1. + sin(p.yzx*.5 + iTime*.0)), float3(.25)); // Sinusoidal layer.
     
     return 4. - abs(p.y) + n; // Warped double planes, "abs(p.y)," plus surface layers.
 
}
*/

// Standard perturbed tunnel function.
//
float map(float3 p){
     
     // Offset the tunnel about the XY plane as we traverse Z.
     p.xy -= path(p.z);
    
     // Standard tunnel. Comment out the above first.
     float n = 5. - length(p.xy*float2(1, .8));
     // Straight up and down walls.
     //float n = 5. - max(abs(p.x), abs(p.y*.8));
    
     // Square tunnel. Almost redundant in a voxel renderer. :)
     //n = 4. - max(abs(p.x), abs(p.y)); 
     
     // Tunnel with a floor.
     return min(p.y + 3., n); //n = min(-abs(p.y) + 3., n);
 
}

/*
float brickShade(float2 p){
    
    p.x -= step(p.y, 1.)*.5;
    
    p = frac(p);
    
    return pow(16.*p.x*p.y*(1.-p.x)*(1.-p.y), 0.25);
    
}
*/

// The brick groove pattern. Thrown together too quickly.
// Needs some tidy up, but it's quick enough for now.
//
//const
float w2h = 2.; // Width to height ratio.
//const
float mortW = 0.05; // Morter width.

float brickMorter(float2 p){
	
    p.x -= step(1., p.y)*.5;
    
    p = abs(frac(p + float2(0, .5)) - .5)*2.;
    
    // Smooth grooves. Better for bump mapping.
    return smoothstep(0., mortW, p.x)*smoothstep(0., mortW*w2h, p.y);
    
}

float brick(float2 p){
    
	p = frac(p*float2(0.5/w2h, 0.5))*2.;

    return brickMorter(p);//*(brickShade(p)*.25 + .75);
}


// Surface bump function. Cheap, but with decent visual impact.
float bumpSurf3D( in float3 p, in float3 n){

    n = abs(n);
    
    if (n.x>0.5) p.xy = p.zy;
    else if (n.y>0.5) p.xy = p.xz;
    
    return brick(p.xy);
    
}

// Standard function-based bump mapping function.
float3 doBumpMap(in float3 p, in float3 nor, float bumpfactor){
    
    const float2 e = float2(0.001, 0);
    float ref = bumpSurf3D(p, nor);                 
    float3 grad = (float3(bumpSurf3D(p - e.xyy, nor),
                      bumpSurf3D(p - e.yxy, nor),
                      bumpSurf3D(p - e.yyx, nor) )-ref)/e.x;                     
          
    grad -= nor*dot(nor, grad);          
                      
    return normalize( nor + grad*bumpfactor );
	
}

// Texture bump mapping. Four tri-planar lookups, or 12 tex2D lookups in total. I tried to 
// make it as concise as possible. Whether that translates to speed, or not, I couldn't say.
float3 doBumpMap( sampler2D tx, in float3 p, in float3 n, float bf){
   
    const float2 e = float2(0.001, 0);
    
    // Three gradient vectors rolled into a matrix, constructed with offset greyscale tex2D values.    
    float3x3 m = float3x3( tex3D(tx, p - e.xyy, n), tex3D(tx, p - e.yxy, n), tex3D(tx, p - e.yyx, n));
    
    float3 g = mul(m, float3(0.299, 0.587, 0.114));// ??? *m; // Converting to greyscale.
    g = (g - dot(tex3D(tx,  p , n), float3(0.299, 0.587, 0.114)) )/e.x; g -= n*dot(n, g);
                      
    return normalize( n + g*bf ); // Bumped normal. "bf" - bump factor.
    
}


// This is just a slightly modified version of fb39ca4's code, with some
// elements from IQ and Reinder's examples. They all work the same way:
// Obtain the current voxel, then test the distance field for a hit. If
// the ray has moved into the voxelized isosurface, break. Otherwise, move
// to the next voxel. That involves a bit of decision making - due to the
// nature of voxel boundaries - and the "mask," "side," etc, variable are
// an evolution of that. If you're not familiar with the process, it's 
// pretty straight forward, and there are a lot of examples on Shadertoy, 
// plus a lot more articles online.
//
float3 voxelTrace(float3 ro, float3 rd, out float3 mask){
    
    float3 p = floor(ro) + .5;

	float3 dRd = 1./abs(rd); // 1./max(abs(rd), float3(.0001));
	rd = sign(rd);
    float3 side = dRd*(rd*(p - ro) + .5);
    
    mask = float3(0, 0, 0);
	
	for (int i = 0; i < 64; i++) {
		
        if (map(p)<0.) break;
        
        // Note that I've put in the messy reverse step to accomodate
        // the "less than or equals" logic, rather than just the "less than."
        // Without it, annoying seam lines can appear... Feel free to correct
        // me on that, if my logic isn't up to par. It often isn't. :)
        mask = step(side, side.yzx)*(1. - step(side.zxy, side));
		side += mask*dRd;
		p += mask*rd;
	}
    
    return p;    
}


// Voxel shadows. They kind of work like regular hard-edged shadows. They
// didn't present too many problems, but it was still nice to have Reinder's
// Minecraft shadow example as a reference. Fantastic example, if you've
// never seen it:
//
// Minecraft - Reinder
// https://www.shadertoy.com/view/4ds3WS
//
float voxShadow(float3 ro, float3 rd, float end){

    float shade = 1.;
    float3 p = floor(ro) + .5;

	float3 dRd = 1./abs(rd);//1./max(abs(rd), float3(.0001));
	rd = sign(rd);
    float3 side = dRd*(rd*(p - ro) + .5);
    
    float3 mask = float3(0, 0, 0);
    
    float d = 1.;
	
	for (int i = 0; i < 16; i++) {
		
        d = map(p);
        
        if (d<0. || length(p - ro)>end) break;
        
        mask = step(side, side.yzx)*(1. - step(side.zxy, side));
		side += mask*dRd;
		p += mask*rd;                
	}

    // Shadow value. If in shadow, return a dark value.
    return shade = step(0., d)*.7 + .3;
    
}

///////////
//
// This is a trimmed down version of fb39ca4's voxel ambient occlusion code with some 
// minor tweaks and adjustments here and there. The idea behind voxelized AO is simple. 
// The execution, not so much. :) So damn fiddly. Thankfully, fb39ca4, IQ, and a few 
// others have done all the hard work, so it's just a case of convincing yourself that 
// it works and using it.
//
// Refer to: Voxel Ambient Occlusion - fb39ca4
// https://www.shadertoy.com/view/ldl3DS
//
float4 voxelAO(float3 p, float3 d1, float3 d2) {
   
    // Take the four side and corner readings... at the correct positions...
    // That's the annoying bit that I'm glad others have worked out. :)
	float4 side = float4(map(p + d1), map(p + d2), map(p - d1), map(p - d2));
	float4 corner = float4(map(p + d1 + d2), map(p - d1 + d2), map(p - d1 - d2), map(p + d1 - d2));
	
    // Quantize them. It's either occluded, or it's not, so to speak.
    side = step(side, float4(0, 0, 0, 0));
    corner = step(corner, float4(0, 0, 0, 0));
    
    // Use the side and corner values to produce a more honed in value... kind of.
    return 1. - (side + side.yzwx + max(corner, side*side.yzwx))/3.;    
	
}

float calcVoxAO(float3 vp, float3 sp, float3 rd, float3 mask) {
    
    // Obtain four AO values at the appropriate quantized positions.
	float4 vAO = voxelAO(vp - sign(rd)*mask, mask.zxy, mask.yzx);
    
    // Use the fractional voxel postion and and the proximate AO values
    // to return the interpolated AO value for the surface position.
    sp = frac(sp);
    float2 uv = sp.yz*mask.x + sp.zx*mask.y + sp.xy*mask.z;
    return lerp(lerp(vAO.z, vAO.w, uv.x), lerp(vAO.y, vAO.x, uv.x), uv.y);

}
///////////

float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
	// Screen coordinates.
	float2 uv = (fragCoord - iResolution.xy*0.5)/iResolution.y;
	
	// Camera Setup.
	float3 camPos = float3(0., 0.5, iTime*8.); // Camera position, doubling as the ray origin.
	float3 lookAt = camPos + float3(0.0, 0.0, 0.25);  // "Look At" position.

 
    // Light positioning. 
 	float3 lightPos = camPos + float3(0, 2.5, 8);// Put it a bit in front of the camera.

	// Using the Z-value to perturb the XY-plane.
	// Sending the camera, "look at," and two light vectors down the tunnel. The "path" function is 
	// synchronized with the distance function. Change to "path2" to traverse the other tunnel.
	lookAt.xy += path(lookAt.z);
	camPos.xy += path(camPos.z);
	lightPos.xy += path(lightPos.z);

    // Using the above to produce the unit ray-direction vector.
    float FOV = PI/2.; // FOV - Field of view.
    float3 forward = normalize(lookAt-camPos);
    float3 right = normalize(float3(forward.z, 0., -forward.x )); 
    float3 up = cross(forward, right);

    // rd - Ray direction.
    float3 rd = normalize(forward + FOV*uv.x*right + FOV*uv.y*up);
    
    //float3 rd = normalize(forward + FOV*uv.x*right + FOV*uv.y*up);
    //rd = normalize(float3(rd.xy, rd.z - dot(rd.xy, rd.xy)*.25));    
    
    // Swiveling the camera about the XY-plane (from left to right) when turning corners.
    // Naturally, it's synchronized with the path in some kind of way.
	rd.xy = mul(rot2( path(lookAt.z).x/24. ), rd.xy);

    // Raymarch the voxel grid.
    float3 mask;
	float3 vPos = voxelTrace(camPos, rd, mask);
	
    // Using the voxel position to determine the distance from the camera to the hit point.
    // I'm assuming IQ is responsible for this clean piece of logic.
	float3 tCube = (vPos - camPos - .5*sign(rd))/rd;
    float t = max(max(tCube.x, tCube.y), tCube.z);
    
	
    // Initialize the scene color.
    float3 sceneCol = float3(0, 0, 0);
	
	// The ray has effectively hit the surface, so light it up.
	if(t<FAR){
	
   	
    	// Surface position and surface normal.
	    float3 sp = camPos + rd*t;
        
        // Voxel normal.
        float3 sn = -(mask*sign( rd ));
        
        // Sometimes, it's necessary to save a copy of the unbumped normal.
        float3 snNoBump = sn;
        
        // I try to avoid it, but it's possible to do a tex2D bump and a function-based
        // bump in succession. It's also possible to roll them into one, but I wanted
        // the separation... Can't remember why, but it's more readable anyway.
        //
        // Texture scale factor.
        const float tSize0 = 1./4.;
        // Texture-based bump mapping.
	    sn = doBumpMap(iChannel3, sp*tSize0, sn, 0.02);

        // Function based bump mapping. Comment it out to see the under layer. It's pretty
        // comparable to regular beveled Voronoi... Close enough, anyway.
        sn = doBumpMap(sp, sn, .15);
        
       
	    // Ambient occlusion.
	    float ao = calcVoxAO(vPos, sp, rd, mask) ;//calculateAO(sp, sn);//*.75 + .25;

        
    	// Light direction vectors.
	    float3 ld = lightPos-sp;

        // Distance from respective lights to the surface point.
	    float lDist = max(length(ld), 0.001);
    	
    	// Normalize the light direction vectors.
	    ld /= lDist;
	    
	    // Light attenuation, based on the distances above.
	    float atten = 1./(1. + lDist*.2 + lDist*0.1); // + distlpsp*distlpsp*0.025
    	
    	// Ambient light.
	    float ambience = 0.25;
    	
    	// Diffuse lighting.
	    float diff = max( dot(sn, ld), 0.0);
   	
    	// Specular lighting.
	    float spec = pow(max( dot( reflect(-ld, sn), -rd ), 0.0 ), 32.);

	    
	    // Fresnel term. Good for giving a surface a bit of a reflective glow.
        //float fre = pow( clamp(dot(sn, rd) + 1., .0, 1.), 1.);
        
 
        // Object texturing.
        //
        // Obfuscated way to tinge the floor and ceiling with a bit of brown.
	    float3 texCol = float3(1, .6, .4) + step(abs(snNoBump.y), .5)*float3(0,.4, .6);
	    
        // Multiplying by the tex2D color.
	    texCol *= tex3D(iChannel3, sp*tSize0, sn);
        
        //texCol *= bumpSurf3D( sp, sn)*.25 + .75; // Darken the grout, if you wanted.

        
        // Shadows... I was having all sorts of trouble trying the move the ray off off the
        // block. Thanks to Reinder's "Minecraft" example for showing me the ray needs to 
        // be bumped off by the normal, not the unit direction ray. :)
        float shading = voxShadow(sp + snNoBump*.01, ld, lDist);
    	
    	// Combining the above terms to produce the final color. It was based more on acheiving a
        // certain aesthetic than science.
        sceneCol = texCol*(diff + ambience) + float3(.7, .9, 1.)*spec;// + float3(.5, .8, 1)*spec2;
        //sceneCol += texCol*float3(.8, .95, 1)*pow(fre, 4.)*2.; // White mortar... not really.
        


	    // Shading.
        sceneCol *= atten*shading*ao;
        
        // "fb39ca4" did such a good job with the AO, that it's worth a look on its own. :)
        //sceneCol = float3(ao); 

	   
	
	}
       
    // Blend in a bit of logic-defying fog for atmospheric effect. :)
    sceneCol = lerp(sceneCol, float3(.08, .16, .34), smoothstep(0., .95, t/FAR)); // exp(-.002*t*t), etc.

    // Clamp and present the badly gamma corrected pixel to the screen.
	fragColor = float4(sqrt(clamp(sceneCol, 0., 1.)), 1.0);
	return fragColor;
	
}
