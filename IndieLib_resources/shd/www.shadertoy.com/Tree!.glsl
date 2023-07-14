#define S(a, b, t) smoothstep(a, b, t)
float random(float seed) {
    return sin(sin(seed*389.1238)*3.590*sin(seed*283.143))*.5+.5;
}

float Rectangle(vec2 uv, float width, float height, float blur) {
    float bottomLine = S(-blur, blur, uv.y+height/2.);
    float topLine = S(blur, -blur, uv.y-height/2.);
    float leftLine = S(blur, -blur, uv.x-width/2.);
    float rightLine = S(-blur, blur, uv.x+width/2.);
    return topLine*bottomLine*leftLine*rightLine;
}

float Circle(vec2 uv, float radius, float blur) {
    return S(blur, -blur, length(uv)-radius);
}

float TaperBox(vec2 uv, float wbase, float wtop, float height, float offset, float blur) {
    float bottomLine = S(-blur, blur, uv.y+height/2.);
    float topLine = S(blur, -blur, uv.y-height/2.);
    
    float width = (uv.y+height/2.)/height * (wtop-wbase) + wbase;
    float current_offset = (uv.y+height/2.)/height * offset;
    // current_offset = 0.0;
    float leftLine = S(blur, -blur, uv.x-width/2. - current_offset);
    float rightLine = S(-blur, blur, uv.x+width/2. - current_offset);
    
    return topLine*bottomLine*leftLine*rightLine;
}

float GetHeight(float x) {
    return sin(x*0.875)*0.2 + sin(x*1.54839)*0.1;
}

float Ground(vec2 uv, float blur) {
    return S(blur, -blur, uv.y-GetHeight(uv.x));
}

// idk what `speed` does but it doesn't do what I wanted it to do
vec4 leavesColour(float id, float speed) {
    const int ColourCount = 4;
    vec4[ColourCount+1] colours;
    colours[0] = vec4(65, 201, 20, 255)/255.; // green
    colours[1] = vec4(201, 196, 42, 255)/255.; // yellow
    colours[2] = vec4(214, 153, 21, 255)/255.; // orange
    colours[3] = vec4(209, 55, 17, 255)/255.; // red
    colours[ColourCount] = colours[0];
    
    float time = mod(id/speed*0.1, 1.)*float(ColourCount);
    int colourIndex = int(time);
    vec4 colour = mix(colours[colourIndex], colours[colourIndex+1], mod(time, 1.));
    
    
    return colour;
}

vec4 Layer(vec2 uv, float speed, vec4 col, float brightness, float blur) {
    float ground = Ground(uv, blur);
    
    float id = floor((uv.x+0.3)/0.6);
    uv.x = mod(uv.x+0.3, 0.6)-0.3;
    
    vec4 trunkColour = vec4(189, 111, 23, 255)/255.;
    vec4 leavesColour = leavesColour(id, speed);
    vec4 groundColour = vec4(57, 189, 120, 255)/255.;
    
    float height = GetHeight(id*0.6 + 0.3);
    uv.y -= height;
    
    uv.y -= 0.2;
    uv.x += random(id)*0.1;
    
    float trunk = TaperBox(uv-vec2(0., -0.2), 0.05, 0.05, 0.4, 0.0, blur);
    col = mix(col, vec4(trunkColour.xyz*brightness, 1.), trunk);
    
    float leaves = TaperBox(uv, 0.2, 0.1, 0.1, 0.0, blur);
    float shadowOnLeaves = 1. - TaperBox(uv-vec2(0.075, 0.025), 0.0, 0.1, 0.05, -0.075, blur)*0.3;
    col = mix(col, vec4(vec3(leavesColour*0.8)*shadowOnLeaves*brightness, 1.), leaves);
    shadowOnLeaves = 1. - TaperBox(uv-vec2(-0.055, 0.105), 0.0, 0.05, 0.05, 0.05, blur)*0.3; // TODO: this
    leaves = TaperBox(uv-vec2(0., 0.09), 0.15, 0.05, 0.1, 0.0, blur);
    col = mix(col, vec4(vec3(leavesColour*0.9)*shadowOnLeaves*brightness, 1.0), leaves);
    leaves = TaperBox(uv-vec2(0., 0.18), 0.1, 0.0, 0.1, 0.0, blur);
    col = mix(col, vec4(leavesColour.xyz*brightness, 1.), leaves);
    
    float shadowOnTrunk = TaperBox(uv-vec2(-0.025, -0.0625), 0.0, 0.05, 0.025, 0.025, blur);
    col = mix(col, vec4(vec3(0.), 1.0), shadowOnTrunk*0.3);
    
    col = mix(col, vec4(groundColour.xyz*brightness, 1.), ground);
    return col;
}

void main( )
{
    float t = iTime*0.3;
    float blur = 0.002;
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = (xlv_TEXCOORD0-iResolution.xy/2.)/iResolution.y;

    vec3 col = mix(vec3(222, 150, 35)/255., vec3(121, 207, 224)/255., (uv.y+.5));
    
    col = mix(col, vec3(252, 251, 151)/255., Circle(uv, 0.2, blur*14.));
    
    vec4 layer = vec4(1.);
    for (float i=0.; i<1.; i+=1./5.) {
        float scale = 3. - 2.*i;
        layer = Layer(uv * scale - vec2(random(i)*3.192384, -i*1.2+0.6) + vec2(t*(i+0.1), 0.), i+0.1, vec4(col, 1.), i/2. + .5, (1.-sin(i*3.1415926))*blur*14.);
        col = mix(col, layer.xyz, layer.a);
    }
    

    // Output to screen
    gl_FragColor = vec4(col,1.0);
}

