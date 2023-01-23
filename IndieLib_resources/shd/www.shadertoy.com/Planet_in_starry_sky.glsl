#define PI 3.14159265359

mat2 Rot(float angle) {
    float s=sin(angle);
    float c=cos(angle);
    return mat2(c, -s, s, c);
}

float hash21( vec2 p )
{
    p = fract( p*vec2( 789.123, 456.987));
    p += dot( p, p + vec2(12.34));
    return fract( p.x*p.y);
}

float circle( vec2 pos, vec2 center, float radius, float width, float smooth_width)
{
    float l = length(pos-center);

    float low_val = width*.5 - smooth_width;
    float high_val = width*.5;
    float tore = smoothstep( high_val, low_val, abs( l - radius));
    return tore;
}

float dirty_circle( vec2 pos, vec2 center, float radius, float width, float smooth_width)
{
    float rand_val = hash21(pos);
    pos = pos + vec2( rand_val, fract(rand_val * 11.)) * 0.01;
    return circle (pos, center, radius, width, smooth_width);
}
vec3 planet_text( vec2 uv )
{
    float nb_circles = 5.;
    float val = 0.;

    //vec3 col = vec3(1.,.5,.5)*.28; // Redish planet
    //vec3 col = vec3(0.1,.3,.6)*.28;  // bluesish planet
    vec3 col = vec3(.2,.6,.4)*.28;   // greenish planet
    
    for( float i = 0.001; i < nb_circles; i+= 1.)
    {
        float rand_val = hash21( vec2(i, i*.123));
        float rand_val1 = fract( rand_val * 13.);
        float rand_val2 = fract( rand_val * 53.);
        float rand_val3 = fract( rand_val * 111.);
        vec2 center = vec2( rand_val -.5, rand_val1 -.5);
        center.x *= 1.7;
        float radius = .3- ( rand_val2 -.5) * 0.3;
        float width = .2- ( rand_val3 -.5) * 0.05;
        float tore = dirty_circle( uv, center, radius, width, 0.1);
        //float tore = circle( uv, center, radius, width, 0.1);
        
        val = max(tore, val );
    }
    nb_circles = 40.;
    for( float i = 0.00001; i < nb_circles; i+= 1.)
    {
        float rand_val = hash21( vec2(i*.321, i*.456));
        float rand_val1 = fract( rand_val * 13.);
        float rand_val2 = fract( rand_val * 53.);
        float rand_val3 = fract( rand_val * 111.);
        vec2 center = vec2( rand_val -.5, rand_val1 -.5);
        center.x *= 1.7;
        center.y *= 2.1;
        float radius = .1- ( rand_val2 -.5) * 0.1;
        float width = .1- ( rand_val3 -.5) * 0.2;
        float tore = dirty_circle( uv, center, radius, width, 0.1);
        //float tore = circle( uv, center, radius, width, 0.1);
        
        val = max(tore, val );
    }
    
    return vec3(val)+col;
}

vec3 planet( vec2 uv, vec3 local_y, vec3 local_z )
{
    vec3 local_x = cross( local_y, local_z);


    float dist_from_center = length(uv);
    float dist_from_circle_plane = sqrt( 1. - dist_from_center * dist_from_center );
    
    
    vec3 pos_on_sphere = vec3( uv.x, uv.y, dist_from_circle_plane);
    vec2 proj_on_equator_plane = vec2( dot( pos_on_sphere, local_x), dot( pos_on_sphere, local_z) );
    float long_angle = atan( proj_on_equator_plane.y, proj_on_equator_plane.x );
    
    vec3 proj_eq_plane = dot( pos_on_sphere, local_x) * local_x + dot( pos_on_sphere, local_z) * local_z;
    float lat_angle= atan( dot( pos_on_sphere, local_y), dot( pos_on_sphere, proj_eq_plane ) );
    
    vec2 text_uv = vec2( (long_angle+PI)/(2.*PI), ( lat_angle + PI/2.)/PI);
    text_uv = 2.*text_uv -1.;
    
    vec3 col = vec3(0.);
    
    const float nb_div = 6.;
    float inc = 2. * PI / nb_div;
    for ( float i = 0.;  i < 2.*PI; i+= inc)
    {
        float moved_ux = fract( (text_uv.x + i)*.5 + .5 ) *2. - 1.;
        vec2 cur_uv= vec2(moved_ux, text_uv.y);
        col += planet_text( cur_uv );
    }
    
    const float nb_div2 = 3.;
    inc = 2. * PI / nb_div2;
    float wind = .02*iTime + sin(iTime*.1) * .09;
    for ( float i = 0.;  i < 2.*PI; i+= inc)
    {
        float moved_ux = fract( (0.456 + text_uv.x + i + wind*i)*.5 + .5 ) *2. - 1.;
        vec2 cur_uv= vec2(moved_ux, text_uv.y);
        col += planet_text( cur_uv );
    }
    return col/(nb_div + nb_div2) ;
}

vec3 add_planet( vec2 uv )
{
    vec3 col = vec3(0.2);
    
    // quasi up vect :
    //vec3 axis = vec3 ( sin(iTime) * 0.3, 1.0, cos(iTime*2.));
    vec3 axis = vec3 ( 0.3, 1.0, 0.);
    axis = normalize(axis);
    
    vec3 right_vect = vec3(1.,0.,0.);
    float rotation_speed = 0.35;
    float angle = mod( iTime * rotation_speed, 2.*PI);
    //float angle = 0.;
    right_vect = vec3(cos(angle),0.,sin(angle));
    
    
    vec3 dir = normalize( cross( right_vect, axis ));
    
    col = planet(uv, axis, dir);
    return col;
}


float star(vec2 uv, float flare)
{
    float d = length(uv);
    float col = 0.;
    //col = .001/d/d;
    col = .01/d;
    
    col += max(0., 1.-abs(uv.x * uv.y) * 2000.) * flare;
    uv = Rot(3.14/4.) * uv;
    col += max(0.,1.-abs(uv.x * uv.y) * 2000.) * .4 * flare;
    
    col *= smoothstep(.5, .3, d);
    return col;
}

vec3 starfield( vec2 uv, float flare, vec3 color1, vec3 color2 )
{
    vec2 gv = fract(uv)-.5;
    vec2 n = floor(uv);
    float flare_effect = flare*1./5.;
            
    vec3 col = vec3(0.);
    for( int i = -1; i<=1; i++)
        for( int j = -1; j<=1; j++)
        {
            vec2 offset = vec2( i,j );
            float rand_val = hash21(n+offset);
            float flare_rand = flare * ( fract( rand_val * 789.) *.5 +.5);
            float starcol = star(gv - offset  + vec2(rand_val-.5, fract(rand_val*10.) -.5), flare_rand);
            
            // flickering :
            float freq  = (fract(rand_val * 100.) + 3.) / 2.;
            float phase = fract(rand_val * 1000.)*15.;
            float power = fract(rand_val * 55.) * (1.-flare_effect) + flare_effect;
            float sin_amplitude = ( 1.-power);
            
            starcol *= abs(sin( iTime *freq +phase ))* sin_amplitude + power + sin_amplitude/2.;
            float colRand = fract( rand_val * 123456.);
            col += starcol * mix(color1, color2, colRand) * fract(rand_val*111.);
        }
    
    //if (gv.x > .48 ||gv.y > .48) col.r = 1.;
    return col;
}

vec3 starry_sky( vec2 uv )
{
    vec3 col = vec3(0.,0.,0.1);

    uv *= 12.;
    col += starfield(uv, .1, vec3(.97,.95,.77), vec3(.97,.77,.22));
    uv /= 2.;
    uv = Rot(.5) * uv;
    col += starfield(uv, .1, vec3(.65,.32,.60), vec3(.18,.50,.63));
    uv /= 2.;
    uv = Rot(1.) * uv;
    col += starfield(uv, .4, vec3(.34,.09,.39), vec3(.18,.43,.52));
    uv /= 2.;
    uv += vec2(5.,10.);
    uv = Rot(6.2) * uv;
    col += starfield(uv, .4, vec3(.18,.43,.52), vec3(.08,.40,.51));
    return col;
}

void main( )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = (xlv_TEXCOORD0-.5*iResolution.xy)/iResolution.y;
    
    vec3 col = starry_sky(uv);
    uv *= 2.;
    
    const float planet_factor = .6;
    if ( length(uv) < planet_factor )
    {
        col = add_planet(uv / planet_factor);
    }   

    // Output to screen
    gl_FragColor = vec4(col,1.0);
}
