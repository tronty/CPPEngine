// Original shader from: https://twigl.app/?ol=true&ss=-NQVK1bDj18UhjVT7D2C

#if 0
#ifdef GL_ES
precision highp float;
#endif

//uniform float time;
uniform vec2 resolution;
#endif

const float PI = 3.141592653589793;
const float PI2 = PI * 2.0;

mat2 rotate2D(float r){
    return mat2(cos(r), sin(r), -sin(r), cos(r));
}

void twigl(out vec4 o, vec2 FC, vec2 r, float t) {
    o=vec4(0);
    vec3 P,Q;
    float i=0.,d=1.,a,g=0.;
    for(int ii=0;ii<99;ii++){
        i++;
        if (d<=1e-4) break;
        P=vec3((FC.xy-r*.5)/r.y*g,g);
        P.zy=P.zy*rotate2D(1.)+vec2(t,3)+sin(t*PI2)*.07;
        d=min((P.y-abs(fract(P.z)-.5))*.7,1.5-abs(P.x));
        a=2.;
        for(int j=0;j<9;j++) {
            Q=P*a,Q.xz*=rotate2D(a),d+=abs(dot(sin(Q),Q-Q+1.))/a/7.;
            a+=a;
        }
        g+=d*.5;
    }
    o+=9./i;
}

void main(void)
{
    twigl(gl_FragColor, xlv_TEXCOORD0, resolution, time);
    gl_FragColor.a = 1.;
}

