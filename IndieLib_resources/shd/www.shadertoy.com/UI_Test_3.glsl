#define Rot(a) mat2(cos(a),-sin(a),sin(a),cos(a))
#define S(p,d,b) smoothstep(fwidth(p.y),0.0,d)
#define B(p,s) max(abs(p).x-s.x,abs(p).y-s.y)
#define DF(a,b) length(a) * cos( mod( atan(a.y,a.x)+6.28/(b*8.0), 6.28/((b*8.0)*0.5))+(b-1.)*6.28/(b*8.0) + vec2(0,11) )
#define Slice(p,a) dot(p,vec2(cos(a),sin(a)))

#define antialiasing(n) (1.0*n)/min(iResolution.y,iResolution.x)
//define S(p,d,b) smoothstep(antialiasing(1.0),b,d)
//define Tri(p,s,a) max(-dot(p,vec2(cos(-a),sin(-a))),max(dot(p,vec2(cos(a),sin(a))),max(abs(p).x-s.x,abs(p).y-s.y)))

#define PURPLE vec3(0.8,0.6,0.9)
#define GREEN vec3(0.1,0.8,0.6)

float dLine(vec2 p, vec2 a, vec2 b, float w){
    vec2 v = normalize(b-a);
    vec2 right = normalize(cross(vec3(v,0.0),vec3(0.0,0.0,1.0)).xy);
    vec2 newRightVec = right*(w*0.5);
    vec2 a1 = a+newRightVec, a2 = a-newRightVec, b1 = b+newRightVec, b2 = b-newRightVec;
    vec2 mida = vec2((a1.x + b1.x) * 0.5, (a1.y + b1.y) * 0.5);
    vec2 midb = vec2((a2.x + b2.x) * 0.5, (a2.y + b2.y) * 0.5);
    
    float rad1 = -atan(b1.x-a1.x,b1.y-a1.y);
    float rad2 = -atan(a2.x-a1.x,a2.y-a1.y);
    
    float mad = Slice(p-mida,rad1);
    float mbd = Slice(p-midb,rad1);
    
    float ad = Slice(p-a,rad2);
    float bd = Slice(p-b,rad2);
    float d = max(max(mad,-mbd),max(-ad,bd));
    return d;
}

#if 0
// distance 2d digital alarm font
/* const */ float cDefault[7] = float[](0.,0.,0.,0.,0.,0.,0.);
/* const */ float c1[7] = float[](10.,0.,0.,10.,10.,10.,10.);
/* const */ float c2[7] = float[](0.,0.,10.,0.,0.,10.,0.);
/* const */ float c3[7] = float[](0.,0.,0.,0.,10.,10.,0.);
/* const */ float c4[7] = float[](10.,0.,0.,10.,10.,0.,0.);
/* const */ float c5[7] = float[](0.,10.,0.,0.,10.,0.,0.);
/* const */ float c6[7] = float[](0.,10.,0.,0.,0.,0.,0.);
/* const */ float c7[7] = float[](0.,0.,0.,10.,10.,10.,10.);
/* const */ float c8[7] = float[](0.,0.,0.,0.,0.,0.,0.);
/* const */ float c9[7] = float[](0.,0.,0.,0.,10.,0.,0.);
/* const */ float c0[7] = float[](0.,0.,0.,0.,0.,0.,10.);
/* const */ float cTBase[7] = float[](0.,10.,10.,10.,10.,10.,10.);
/* const */ float cKBase[7] = float[](10.,10.,0.,10.,0.,0.,0.);
/* const */ float cYBase[7] = float[](10.,0.,0.,0.,10.,0.,0.);
#else
float c_0 = 31599.0;
float c_1 = 9362.0;
float c_2 = 29671.0;
float c_3 = 29391.0;
float c_4 = 23497.0;
float c_5 = 31183.0;
float c_6 = 31215.0;
float c_7 = 29257.0;
float c_8 = 31727.0;
float c_9 = 31695.0;
float c_colon = 1040.0;
#endif

const vec2 digitVPos = vec2(0.08,0.073);
const vec2 digitHPos = vec2(0.0,0.15);

float dDigitV(vec2 p){
    float d = B(p,vec2(0.02,0.1));
    p = abs(p);
    p-=vec2(0.02,0.05);
    d = max(Slice(p,44.775),d);
    return d;
}

float dDigitH(vec2 p){
    p*=Rot(radians(90.0));
    float d = dDigitV(p);
    return d;
}

float dDigitBase(vec2 p, float[7] char){
    vec2 prevP = p;
    float d = 10.0;
    float d2 = 10.0;
    // 1
    d = (char[0])+dDigitH(p-digitHPos);
    // 2
    d2 = (char[1])+dDigitV(p-digitVPos);
    d = min(d,d2);
    // 3
    d2 = (char[2])+dDigitV(p+vec2(-digitVPos.x,digitVPos.y));
    d = min(d,d2);
    // 4
    d2 = (char[3])+dDigitH(p+digitHPos);
    d = min(d,d2);
    // 5
    d2 = (char[4])+dDigitV(p+digitVPos);
    d = min(d,d2);
    // 6
    d2 = (char[5])+dDigitV(p+vec2(digitVPos.x,-digitVPos.y));
    d = min(d,d2);
    // 7
    d2 = (char[6])+dDigitH(p);
    d = min(d,d2);
    
    return d;
}

#if 0
float dDigitNumber(vec2 p, int num){
    float d = 100.0;
    if(num == 0){
        d = dDigitBase(p,c0);
    } else if(num == 1){
        d = dDigitBase(p,c1);
    } else if(num == 2){
        d = dDigitBase(p,c2);
    } else if(num == 3){
        d = dDigitBase(p,c3);
    } else if(num == 4){
        d = dDigitBase(p,c4);
    } else if(num == 5){
        d = dDigitBase(p,c5);
    } else if(num == 6){
        d = dDigitBase(p,c6);
    } else if(num == 7){
        d = dDigitBase(p,c7);
    } else if(num == 8){
        d = dDigitBase(p,c8);
    } else if(num == 9){
        d = dDigitBase(p,c9);
    }
    
    return d;
}
#else
float getBit(float num,float bit)
{
	num = floor(num);
	bit = floor(bit);
	return float(mod(floor(num/pow(2.,bit)),2.) == 1.0);
}

float Sprite3x5(float sprite,vec2 p)
{
	float bounds = float(all(lessThan(p,vec2(3,5))) && all(greaterThanEqual(p,vec2(0,0))));
	return getBit(sprite,(2.0 - p.x) + 3.0 * p.y) * bounds;
}

float Digit(vec2 p, float num)
//float Digit(float num,vec2 p)
{
	num = mod(floor(num),11.0);
	if(num == 0.0) return Sprite3x5(c_0,p);
	if(num == 1.0) return Sprite3x5(c_1,p);
	if(num == 2.0) return Sprite3x5(c_2,p);
	if(num == 3.0) return Sprite3x5(c_3,p);
	if(num == 4.0) return Sprite3x5(c_4,p);
	if(num == 5.0) return Sprite3x5(c_5,p);
	if(num == 6.0) return Sprite3x5(c_6,p);
	if(num == 7.0) return Sprite3x5(c_7,p);
	if(num == 8.0) return Sprite3x5(c_8,p);
	if(num == 9.0) return Sprite3x5(c_9,p);
	if(num == 10.0) return Sprite3x5(c_colon,p);	
	return 0.0;
}

float dDigitNumber(vec2 p, float num)
{
	return Digit(p, num);
}
#endif

float dDrawNumbers(vec2 p){
    float t = iTime*7.0;

    float d = dDigitNumber(p,float(mod(t*2.0,10.0)));
    float d2 = dDigitNumber(p+vec2(0.22,0.0),float(mod(t*0.5,10.0)));
    d = min(d,d2);
    d2 = dDigitNumber(p+vec2(-0.22,0.0),float(mod(t,10.0)));
    d = min(d,d2);
    return d;
}

// noise function from https://www.shadertoy.com/view/Xd3GD4
//-----------------------------------------------------------------------------
vec2 hash( vec2 p ){
    p = vec2( dot(p,vec2(127.1,311.7)),
             dot(p,vec2(269.5,183.3)) );
    return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}
float noise2d( in vec2 p ){
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;
    
    vec2 i = floor( p + (p.x+p.y)*K1 );
    
    vec2 a = p - i + (i.x+i.y)*K2;
    vec2 o = (a.x>a.y) ? vec2(1.0,0.0) : vec2(0.0,1.0);
    vec2 b = a - o + K2;
    vec2 c = a - 1.0 + 2.0*K2;
    
    vec3 h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
    
    vec3 n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));
    
    return dot( n, vec3(70.0) );
}

vec3 UIItem1(vec2 p, vec3 col){
    vec2 prevP = p;
    
    p.x+=sin(3.0*p.y+iTime*0.5)*1.2*cos(-2.0*p.x+iTime*0.7)*0.3;
    vec2 gv = fract(p*25.0);
    
    float thick = 0.1;
    float d = B(gv,vec2(2.0,thick));
    d = min(d,B(gv,vec2(thick,2.0)));
    
    p = prevP;
    d = max((length(p)-0.38),d);
    
    col = mix(col,vec3(0.0),S(p,d,0.0));
    
    float r = 5.0;
    float index = 0.0;
    for(float i = 0.02; i<=0.08; i+= 0.01){
        vec2 pos = p-vec2(-0.35,-0.35);
        pos *= Rot(radians(-iTime*r*10.0));
        d = abs(length(pos)-(0.05+i))-0.001;
        float a = radians(-15.*(i*100.0));
        d = max(min(dot(pos,vec2(cos(a),sin(a))),dot(pos,vec2(cos(-a),sin(-a)))),d);
        
        vec3 lcol = (mod(floor(index),2.0) == 0.0)?PURPLE:GREEN;
        col = mix(col,lcol,S(p,d,0.0));
        r+=2.0;
        index+=1.0;
    }
    
    p = prevP;
    
    vec2 pos = p-vec2(-0.35,-0.35);
    pos.y = abs(pos.y);
    d = dLine(pos,vec2(-0.04,0.04),vec2(0.05,0.0),0.001);
    float d2 = dLine(pos,vec2(-0.01,0.00),vec2(0.05,0.0),0.001);
    d = min(d,d2);
    d2 = dLine(pos,vec2(-0.04,0.04),vec2(-0.03,0.0),0.001);
    d = min(d,d2);
    d2 = dLine(pos,vec2(-0.04,0.04),vec2(-0.03,0.0),0.001);
    d = min(d,d2);
    d2 = dLine(pos,vec2(-0.04,0.04),vec2(-0.01,0.0),0.001);
    d = min(d,d2);
    
    p = prevP;
    col = mix(col,vec3(0.0),S(p,d,0.0));
    
    d = abs(length(p-vec2(-0.42,0.3))-0.07)-0.01;
    p = prevP;
    col = mix(col,vec3(0.0),S(p,d,0.0));
    d = abs(length(p-vec2(-0.42,0.3))-0.07)-0.01;
    
    p-=vec2(-0.42,0.3);
    p*=Rot(radians(iTime*30.0));
    d = max(p.y,d);    
    
    p = prevP;
    col = mix(col,GREEN,S(p,d,0.0)); 
    
    d = abs(length(p-vec2(0.35,-0.4))-0.05)-0.01;
    p = prevP;
    col = mix(col,vec3(0.0),S(p,d,0.0));
    d = abs(length(p-vec2(0.35,-0.4))-0.05)-0.01;
    
    p-=vec2(0.35,-0.4);
    p*=Rot(radians(-iTime*50.0));
    d = max(p.y,d);    
    
    p = prevP;
    col = mix(col,PURPLE,S(p,d,0.0));     
    
    return col;
}

vec3 UIItem2(vec2 p, vec3 col){
    vec2 prevP = p;
    
    p*=Rot(radians(-60.0));
    float d = abs(length(p)-0.283)-0.02;
    
    float a = radians(30.0);
    float a2 = radians(abs(sin(iTime*0.5)*150.0));
    d = max(-min(dot(p,vec2(cos(a),sin(a))),-dot(p,vec2(cos(-a2),sin(-a2)))),d);    
    
    p = prevP;
    col = mix(col,PURPLE,S(p,d,0.0));
    
    p = DF(p,4.0);
    p = abs(p);
    p -= vec2(0.2);
    d = B(p*Rot(radians(45.0)),vec2(0.002,0.025));
    
    p = prevP;
    p = DF(p,8.0);
    p = abs(p);
    p -= vec2(0.2);
    float d2 = B(p*Rot(radians(45.0)),vec2(0.001,0.017));
    d = min(d,d2);
    
    p = prevP;
    
    a = radians(-50.0);
    d = max(min(dot(p,vec2(cos(a),sin(a))),-dot(p,vec2(cos(-a),sin(-a)))),d);
    d2 = abs(length(p)-0.283)-0.001;
    d = min(d,d2);
    
    col = mix(col,vec3(0.0),S(p,d,0.0));
    
    
    d = abs(length(p)-0.169)-0.001;
    d = max(-p.y-0.002,d);
    col = mix(col,GREEN,S(p,d,0.0));
    
    
    p = DF(p,4.0);
    p = abs(p);
    p -= vec2(0.13);
    d = B(p*Rot(radians(45.0)),vec2(0.002,0.015));
    
    p = prevP;
    p = DF(p,8.0);
    p = abs(p);
    p -= vec2(0.128);
    d2 = B(p*Rot(radians(45.0)),vec2(0.001,0.012));
    d = min(d,d2);
    
    p = prevP;
    d = max(-p.y-0.002,d);
    
    col = mix(col,vec3(0.0),S(p,d,0.0));
    
    p = prevP;
    p = DF(p,16.0);
    p = abs(p);
    p -= vec2(0.14);
    d = B(p*Rot(radians(45.0)),vec2(0.005,0.002));
    
    p = prevP;
    a = radians(-48.0);
    d = max(-min(dot(p,vec2(cos(a),sin(a))),-dot(p,vec2(cos(-a),sin(-a)))),d);
    
    p = prevP;
    col = mix(col,PURPLE,S(p,d,0.0));    
    
    p = prevP;
    p = DF(p,16.0);
    p = abs(p);
    p -= vec2(0.14);
    d = B(p*Rot(radians(45.0)),vec2(0.005,0.005));
    
    p = prevP;
    a = radians(-48.0);
    a2 = radians(2.0);
    d = max(-min(dot(p,vec2(cos(a),sin(a))),-dot(p,vec2(cos(a2),sin(a2)))),d);
    
    p = prevP;
    col = mix(col,vec3(0.0),S(p,d,0.0));
    
    d = abs(length(p)-0.17)-0.002;
    d = max(p.y+0.13,d);
    col = mix(col,PURPLE,S(p,d,0.0));
    
    d = dDrawNumbers(p*3.5);
    col = mix(col,vec3(0.0),S(p,d,0.0));
    
    d = dDigitNumber((p-vec2(0.0,-0.13))*8.0,9.0);
    col = mix(col,vec3(0.0),S(p,d,0.0));
    
    return col;
}

vec3 UIItem3(vec2 p, vec3 col){
    vec2 prevP = p;
    float d = length(p-vec2(0.15,0.0))-0.2;
    p.x += iTime*0.1;
    
    float n = noise2d(p*73.0)*3.0;
    
    p = prevP;
    col = mix(col,PURPLE*0.9,S(p,d,0.0));
    col = mix(col,GREEN,S(p,max(d,d*n),0.0));
    
    d = length(p-vec2(-0.15,0.0))-0.2;
    
    p.x += iTime*0.12;
    n = noise2d(p*75.0)*2.0;
    p = prevP;
    col = mix(col,GREEN*0.9,S(p,d,0.0));
    col = mix(col,PURPLE,S(p,max(d,d*n),0.0));
    
    return col;
}

void main( )
{
    vec2 uv = (2.0*xlv_TEXCOORD0.xy-iResolution.xy)/min(iResolution.y,iResolution.x);
    uv*=1.6;
    vec3 col = vec3(1.0);
    col = UIItem1(uv-vec2(0.4,0.05),col);    
    col = UIItem2((uv-vec2(-0.52,-0.2))*1.1,col); 
    col = UIItem3((uv-vec2(-0.52,0.3))*1.3,col); 
    
    // frame
    float d = abs(B(uv,vec2(0.88,0.49)))-0.01;
    float d2 = B(uv-vec2(-0.16,0.),vec2(0.01,1.0));
    d = min(d,d2);
    d2 = B(uv-vec2(-0.6,0.11),vec2(0.45,0.01));
    d = min(d,d2);
    col = mix(col,vec3(0.0),S(uv,d,0.0));
    
    gl_FragColor = vec4(col,1.0);
}

