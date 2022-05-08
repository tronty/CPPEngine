uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
vec3 cameraPos() {
float t = iTime + 14.0;

vec2 rot = vec2(cos(t * 0.2), sin(t * 0.2));
    float rot_speed = 20.0;
    vec3 pos = vec3(13.0 + rot.x * rot_speed,1.5 + min(t, 10.0) * min(exp(t * 0.05) , 5.0),14.0 + rot.y * rot_speed) * 0.5;
    pos.y = clamp(pos.y, 0.0, 5.5);
    
    return pos;
}


float sdBoard(vec3 p) {

    float d = sdBox(p, BOARD_VEC);
    return d;
}

// This is used by other functions for blending between two materials
// Also contains much of our material rendering code in one place.

  const vec3 arr[] = vec3[](
        VEC_ZERO,
        VEC_ZERO,
        vec3(.2,.3,.8),
        vec3(0.0,.8,.4),
        vec3(.8,.3,.2),
        vec3(.3,.8,.8),
        vec3(0.5,0.8,0.8),
        VEC_ZERO,
        VEC_ZERO,
        VEC_ZERO
    );
        
        
vec3 calculateMaterial(vec3 p, int mat) {
    vec3 col = arr[mat];  
    if (mat == MAT_PAWN || mat == MAT_ROOK) {
    // fake AO by making it darker lower it is
    col = (1.0 - smoothstep(1.0, 0.0, p.y) * VEC_ONE) * 0.8; 
    col = (p.z > 0.0) ? col : (col - 0.5);
    col = mix (col, col * texture(iChannel1, p.xy).xyz, 0.25);
    }
        
    if (mat == MAT_OUTERBOARD)
    {
        vec2 samplePoint = p.xz + p.y;
        col = vec3(0.6, 0.2, 0.3) * texture(iChannel0, samplePoint).xyz;
    }
    
    if (mat == MAT_BOARD) {
        col = VEC_ONE * boardParity(p);
        col = mix(col, texture(iChannel0, p.xz).xyz, 0.25);
    }

    return col;
}


// This handles drawing all of the pieces

float sdBoardFill(vec3 p) 
{  
    // Nothing below the board is ever drawn 
    if (p.y < 0.0) return float(MAX_RAY_LENGTH);
    float d; 
    
    p.z = abs(p.z);
    
    // Repeat/mirror pawns
    {
        vec3 p = p;
        p -= vec3(0.5, BOARD_VEC.y, 2.5);
        // Make sure to adjust p so they're given the surface of the board
        vec3 bv = vec3(4.0,0.0,4.0);
        p = opRepLim(p, 1.,vec3(-4.0,0.0,0.0), vec3(3.0,0.0,0.0));
        d = sdPawn(p / 0.5) * 0.5; // sdRook2(p - vec3(0.0, BOARD_VEC.y, 0.0));
    }
    
    // Rooks   
    #ifdef ENABLE_ROOKS
    {
        vec3 p = p;
            p.x = abs(p.x);
            p -= vec3(3.5, BOARD_VEC.y, 3.5);
            float scale = 0.6;
            float d2 = sdRook2(p/scale, d) * scale;
            d = min(d, d2);
    }
    #endif
    
    // Bishops
    {
        vec3 p = p;
        p.x = abs(p.x);
        p -= vec3(1.5, BOARD_VEC.y, 3.5);
        
        float scale = 0.55;
        float d2 = sdBishop(p/scale) * scale;

        d = min(d, d2);
    }
    
    // Knights
    {
        vec3 p = p;
        p.x = abs(p.x);
        p -= vec3(2.5, BOARD_VEC.y, 3.5);
        
        float scale = 0.45;
        float d2 = sdKnight(p/scale) * scale;

        d = min(d, d2);
    }
    
    // King
    {
        vec3 p = p;
        p -= vec3(0.5, BOARD_VEC.y, 3.5);
        
        float scale = 0.65;
        float d2 = sdKing(p/scale) * scale;

        d = min(d, d2);
    }
       
    // Queen
    {
        vec3 p = p;
        p -= vec3(-.5, BOARD_VEC.y, 3.5);
        
        float scale = 0.60;
        float d2 = sdQueen(p/scale) * scale;

        d = min(d, d2);
    }
    
    return d;
}



// This finds the distance to any surface in our scene.
// We also return the material of that surface because it's cheap to calculate here.

float sdCast(vec3 p, out int material) {

    // Checkerboard base
    float d = sdBoard(p);

    if (d > 0.) {
        material = MAT_BOARD;
    }
        
   // Outer edge of chess board
   float cutOut = sdBox(p, BOARD_VEC + vec3(0., +0.5, 0.)); 
   float d3 = sdBox(p, BOARD_VEC + vec3(0.3, +0.2, 0.3)); 
    
   d3 = max(d3, -cutOut);


    if (d3 < d) {
    d = d3;
    material = MAT_OUTERBOARD;
    }
        
    float d2 = sdBoardFill(p);
    if (d2 < d) {
        material = MAT_PAWN;
        d = d2;
        
        // back row uses their own material which is less shiny than pawns
        if (abs(p.z) > 3.0) material = MAT_ROOK;
    }

    return d;
}


int rayMarch(vec3 rp, vec3 rd, out vec3 hit, out vec3 normal) 
{
    int material = MAT_NONE;

    rd = normalize(rd);
    // The total distance traveled from rp in the direction of rd
    float dist = 0.0;
    vec3 pos;
    
    for(int i = 0; i < MAX_ITER; i++) {
        pos = rp + rd * dist;
        float closest_point_dist = sdCast(pos, material);

        if (closest_point_dist >  MIN_DIST)  dist += closest_point_dist;
        // TODO: The normal of a skybox should 
        if (dist >= MAX_RAY_LENGTH) return MAT_SKYBOX;
        
        // If we're making very little progress, just exit the loop.
        // This is also where we have to calculate the normal.
        
        if (closest_point_dist <= MIN_DIST || i == MAX_ITER - 1) { 
        hit = pos;
            vec2 h = vec2(0.0001, 0.0);
            int _ = 0;
            normal.x = sdCast(pos + h.xyy, _) - closest_point_dist; 
            normal.y = sdCast(pos + h.yxy, _) - closest_point_dist; 
            normal.z = sdCast(pos + h.yyx, _) - closest_point_dist;       
            normal = normalize(normal);
            
            return material;
        }
    }

}


// From https://www.shadertoy.com/view/4slSWf
void generateRay( out vec3 resRo, out vec3 resRd, in vec3 po, in vec3 ta, in vec2 pi )
{
	vec2 p = (2.0*pi-iResolution.xy)/iResolution.y;
        
    // camera matrix
    vec3 ww = normalize( ta - po );
    vec3 uu = normalize( cross(ww,vec3(0.0,1.0,0.0) ) );
    vec3 vv = normalize( cross(uu,ww));

	// create view ray
	vec3 rd = normalize( p.x*uu + p.y*vv + 2.2*ww );

    resRo = po;
    resRd = rd;
}

void main( )
{
    vec3 col_total;
    
    for(int i = 0; i < AA; i++) 
    for(int j = 0; j < AA; j++)
    {{
    vec3 col;
    
    vec3 camera_pos = cameraPos();   
    vec3 camera_look_at = VEC_ZERO;
    
    vec2 fragOffset = vec2(float(i),float(j)) / float(AA);
      
    vec3 rp, rd; generateRay(rp, rd, camera_pos, camera_look_at, xlv_TEXCOORD0.xy + fragOffset);
    
    // The sun faces down at an angle
    vec3 sun = normalize(vec3(0, -0.8, -0.5));

    vec3 hit;
    
    // By default the sky should point towards our light
    vec3 normal = -sun;
    
    int mat = rayMarch(rp, rd, hit, normal);

    
    float diffuse = dot(normal,-sun) + 0.5;
    diffuse = clamp(diffuse, 0.0, 0.8) + 0.2;
    
    // Now see if we a ray in the direction of our light is blocked by anything
    
    float shadow_dist; 
    // vec3 sp, sd;  generateRay(sp, sd, hit, -sun, xlv_TEXCOORD0.xy);
    
    vec3 shadow_hit, shadow_normal;
    
    // Also offset slightly from the surface to avoid issues with numerical accuracy
    
    int shadow_material = rayMarch(hit + normal * 0.001, -sun, shadow_hit, shadow_normal);
    
    bool is_shadow = (shadow_material != MAT_SKYBOX);

    col.xyz =  calculateMaterial(hit, mat) * diffuse; // smoothstep(diffuse, 0.2,1.0);  

    vec3 r_rd = rd - 2.0 * normal * dot(normal, rd);

    vec3 r_hit; vec3 r_normal; int reflect_mat = rayMarch(hit + r_rd * VEC_ONE * 0.01, r_rd, r_hit, r_normal);
    
    vec3 reflect_col = calculateMaterial(r_hit, reflect_mat) * 0.9;
    
    
    // Note that the actual skybox material doesn't get lighting.
     col =  mix(col, reflect_col, CalculateMaterialReflect(mat));
     if (is_shadow && mat != MAT_SKYBOX) col.xyz *= 0.5;
     col_total += col;
     }}
          
     gl_FragColor.xyz = col_total.xyz / (float(AA*AA));
}
