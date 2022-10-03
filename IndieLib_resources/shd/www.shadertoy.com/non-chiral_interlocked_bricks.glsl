// https://www.shadertoy.com/view/slyBzy non-chiral interlocked bricks, 2022 by Jakob Thomsen
// based on https://www.shadertoy.com/view/7dK3D3 Branchless Voxel Raycasting Tex
// using DDA from https://www.shadertoy.com/view/4dX3zl Branchless Voxel Raycasting by fb39ca4
// (with loop optimization by kzy), based on http://lodev.org/cgtutor/raycasting.html
// 3d-texture on voxels addon by jt

// Symmetric non-chiral space filling with cuboids (bricks),
// so that no layer could be moved along either coordinate.
// (This structure refines https://www.shadertoy.com/view/styfRG interlocked grids.)
// Animation shows different parts of the structure.
// Bricks visualized colored or grey, "left-over" cubes shown in white.

// Here non-chiral is supposed to mean that this structure
// does not have a "preferred" diagonal around which it is MORE symmetric (rotationally)
// (contrary to e.g. https://www.shadertoy.com/view/sttBDl stacked pipes).

// It appears that for a maximally symmetric structure "left-over" cubes cannot be avoided.
// Also when trying to simplify this structure either symmetry gets lost or bricks become cubes.
// The only smaller structure with similar properties I could find is
// https://www.shadertoy.com/view/stVfDw interlocked cube tori

// tags: grid, cube, voxel, honeycomb, dda, tiling, tesselation, spacefilling, interlocked, bcc, regular, skew, apeirohedron, non-chiral

#define REPETITIONS 2u

#define BLACK 0u
#define RED (BLACK + 1u)
#define GREEN (RED + 1u)
#define YELLOW (GREEN + 1u)
#define BLUE (YELLOW + 1u)
#define MAGENTA (BLUE + 1u)
#define CYAN (MAGENTA + 1u)
#define WHITE (CYAN + 1u)
#define COLORS (WHITE + 1u)

#define GREY COLORS

vec3 palette(int i)
{
#if 0
    return i == GREY ? vec3(0.5) : vec3((i >> 0u) & 1u, (i >> 1u) & 1u, (i >> 2u) & 1u);
#else
    return vec3(0.5);
#endif
}

bool node(vec3 l) // local coordinates
{
    if(l.y == 1u && l.z == 1u && l.x == 1u) return true;

    return false;
}

bool beam(vec3 l) // local coordinates
{
    if(l.y == 1u && l.z == 1u && l.x > 1u) return true;

    return false;
}

// two half-patterns fit together to fill one cube exactly in a body-centred cubic (BCC) lattice pattern.
int half_pattern(vec3 i, int stage)
{
    stage = stage & 15u; // using bit-operators to avoid modulo-bugs on windows

    //if(any(lessThan(i, ivec3(0)))) return 0u;
    vec3 l = vec3(i) & 7u; // using bit-operators to avoid modulo-bugs on windows

    if(l.x >= 4u) l.x = 7u - l.x; // mirror
    if(l.y >= 4u) l.y = 7u - l.y; // mirror
    if(l.z >= 4u) l.z = 7u - l.z; // mirror

    vec2 o = vec2(0,1); // offset

    if(node(l+o.yyy)) return WHITE;
    if(stage >= 0u) if(node(l+o.xxx)) return WHITE;
    if(stage >= 1u) if(node(l+o.yxx)) return GREY;
    if(stage >= 2u) if(node(l+o.xyx)) return GREY;
    if(stage >= 3u) if(node(l+o.xxy)) return GREY;

    if(stage >= 4u) if(beam(l.xyz+o.yyy)) return RED;
    if(stage >= 5u) if(beam(l.yzx+o.yyy)) return GREEN;
    if(stage >= 6u) if(beam(l.zxy+o.yyy)) return BLUE;
    
    if(stage >= 7u) if(beam(l.xyz+o.xxx)) return RED;
    if(stage >= 8u) if(beam(l.yzx+o.xxx)) return GREEN;
    if(stage >= 9u) if(beam(l.zxy+o.xxx)) return BLUE;
    
    if(stage >= 10u) if(beam(l.xyz+o.xxy)) return CYAN;
    if(stage >= 11u) if(beam(l.yzx+o.xxy)) return MAGENTA;
    if(stage >= 12u) if(beam(l.zxy+o.xxy)) return YELLOW;

    if(stage >= 13u) if(beam(l.xyz+o.xyx)) return CYAN;
    if(stage >= 14u) if(beam(l.yzx+o.xyx)) return MAGENTA;
    if(stage >= 15u) if(beam(l.zxy+o.xyx)) return YELLOW;

    return 0u;
}

// Fills one unit cell (composed of 6^3 voxels).
int pattern(vec3 i, int stage)
{
    // NOTE: using bit-operators rather than modulo because of % bugs on windows

    stage = stage & 31u;

    {
        int c = half_pattern(i, stage);
        if(c > 0u)
            return c;
    }

    if((stage >> 4u) != 0u)
    {
        int c = half_pattern(i+4, stage);
        if(c > 0u)
            return c;
    }

    return 0u;
}

#define PI 3.1415926

float checker(vec3 p)
{
    //return step(0.5, length(1.0 - abs(2.0 * fract(p) - 1.0))); // dots
    return step(0.0, sin(PI * p.x + PI/2.0)*sin(PI *p.y + PI/2.0)*sin(PI *p.z + PI/2.0));
    //return step(0.0, sin(p.x)*sin(p.y)*sin(p.z));
}

mat2 rotate(float t)
{
    return mat2(vec2(cos(t), sin(t)), vec2(-sin(t), cos(t)));
}

float sdSphere(vec3 p, float d)
{ 
    return length(p) - d;
} 

float sdBox( vec3 p, vec3 b )
{
    vec3 d = abs(p) - b;
    return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}

bool getVoxel(vec3 c, int stage)
{
    if(any(greaterThan(abs(c), vec3(REPETITIONS * 6u)))) return false;
    return pattern(c, stage) > 0u;
}

// "The raycasting code is somewhat based around a 2D raycasting toutorial found here: 
//  http://lodev.org/cgtutor/raycasting.html" (fb39ca4)

#define MAX_RAY_STEPS (24u * (1u << REPETITIONS))

void main( )
{
    vec2 fragCoord = xlv_TEXCOORD0.xy;
    int stage = int(floor(iTime / 2.0)) + 15u/*for interesting preview image*/;

    vec2 screenPos = 2.0 * fragCoord.xy / iResolution.y - 1.0;
    screenPos.x -= 0.5 * iResolution.x/iResolution.y;
    vec3 rayDir = vec3(screenPos.x, screenPos.y, 2.0);
    vec3 rayPos = vec3(0.0, 0.0, -float(REPETITIONS * 20u));

    float mx = 2.0 * PI * float(-iMouse.x) / float(iResolution.x);
    float my = PI * float(-iMouse.y) / float(iResolution.y);
    mx = (iMouse.x > 10.0) ? mx : 2.0 * PI * fract(iTime * 0.025) + (PI/4.0)/*for interesting preview image*/;
    my = (iMouse.y > 10.0) ? my : PI / 2.0 + (PI/4.0)/*for interesting preview image*/;

    mat2 S = mat2(vec2(cos(my), sin(my)), vec2(-sin(my), cos(my)));
    rayPos.yz = S * rayPos.yz;
    rayDir.yz = S * rayDir.yz;

    mat2 R = mat2(vec2(cos(mx), sin(mx)), vec2(-sin(mx), cos(mx)));
    rayPos.xy = R * rayPos.xy;
    rayDir.xy = R * rayDir.xy;
    
    rayDir = normalize(rayDir);

    ivec3 mapPos = ivec3(floor(rayPos + 0.));

    vec3 color = vec3(1.0);
    vec3 sideDist;
    bvec3 mask;
    // core of https://www.shadertoy.com/view/4dX3zl Branchless Voxel Raycasting by fb39ca4 (somewhat reduced)
    vec3 deltaDist;
    {
        deltaDist = 1.0 / abs(rayDir);
        ivec3 rayStep = ivec3(sign(rayDir));
        sideDist = (sign(rayDir) * (vec3(mapPos) - rayPos) + (sign(rayDir) * 0.5) + 0.5) * deltaDist; 

        for (int i = 0u; i < MAX_RAY_STEPS; i++)
        {
            if (getVoxel(mapPos, stage)) break; // forked shader used continue here

            //Thanks kzy for the suggestion!
            mask = lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy));
            sideDist += vec3(mask) * deltaDist;
            mapPos += ivec3(vec3(mask)) * rayStep;
        }

        color *= mask.x ? vec3(0.25) : mask.y ? vec3(0.5) : mask.z ? vec3(0.75) : vec3(0.0);
    }

    fragColor = vec4(0);
    
    if(any(greaterThan(abs(mapPos), ivec3(REPETITIONS * 6u))))
        return;

    color *= palette(pattern(mapPos, stage));

    // jt's 3d-texture addon recovering distance & subvoxel intersection-position of ray 
    // as described in https://lodev.org/cgtutor/raycasting.html (see "perpWallDist" there)
    //float d = (mask.x ? sideDist.x - deltaDist.x : mask.y ? sideDist.y - deltaDist.y : mask.z ? sideDist.z - deltaDist.z : 0.0) / length(rayDir);
    //float d = length(vec3(mask) * (sideDist - deltaDist)) / length(rayDir); // rayDir not normalized
    float d = length(vec3(mask) * (sideDist - deltaDist)); // rayDir normalized

    vec3 dst = rayPos + rayDir * d;    

    //color *= smoothstep(0.6,0.61, distance(dst, vec3(mapPos)+0.5));
    color += 0.05*(1.0-smoothstep(0.6,0.61, distance(dst, vec3(mapPos)+0.5)));

    vec3 fogcolor = vec3(0.25, 0.4, 0.5); // fog
    //vec3 fogcolor = vec3(0.75, 0.6, 0.3); // smog
    color *= mix(fogcolor, color, exp(-d * d / 200.0)); // fog for depth impression & to suppress flickering

    //fragColor = vec4(color, 1.0);
	gl_FragColor = vec4(sqrt(color), 1.0);
}
