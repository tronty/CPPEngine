// fork of Detail Spheres http://shadertoy.com/view/wsjfWK
// removed the detail spheres and replaced with the brickwork
// that I initially developed at http://shadertoy.com/view/tlGXDK

const float
    rad = .3,    // torus large radius
    rad2 = .2,   // torus small radius
    grid = 18.,  // brick scale
    size = .15 / grid, //.01   // detail radius 
    pi = acos(-1.);

float dBox(vec3 q, vec3 e)
{
    q = abs(q) - e;
    return min(0., max(q.x, max(q.y, q.z))) + length(max(q, 0.)); // iq way
}

// iq http://iquilezles.org/www/articles/smin/smin.htm
float smin(float a, float b, float k)
{
	float h = a - b;
    return .5 * (a + b - sqrt(h * h + k));
}
// sqrt version is apparently best
float smax(float a, float b, float k)
{
	return -smin(-a, -b, k);
}

// carve bricks into dlod shape sdf
float dBrickwork(vec3 q, float dlod)
{
    const float s = grid;
    q *= s;
    q *= .5;
    q = fract(q) - .5;
    //if (q.y > 0.) q.xz = q.zx; // this never worked out right
    q.z += .25 * (2. * floor(2. * q.x) + floor(2. * q.y));
    q.z = fract(q.z) - .5;
    //q.xy = abs(q.xy);
    q *= 2.;
    q.xy = fract(q.xy) - .5;
    float d = (dBox(q, vec3(.5,.5,1) - .03) - .01) / s;
    d = smin(d, dlod + .061, 1e-4); // limit depth of mortar cracks
    d = smax(d, dlod, 1e-4); // wow smoothmin makes a heckuva difference!
    return d;
}
// TODO with a normal, could fade out the lines cutting the wrong way trilinear style

// the general shape, uncarved
float dShapeMain(vec3 q) 
{
    float db = max(abs(max(abs(q.x), abs(q.y)) - rad), abs(q.z)) - rad2,
        dt = length(vec2(length(q.xy) - rad, q.z)) - rad2;
        //length(vec2(abs(max(abs(q.x), abs(q.y)) - rad), abs(q.z))) - rad2; //length(q - normalize(q * vec3(1, 1, 0)) * rad) - rad2; // weird way to make a torus but whatevs
    //db = min(db, q.y + .46); // 'floor'
    return q.y < -.2 ? db : q.y > .2 ? dt : mix(db, dt, .5 + .5/.2 * q.y);
}

// shim adding a meta-material sdf to an sdf
// I had idea to do brickworks, tried out, voila
float dScene(vec3 q)
{
    float d = dShapeMain(q);
    if (abs(d) > 2. * size) return d - sign(d) * size;   // details evaluated only near surface
    //vec3 e = ro - q;
    // TODO fade out, don't just clip
    // just do this very near the surface
    if (true && abs(d) < .25) { // && dot(e, e) < 36.) {
        float db = dBrickwork(q.yzx, d);
        db = smin(db, d + .006, 1e-5); // limit depth of mortar cracks
        d = smax(d, db, 1e-5);
    }
    // fine 'texture' bumps just so there's *some* texture at all
    d += .006 * size * dot(vec3(.33), sin(73. * q.yzx * (1. - 79. * q.zxy))) * exp2(-d * d);
    return d;
}
    //float dlod = d - 2. * size;         // bound shell around max detail protrusion
    //if (d > dlod + 3. * size) return dlod; // low LOD for further queries

// might be the most versatile lighting primitive ever
// should probably make the length(L) into an argument, move to caller
float liteHaxx(vec3 p, float v, vec3 L) // given point and known closest distance at point,
{
    float dl = dScene(p + L),          // another sample toward 'light'
        dx = (dl - v) / length(L);     // cheap directional derivative
    return clamp(dx, 0., 1.);          // approximates Lambert shading    
}

// look, ma, no normals!
/* // simple forward difference gradient when you already know the distance at center sample
vec3 gScene(vec3 p, float v, float e)
{
    vec2 f = vec2(e, 0);
    return (vec3(
        dScene(p + f.xyy),
        dScene(p + f.yxy),
        dScene(p + f.yyx)
        ) - v) / e;
}
// but I found an even simpler way that doesn't even require obtaining the normal!
float aoHaxx(vec3 p, float v)
{
    float e = .7 * size;
    vec3 n = normalize(gScene(p, v, e));
    return pow(liteHaxx(p, v, e * n), 2.);
}
*/

void main()
{
    vec2 R = iResolution.xy,
         q = (2. * xlv_TEXCOORD0 - R) / R.y,
         M = iMouse.xy / R;
    if (M == vec2(0)) M = vec2(.5); // unattended
    
    // set camera
    float theta = (.5 - M.x) * pi * 2., 
         phi = (.5 - M.y) * pi;
    if (iMouse.z <= 0.) { // camera animate if no mouselook 
    	float t = .7 * iTime, B = .02;
        theta += B * cos(t) + 2. * B * t; phi += B * sin(t);
    }
    //#define rot(a)     mat2(cos(a), -sin(a), sin(a), cos(a))

    // TODO golf this
    vec3 eye = vec3(sin(theta) * cos(phi), sin(phi), cos(theta) * cos(phi)),
        lookAt = vec3(0),
        W = normalize(eye - lookAt),
        U = vec3(W.z, 0, -W.x),
        rayDir = normalize(mat2x3(U, cross(W, U)) * q - 1.5 * W);
        //U = normalize(cross(vec3(0, 1, 0), W)),
        //V = cross(W, U),
        //rayDir = normalize(q.x * U + q.y * V -1.5 * W);

    // ray-march a radial section,
    // which bounds the scene around 
    // front of big shape for performance.
    float l = .7 - rad - rad2, 
        bound = 1. + rad + rad2 + 2. * size, 
        d = 9e9;
    vec3 p,
        col; // = vec3(0);
    //if (dot(q, q) < 2.) // HACK optimization: don't bother checking details in corners of screen
    // ray-march
    for (float i = 0.; i < 128.; ++i) {
        p = eye + l * rayDir;
        d = dScene(p); 
        l += d; // * .9;
        if (d < .001            // "hit" on an object
         || l > bound + 1.
         || dot(p, p) > bound * bound)
            break;
    }
    
    const vec3
        skyColor = vec3(.3,.5,.7),
        lightDir = normalize(vec3(.8,.3,.2)),
        ambient  = vec3(.15), 
        diffuse  = vec3(.8), 
        specular = vec3(.4);

    //d = min(d, 1e2);
    if (d < .001) {                           // hit
        vec3 c = vec3(1,.8,.6);               // albedo
        float shade = liteHaxx(p, d, .5 * size * lightDir)  // shading
            * sqrt(liteHaxx(p, d, rad * lightDir)); // 'shadow'
        c *= diffuse * shade
            + ambient * liteHaxx(p, d, size * W); // * aoHaxx(p, d)
        c += specular * shade * pow(liteHaxx(p, d, .8 * size * (lightDir + W)), 32.); // why not
 	    col += c;
    } else {
        col += skyColor;
        col += .7 * pow(dot(rayDir, lightDir) * .5 + .5, 4.);
    }
      
    gl_FragColor = vec4(sqrt(col), 1);
}

