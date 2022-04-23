/*

	Cheap Cloud Flythrough 
	----------------------
	
	"Cheap" should probably refer to the quality of the physics in this shader, which is
	virtually non-existent, but it actually refers to the fake cloud algorithm... if you could 
	call it that. :)
	
	This is merely an attempt to give the impression of a cloud flythrough, whilst maintaining	
	a reasonably acceptable framerate. The key to that is keeping the distance field as simple 
	as possible.

	Due to the amount of cost cutting, it's far from  perfect. However, there's still plenty 
	of room for improvement.

    I've used density based accumulation, which in one way or another, uses concepts from many 
	volumetric examples, but particularly from the following:
	
	Cloudy Spikeball - Duke
    https://www.shadertoy.com/view/MljXDw
    // Port from a demo by Las - Worth watching.
    // http://www.pouet.net/topic.php?which=7920&page=29&x=14&y=9
    
    Other shaders worth looking at:

	Clouds - IQ: One of my favorite shaders, and everyone elses, it seems.
	https://www.shadertoy.com/view/XslGRr
		
	Sample Pinning - huwb: Fast, and pretty.
	https://www.shadertoy.com/view/XdfXzn
	
	FakeVolumetricClouds - Kuvkar: Fast, using parallax layers. Really cool.
	https://www.shadertoy.com/view/XlsXzN

    Emission clouds - Duke: Nice, and straight forward.
    https://www.shadertoy.com/view/ltBXDm


*/

// random/hash function              
float hash( float n )
{
  return fract(cos(n)*41415.92653);
}

// 3d noise function
float pn( in vec3 x )
{
  vec3 p  = floor(x);
  vec3 f  = smoothstep(0.0, 1.0, fract(x));
  float n = p.x + p.y*57.0 + 113.0*p.z;

  return mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
    mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
    mix(mix( hash(n+113.0), hash(n+114.0),f.x),
    mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
}

#if 0
// IQ's texture lookup noise... in obfuscated form. There's less writing, so
// that makes it faster. That's how optimization works, right? :) Seriously,
// though, refer to IQ's original for the proper function.
// 
// By the way, you could replace this with the non-textured version, and the
// shader should run at almost the same efficiency.
uniform sampler2D iChannel0;
float pn_original( in vec3 p ){
    
    vec3 i = floor(p); p -= i; p *= p*(3. - 2.*p);
	p.xy = texture(iChannel0, (p.xy + i.xy + vec2(37, 17)*i.z + .5)/256., -100.).yx;
	return mix(p.x, p.y, p.z);
}
#endif


// Basic low quality noise consisting of three layers of rotated, mutated 
// trigonometric functions. Needs work, but sufficient for this example.
float trigNoise3D(in vec3 p){

    
    float res = 0., sum = 0.;
    
    // IQ's cheap, texture-lookup noise function. Very efficient, but still 
    // a little too processor intensive for multiple layer usage in a largish 
    // "for loop" setup. Therefore, just one layer is being used here.
    float n = pn(p*8. + iTime*2.);


    // Two sinusoidal layers. I'm pretty sure you could get rid of one of 
    // the swizzles (I have a feeling the GPU doesn't like them as much), 
    // which I'll try to do later.
    
    vec3 t = sin(p.yzx*3.14159265 + cos(p.zxy*3.14159265+1.57/2.))*0.5 + 0.5;
    p = p*1.5 + (t - 1.5); //  + iTime*0.1
    res += (dot(t, vec3(0.333)));

    t = sin(p.yzx*3.14159265 + cos(p.zxy*3.14159265+1.57/2.))*0.5 + 0.5;
    res += (dot(t, vec3(0.333)))*0.7071;    
	 
	return ((res/1.7071))*0.85 + n*0.15;
}

// Distance function.
float map(vec3 p) {

    return trigNoise3D(p*0.5);
    
    // Three layers of noise, for comparison.
    //p += iTime;
    //return pn(p*.75)*0.57 + pn(p*1.875)*0.28 + pn(p*4.6875)*0.15;
}

#define rot(spin) mat2(cos(spin),sin(spin),-sin(spin),cos(spin))

void main( )
{  

    // Unit direction ray vector: Note the absence of a divide term. I came across
    // this via a comment Shadertoy user "coyote" made. I'm pretty easy to please,
    // but I thought it was pretty cool.
    vec3 rd = normalize(vec3(xlv_TEXCOORD0 - iResolution.xy*.5, iResolution.y*.75)); 


    //if (length(iMouse.xy) > 40.0) {
        rd.xz *= rot(-3.14+iMouse.x/iResolution.x*3.14*2.0);
        rd.yz *= rot(-3.14*0.5+iMouse.y/iResolution.y*3.14);
    //}
    // Ray origin. Moving along the Z-axis.
    vec3 ro = vec3(0, 0, iTime*4.);

    // Cheap camera rotation.
    //
    // 2D rotation matrix. Note the absence of a cos variable. It's there, but in disguise.
    // This one came courtesy of Shadertoy user, "Fabrice Neyret."
    vec2 a = sin(vec2(1.5707963, 0) + iTime*0.1875);
    mat2 rM = mat2(a, -a.y, a.x);
    rd.xy = rd.xy*rM; // Apparently, "rd.xy *= rM" doesn't work on some setups. Crazy.
    rd.xz = rd.xz*rM;



    // Placing a light in front of the viewer and up a little, then rotating it in sync
    // with the camera. I guess a light beam from a flying vehicle would do this.
    vec3 lp = vec3( 0, 1, 4);
    lp.xy = lp.xy*rM;
    lp.xz = lp.xz*rM;
    lp += ro;

    // The ray is effectively marching through discontinuous slices of noise, so at certain
    // angles, you can see the separation. A bit of randomization can mask that, to a degree.
    // At the end of the day, it's not a perfect process. Note, the ray is deliberately left 
    // unnormalized... if that's a word.
    //
    // Randomizing the direction.
    //rd = (rd + ((rd.zyx)*.006 - .003)); 
    // Randomizing the length also. 
    rd *= (1. + fract((dot(vec3(7, 157, 113), rd.zyx))*43758.5453)*0.06-0.03);      
    
    // Local density, total density, and weighting factor.
    float lDe = 0., td = 0., w = 0.;

    // Closest surface distance, and total ray distance travelled.
    float d = 1., t = 0.;

    // Distance threshold. Higher numbers give thicker clouds, but fill up the screen too much.    
    const float h = .5;


    // Initializing the scene color to black, and declaring the surface position vector.
    vec3 col = vec3(0), sp;



    // Particle surface normal.
    //
    // Here's my hacky reasoning. I'd imagine you're going to hit the particle front on, so the normal
    // would just be the opposite of the unit direction ray. However particles are particles, so there'd
    // be some randomness attached... Yeah, I'm not buying it either. :)
    vec3 sn = normalize(-rd);

    // Raymarching loop.
    for (int i=0; i<64; i++) {

        // Loop break conditions. Seems to work, but let me
        // know if I've overlooked something.
        if((td>1.) || d<.01*t || t>80.)break;


        sp = ro + rd*t; // Current ray position.
        d = map(sp); // Closest distance to the surface... particle.

        // If we get within a certain distance, "h," of the surface, accumulate some surface values.
        // The "step" function is a branchless way to do an if statement, in case you're wondering.
        //
        // Values further away have less influence on the total. When you accumulate layers, you'll
        // usually need some kind of weighting algorithm based on some identifying factor - in this
        // case, it's distance. This is one of many ways to do it. In fact, you'll see variations on 
        // the following lines all over the place.
        //
        lDe = (h - d)*step(d, h); 
        w = (1. - td)*lDe;   

        // Use the weighting factor to accumulate density. How you do this is up to you. 
        td += w*w*8. + 1./64.; //w*w*5. + 1./50.;
        //td += w*.4 + 1./45.; // Looks cleaner, but a little washed out.


        // Point light calculations.
        vec3 ld = lp-sp; // Direction vector from the surface to the light position.
        float lDist = max(length(ld), .001); // Distance from the surface to the light.
        ld/=lDist; // Normalizing the directional light vector.

        // Using the light distance to perform some falloff.
        float atten = 1./(1. + lDist*.125 + lDist*lDist*.05);

        // Ok, these don't entirely correlate with tracing through transparent particles,
        // but they add a little anglular based highlighting in order to fake proper lighting...
        // if that makes any sense. I wouldn't be surprised if the specular term isn't needed,
        // or could be taken outside the loop.
        float diff = max(dot( sn, ld ), 0.);
        float spec = pow(max( dot( reflect(-ld, sn), -rd ), 0. ), 4.);


        // Accumulating the color. Note that I'm only adding a scalar value, in this case,
        // but you can add color combinations. Note the "d*3. - .1" term. It's there as a bit
        // of a fudge to make the clouds a bit more shadowy.
        col += w*(d*3. - .1)*(.5 + diff + spec*.5)*atten;

        // Try this instead, to see what it looks like without the fake contrasting. Obviously,
        // much faster.
        //col += w*atten*1.25;


        // Enforce minimum stepsize. This is probably the most important part of the procedure.
        // It reminds me a little of of the soft shadows routine.
        t +=  max(d*.5, .02); //
        // t += .2; // t += d*.5;// These also work, but don't seem as efficient.

    }

    col = max(col, 0.);

    // trigNoise3D(rd*1.)
    col = mix(pow(vec3(1.5, 1, 1)*col,  vec3(1, 2, 8)), col, dot(cos(rd*6. +sin(rd.yzx*6.)), vec3(.333))*.35 + .65);
    col = mix(col.zyx, col, dot(cos(rd*9. +sin(rd.yzx*9.)), vec3(.333))*.15 + .85);//xzy
    

    //col = mix(col.zyx, col, dot(rd, vec3(.5))+.5);

    gl_FragColor = vec4(sqrt(max(col, 0.)), 1.0);
}
