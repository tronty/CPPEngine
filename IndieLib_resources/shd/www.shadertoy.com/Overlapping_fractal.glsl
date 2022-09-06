const float PI = 3.14159;
const float SQ2 = 1.4142;

mat2 rotate2D(float r){
    return mat2(cos(r), sin(r), -sin(r), cos(r));
}

vec3 square(vec2 p, float r, float s) {
    vec3 col = vec3(0);
    vec2 q = abs(p) - r;
    col += smoothstep(.01/s, .0, length(max(q,.0)+min(max(q.x,q.y),.0)));
    q = step(abs(p), vec2(r,r));
    col += q.x * q.y * .3;
    return col;
}

void main( ){
    
    float t = iTime * .5;
    vec2 mouse = iMouse.xy/iResolution.xy;
    
    vec2 uv = 2.0 * (xlv_TEXCOORD0.xy - 0.5 * iResolution.xy) / min(iResolution.y, iResolution.x);
    vec3 col = vec3(0);
    
        vec3 red = vec3(.7,.2,0), green = vec3(0,.6,0);
    for(int j=0;j<1<<4;++j){
        vec2 p = uv, q;
        p.y+=.7;
        float r = .2, scale = 1.;
        for(int i=0;i<9;++i){
            if(i>=4||(j>>i)==0){
                col += square(p, r, scale) * mix(red, green, float(i)/8.);
            }
            
            float angle = fract(t*.3)*PI, a2 = PI*.25;
            q=p;


            if(i<4&&(j>>i&1)==1||i>=4&&q.x/r<cos(angle)){
                float rot = -angle*.5, s = cos(rot);
                q.y -= r + r*s*sin(a2-rot)*SQ2;
                q.x += r - r*s*cos(a2-rot)*SQ2;
                q *= rotate2D(-rot);
                scale *= s;
                q/= s;
            }
            else{
                float rot = -PI*.5 + angle*.5, s = cos(rot);
                q.y -= r + r*s*sin(a2-rot)*SQ2;
                q.x -= r - r*s*cos(a2-rot)*SQ2;
                q *= rotate2D(rot);
                scale *= s;
                q/= s;
            }
            p=q;
        }
    }
   
    gl_FragColor = vec4(col,1.0);
}
