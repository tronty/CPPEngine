#define TILES 10.0

//2D random from https://www.shadertoy.com/view/WstGDj
float random (vec2 uv) {
    return fract(sin(dot(uv, vec2(135., 263.))) * 103.214532);
}

vec4 drawSnow(vec2 curid, vec2 uv, vec4 gl_FragColor, float r, float c)
{
    float maxoff = 2.0 / TILES; //calculate the max offset a particle can have (two tiles)

    //loop through neighboring tiles
    for(int x=-2; x<=1; x++)
    {
        for(int y=-2; y<=0; y++)
        {
            float rad = (1.0 / (TILES * 5.0)) * r; //set default radius
            vec2 id = curid + vec2(x, y); //get the id of the tile we're visiting
            vec2 pos = id / TILES; //calculate position
            float xmod = mod(random(pos), maxoff);
            pos.x += xmod; //add a random x-offset
            pos.y += mod(random(pos+vec2(4,3)), maxoff); //add a random y-offset
            rad *= mod(random(pos), 1.0); //vary the radius by multiplying by a random val
            pos.x += 0.5*(maxoff-xmod)*sin(iTime*r + random(pos)*100.0); //dynamic sin wave x-offset
            
            float len = length(uv - pos); //calculate distance from tile's particle

            //if we're inside the particle, draw it
            float v = smoothstep(0.0, 1.0, (rad - len) / rad*0.75);
            gl_FragColor = mix(gl_FragColor, vec4(c), v);      
        }
    }
    
    return gl_FragColor;
}


void main( )
{
    vec2 uv = (2.0*xlv_TEXCOORD0 - iResolution.xy)/iResolution.x;
    uv.y -= 0.3;
    
    //uv.x -= 0.6;

    
    vec3 col = mix(vec3(0.0, 0.45, 0.85), vec3(1), -0.3-uv.y);

    // Output to screen
    gl_FragColor = vec4(col,1.0);
    
    vec4 bg = vec4(.529, .808, .922, 1) * 0.25;
    vec2 uvNorm = xlv_TEXCOORD0.xy / iResolution.xy; //normalized UV coordinate [0, 1]
    vec2 uvog = xlv_TEXCOORD0.xy / iResolution.y; //UV coordinate (will remain static)
    uv = xlv_TEXCOORD0.xy / iResolution.y; //UV coordinate (we'll modify this one)
    
    //draw the closest snow layer
    uv += 0.2*vec2(-iTime, iTime); //move the UV coords based on time
    vec2 curid = floor(uv * TILES); //calculate the ID associated with the current UV
    curid += vec2(0.5); //center the ID
    
    //if(curid.y > 10.0)
    {
    gl_FragColor = drawSnow(curid, uv, gl_FragColor, 1.0, 0.9); //draw closest snow layer
    
    //draw the middle snow layer, calculate new UV and ID
    uv = uvog + 0.1*vec2(-iTime - 100.0, iTime + 100.0);
    curid = floor(uv * TILES);
    curid += vec2(0.5);
    gl_FragColor += drawSnow(curid, uv, vec4(0), 0.75, 0.45); 
    
    //draw the far snow layer, calculate new UV and ID
    uv = uvog + 0.05*vec2(-iTime - 150.0, iTime + 150.0);
    curid = floor(uv * TILES);
    curid += vec2(0.5);
    gl_FragColor += drawSnow(curid, uv, vec4(0), 0.5, 0.225);
    
    gl_FragColor = smoothstep(0.0, 3.0, iTime)*gl_FragColor;
    }
}

