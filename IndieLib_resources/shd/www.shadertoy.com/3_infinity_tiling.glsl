const float RT3 = 1.73205080757;

bool invert(inout vec2 pt, vec2 cen, float radSq) {
    // Inversion in a circle defined by center and square radius
    vec2 diff = pt - cen;
    float dis = dot(diff, diff);
   
    if (dis < radSq) {
        pt = cen + diff * (radSq / dis);
        return false;
    }
    return true;
}

vec3 getCol(vec2 pt) {
    pt = (2. * pt - iResolution.xy) / iResolution.y;
    if (dot(pt, pt) > 1.) return vec3(0.);
    
    bool fund;
    float n=0.0;
    for (int i = 0; i < 50; i ++) {
        // Invert about edges of central ideal triangle
        fund = invert(pt, vec2(2., 0.), 3.);
        fund = fund && invert(pt, vec2(-1., RT3), 3.);
        fund = fund && invert(pt, vec2(-1., -RT3), 3.);
        
        // If we are already in the fundamental domain break
        if (fund) break;
        n ++;
    }
    return vec3(mod(n, 2.));
}

void main() {
    vec3 col = getCol(xlv_TEXCOORD0);
    
    // Antialiasing
    col += getCol(xlv_TEXCOORD0 + vec2(.5));
    col += getCol(xlv_TEXCOORD0 + vec2(-.5));
    col += getCol(xlv_TEXCOORD0 + vec2(-.5, .5));
    col += getCol(xlv_TEXCOORD0 + vec2(.5, -.5));
    col += getCol(xlv_TEXCOORD0 + vec2(.5, 0.));
    col += getCol(xlv_TEXCOORD0 + vec2(-.5, 0.));
    col += getCol(xlv_TEXCOORD0 + vec2(0., .5));
    col += getCol(xlv_TEXCOORD0 + vec2(0., -.5));
    col *= .11111;
    
    gl_FragColor = vec4(col,1.0);
}

