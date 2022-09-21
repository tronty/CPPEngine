// reuse "swirls on sphere"  https://shadertoy.com/view/ssGSDh

#define H(p)       fract(sin((p)*mat3(127.1,311.7, 74.7,  269.5,183.3,246.1,  113.5,271.9,124.6))*43758.5453123)
#define R(p,a,r) ( mix( a*dot(p,a), p, cos(r) ) + sin(r)*cross(p,a) )
#define hue(v)     vec4( .6 + .6 * cos( 6.3*(v)  + vec3(0,23,21)  ) , 1 )
#define S(v)       smoothstep( 0., -6./R.y, v )
#define shad(P)    vec4(vec3( .2 + .8* max(0., dot(P,vec3(1,.5,1)))), 1)

void main( )
{
    vec4 O;
    vec2 R = iResolution.xy, S,
         U = 2.* ( 2.*xlv_TEXCOORD0 - R ) / R.y;

    O-=O;
    float r = length(U), y, l=9., t = -iTime; int i,k;
    vec3 P;
#define moon(d,v,w,h)                                         \
        S = d* vec2(2.*cos(v*t), .2*sin(v*t) );               \
        if ( r > 1. || S.y < 0.)                              \
            if ( length( S = (U-S)/w ) < 1.)                  \
                P = vec3( S, sqrt(1.-dot(S,S)) ),             \
                O +=    S( length(S) - 1. )                   \
                      * shad(P) * (.7+.3*hue(h)) 
    moon(1., 1.,.1,.0);
    moon(2.,.4,.07,.5);
    moon(3.,.2,.05,.3);
    moon(4.,.15,.03,.0);
    moon(5.,.1,.03,.5);
    
    if ( r > 1. ) return;
    
    vec3 P0 = vec3( U, sqrt(1.-r*r) ),         // raytracing point in sphere
         I,F, H,D,A;
         P = 4.*P0;
         P.xz *= mat2(cos( .2*iTime + vec4(0,11,33,0) )); // planet rotation
         I = floor(P), F = fract(P);           // coords in 3D grid

#define dist2seed  \
        D = vec3( k%3, (k/3)%3, k/9 ) -1.;     /* cell offset         */  \
        D += H(I+D)-.5;                        /* random seed point   */  \
        A = normalize(I+D);                                               \
        D = 4.*A - I;                          /* project it on sphere ( for even radius ) */ \
        r = length(F-D);                       /* distance to seed    */

    for ( k = 0; k < 27; k++) {                // visit neighbor cells to find closest seed point
        dist2seed;
     // P = R( F-D, A, 10.*smoothstep(.5,0.,r) ) + D+I; // shouldn't we blend effects ?
        r < l ? l = r, i = k : i;              // keep closest dot 
     }
    k = i;
    dist2seed;
    y = atan(P.y,length(P.xz));                                   // latitude ( to tune swirl direction & amplitude )
    y = 4.*cos(4.*y);
    P = R( F-D, A, y * smoothstep(.5,0.,r) ) + D+I;               // rotate coords around seed point
       
 // O = vec4(smoothstep(12./R.y,0., l-.2) ); return;              // test: plot seed points
    
    S = vec2( atan(P.z,P.x) - .1*iTime, atan(P.y,length(P.xz)) ); // surface coordinates
//  S = vec2(acos(U.x/sqrt(1.-U.y*U.y)) , asin(U.y) );            // debug
	vec4 stex;//texture(iChannel0, S*vec2(1,3)
    O += (1.-O.a) * mix( 
          smoothstep( .8, .2, stex ) )  // swirled texture
        * (.5+.5*hue(abs(U.y*2./3.14)))                           // color
        , vec4(.7,.8,.9,0), 1.-abs(P0).z )                        // atmosphere
        * S( length(U)-1.)                                        // antialiasing
        * 1.5*shad(P0);                                           // shading
	gl_FragColor=O;
    
}

