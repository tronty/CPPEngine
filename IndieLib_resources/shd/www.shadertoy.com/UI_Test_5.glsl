#define MAX_STEPS 64
#define Rot(a) mat2(cos(a),-sin(a),sin(a),cos(a))
#define antialiasing(n) n/min(iResolution.y,iResolution.x)
#define S(d,b) smoothstep(antialiasing(1.0),b,d)
#define B(p,s) max(abs(p).x-s.x,abs(p).y-s.y)
#define Tri(p,s,a) max(-dot(p,vec2(cos(-a),sin(-a))),max(dot(p,vec2(cos(a),sin(a))),max(abs(p).x-s.x,abs(p).y-s.y)))
#define DF(a,b) length(a) * cos( mod( atan(a.y,a.x)+6.28/(b*8.0), 6.28/((b*8.0)*0.5))+(b-1.)*6.28/(b*8.0) + vec2(0,11) )
#define Skew(a,b) mat2(1.0,tan(a),tan(b),1.0)
#define SkewX(a) mat2(1.0,tan(a),0.0,1.0)
#define SkewY(a) mat2(1.0,0.0,tan(a),1.0)
#define seg_0 0
#define seg_1 1
#define seg_2 2
#define seg_3 3
#define seg_4 4
#define seg_5 5
#define seg_6 6
#define seg_7 7
#define seg_8 8
#define seg_9 9
#define seg_DP 39

float Hash21(vec2 p) {
    p = fract(p*vec2(234.56,789.34));
    p+=dot(p,p+34.56);
    return fract(p.x+p.y);
}

float cubicInOut(float t) {
  return t < 0.5
    ? 4.0 * t * t * t
    : 0.5 * pow(2.0 * t - 2.0, 3.0) + 1.0;
}

float getTime(float t, float duration){
    return clamp(t,0.0,duration)/duration;
}


float segBase(vec2 p){
    vec2 prevP = p;
    
    float size = 0.02;
    float padding = 0.05;

    float w = padding*3.0;
    float h = padding*5.0;

    p = mod(p,0.05)-0.025;
    float thickness = 0.005;
    float gridMask = min(abs(p.x)-thickness,abs(p.y)-thickness);
    
    p = prevP;
    float d = B(p,vec2(w*0.5,h*0.5));
    float a = radians(45.0);
    p.x = abs(p.x)-0.1;
    p.y = abs(p.y)-0.05;
    float d2 = dot(p,vec2(cos(a),sin(a)));
    d = max(d2,d);
    d = max(-gridMask,d);
    return d;
}

float seg0(vec2 p){
    vec2 prevP = p;
    float d = segBase(p);
    float size = 0.03;
    float mask = B(p,vec2(size,size*2.7));
    d = max(-mask,d);
    return d;
}

float seg1(vec2 p){
    vec2 prevP = p;
    float d = segBase(p);
    float size = 0.03;
    p.x+=size;
    p.y+=size;
    float mask = B(p,vec2(size*2.,size*3.7));
    d = max(-mask,d);
    
    p = prevP;
    
    p.x+=size*1.8;
    p.y-=size*3.5;
    mask = B(p,vec2(size));
    d = max(-mask,d);
    
    return d;
}

float seg2(vec2 p){
    vec2 prevP = p;
    float d = segBase(p);
    float size = 0.03;
    p.x+=size;
    p.y-=0.05;
    float mask = B(p,vec2(size*2.,size));
    d = max(-mask,d);

    p = prevP;
    p.x-=size;
    p.y+=0.05;
    mask = B(p,vec2(size*2.,size));
    d = max(-mask,d);
    
    return d;
}

float seg3(vec2 p){
    vec2 prevP = p;
    float d = segBase(p);
    float size = 0.03;
    p.y = abs(p.y);
    p.x+=size;
    p.y-=0.05;
    float mask = B(p,vec2(size*2.,size));
    d = max(-mask,d);

    p = prevP;
    p.x+=0.05;
    mask = B(p,vec2(size,size));
    d = max(-mask,d);
    
    return d;
}

float seg4(vec2 p){
    vec2 prevP = p;
    float d = segBase(p);
    float size = 0.03;
    
    p.x+=size;
    p.y+=0.08;
    float mask = B(p,vec2(size*2.,size*2.0));
    d = max(-mask,d);

    p = prevP;
    
    p.y-=0.08;
    mask = B(p,vec2(size,size*2.0));
    d = max(-mask,d);
    
    return d;
}

float seg5(vec2 p){
    vec2 prevP = p;
    float d = segBase(p);
    float size = 0.03;
    p.x-=size;
    p.y-=0.05;
    float mask = B(p,vec2(size*2.,size));
    d = max(-mask,d);

    p = prevP;
    p.x+=size;
    p.y+=0.05;
    mask = B(p,vec2(size*2.,size));
    d = max(-mask,d);
    
    return d;
}

float seg6(vec2 p){
    vec2 prevP = p;
    float d = segBase(p);
    float size = 0.03;
    p.x-=size;
    p.y-=0.05;
    float mask = B(p,vec2(size*2.,size));
    d = max(-mask,d);

    p = prevP;
    p.y+=0.05;
    mask = B(p,vec2(size,size));
    d = max(-mask,d);
    
    return d;
}

float seg7(vec2 p){
    vec2 prevP = p;
    float d = segBase(p);
    float size = 0.03;
    p.x+=size;
    p.y+=size;
    float mask = B(p,vec2(size*2.,size*3.7));
    d = max(-mask,d);
    return d;
}


float seg8(vec2 p){
    vec2 prevP = p;
    float d = segBase(p);
    float size = 0.03;
    p.y = abs(p.y);
    p.y-=0.05;
    float mask = B(p,vec2(size,size));
    d = max(-mask,d);
    
    return d;
}

float seg9(vec2 p){
    vec2 prevP = p;
    float d = segBase(p);
    float size = 0.03;
    p.y-=0.05;
    float mask = B(p,vec2(size,size));
    d = max(-mask,d);

    p = prevP;
    p.x+=size;
    p.y+=0.05;
    mask = B(p,vec2(size*2.,size));
    d = max(-mask,d);
    
    return d;
}

float segDecimalPoint(vec2 p){
    vec2 prevP = p;
    float d = segBase(p);
    float size = 0.028;
    p.y+=0.1;
    float mask = B(p,vec2(size,size));
    d = max(mask,d);
    return d;
}

float drawFont(vec2 p, int char){
    p*=2.0;
    float d = 10.;
    if(char == seg_0) {
        d = seg0(p);
    } else if(char == seg_1) {
        d = seg1(p);
    } else if(char == seg_2) {
        d = seg2(p);
    } else if(char == seg_3) {
        d = seg3(p);
    } else if(char == seg_4) {
        d = seg4(p);
    } else if(char == seg_5) {
        d = seg5(p);
    } else if(char == seg_6) {
        d = seg6(p);
    } else if(char == seg_7) {
        d = seg7(p);
    } else if(char == seg_8) {
        d = seg8(p);
    } else if(char == seg_9) {
        d = seg9(p);
    } else if(char == seg_DP) {
        d = segDecimalPoint(p);
    }
    
    return d;
}

float ring0(vec2 p){
    vec2 prevP = p;
    p*=Rot(radians(-iTime*30.+50.));
    p = DF(p,16.0);
    p -= vec2(0.35);
    float d = B(p*Rot(radians(45.0)),vec2(0.005,0.03));
    p = prevP;
    
    p*=Rot(radians(-iTime*30.+50.));
    float deg = 165.;
    float a = radians(deg);
    d = max(dot(p,vec2(cos(a),sin(a))),d);
    a = radians(-deg);
    d = max(dot(p,vec2(cos(a),sin(a))),d);    
    
    p = prevP;
    p*=Rot(radians(iTime*30.+30.));
    float d2 = abs(length(p)-0.55)-0.015;
    d2 = max(-(abs(p.x)-0.4),d2);
    d = min(d,d2);
    p = prevP;
    d2 = abs(length(p)-0.55)-0.001;
    d = min(d,d2);
    
    p = prevP;
    p*=Rot(radians(-iTime*50.+30.));
    p+=sin(p*25.-radians(iTime*80.))*0.01;
    d2 = abs(length(p)-0.65)-0.0001;
    d = min(d,d2);
    
    p = prevP;
    a = radians(-sin(iTime*1.2))*120.0;
    a+=radians(-70.);
    p.x+=cos(a)*0.58;
    p.y+=sin(a)*0.58;
    
    d2 = abs(Tri(p*Rot(-a)*Rot(radians(90.0)),vec2(0.03),radians(45.)))-0.003;
    d = min(d,d2);      
    
    p = prevP;
    a = radians(sin(iTime*1.3))*100.0;
    a+=radians(-10.);
    p.x+=cos(a)*0.58;
    p.y+=sin(a)*0.58;
    
    d2 = abs(Tri(p*Rot(-a)*Rot(radians(90.0)),vec2(0.03),radians(45.)))-0.003;
    d = min(d,d2);     
    
    return d;
}

float ring1(vec2 p){
    vec2 prevP = p;
    float size = 0.45;
    float deg = 140.;
    float thickness = 0.02;
    float d = abs(length(p)-size)-thickness;
    
    p*=Rot(radians(iTime*60.));
    float a = radians(deg);
    d = max(dot(p,vec2(cos(a),sin(a))),d);
    a = radians(-deg);
    d = max(dot(p,vec2(cos(a),sin(a))),d);
    
    p = prevP;
    float d2 = abs(length(p)-size)-0.001;
    
    return min(d,d2);
}

float ring2(vec2 p){
    vec2 prevP = p;
    float size = 0.3;
    float deg = 120.;
    float thickness = 0.02;
    
    p*=Rot(-radians(sin(iTime*2.)*90.));
    float d = abs(length(p)-size)-thickness;
    float a = radians(-deg);
    d = max(dot(p,vec2(cos(a),sin(a))),d);
    a = radians(deg);
    d = max(dot(p,vec2(cos(a),sin(a))),d);
    
    float d2 = abs(length(p)-size)-thickness;
    a = radians(-deg);
    d2 = max(-dot(p,vec2(cos(a),sin(a))),d2);
    a = radians(deg);
    d2 = max(-dot(p,vec2(cos(a),sin(a))),d2);
    
    return min(d,d2);
}

float ring3(vec2 p){
    p*=Rot(radians(-iTime*80.-120.));
    
    vec2 prevP = p;
    float deg = 140.;
    
    p = DF(p,6.0);
    p -= vec2(0.3);
    float d = abs(B(p*Rot(radians(45.0)),vec2(0.03,0.025)))-0.003;
    
    p = prevP;
    float a = radians(-deg);
    d = max(dot(p,vec2(cos(a),sin(a))),d);
    a = radians(deg);
    d = max(dot(p,vec2(cos(a),sin(a))),d);
    
    p = prevP;
    
    p = DF(p,6.0);
    p -= vec2(0.3);
    float d2 = abs(B(p*Rot(radians(45.0)),vec2(0.03,0.025)))-0.003;
    
    p = prevP;
    a = radians(-deg);
    d2 = max(-dot(p,vec2(cos(a),sin(a))),d2);
    a = radians(deg);
    d2 = max(-dot(p,vec2(cos(a),sin(a))),d2);
    
    return min(d,d2);
}

float ring4(vec2 p){
    p*=Rot(radians(iTime*75.-220.));
    
    vec2 prevP = p;
    float deg = 20.;
    
    float d = abs(length(p)-0.25)-0.01;
    
    p = DF(p,2.0);
    p -= vec2(0.1);
    
    float a = radians(-deg);
    d = max(-dot(p,vec2(cos(a),sin(a))),d);
    a = radians(deg);
    d = max(-dot(p,vec2(cos(a),sin(a))),d);
    
    return d;
}

float ring5(vec2 p){
    p*=Rot(radians(-iTime*70.+170.));
    
    vec2 prevP = p;
    float deg = 150.;
    
    float d = abs(length(p)-0.16)-0.02;
    
    float a = radians(-deg);
    d = max(dot(p,vec2(cos(a),sin(a))),d);
    a = radians(deg);
    d = max(dot(p,vec2(cos(a),sin(a))),d);
    
    p = prevP;
    p*=Rot(radians(-30.));
    float d2 = abs(length(p)-0.136)-0.02;
    
    deg = 60.;
    a = radians(-deg);
    d2 = max(-dot(p,vec2(cos(a),sin(a))),d2);
    a = radians(deg);
    d2 = max(-dot(p,vec2(cos(a),sin(a))),d2);
    
    d = min(d,d2);
    
    return d;
}

float ring6(vec2 p){
    vec2 prevP = p;
    p*=Rot(radians(iTime*72.+110.));
    
    float d = abs(length(p)-0.95)-0.001;
    d = max(-(abs(p.x)-0.4),d);
    d = max(-(abs(p.y)-0.4),d);
    
    p = prevP;
    p*=Rot(radians(-iTime*30.+50.));
    p = DF(p,16.0);
    p -= vec2(0.6);
    float d2 = B(p*Rot(radians(45.0)),vec2(0.02,0.03));
    p = prevP;
    
    p*=Rot(radians(-iTime*30.+50.));
    float deg = 155.;
    float a = radians(deg);
    d2 = max(-dot(p,vec2(cos(a),sin(a))),d2);
    a = radians(-deg);
    d2 = max(-dot(p,vec2(cos(a),sin(a))),d2);    
    
    return min(d,d2);
}

float bg(vec2 p){
    p.y-=iTime*0.1;
    vec2 prevP = p;

    p*=2.8;
    vec2 gv = fract(p)-0.5;
    vec2 gv2 = fract(p*3.)-0.5;
    vec2 id = floor(p);

    float d = min(B(gv2,vec2(0.02,0.09)),B(gv2,vec2(0.09,0.02)));
    
    float n = Hash21(id);
    gv+=vec2(0.166,0.17);
    float d2 = abs(B(gv,vec2(0.169)))-0.004;
         
    if(n<0.3){
        gv*= Rot(radians(iTime*60.));
        d2 = max(-(abs(gv.x)-0.08),d2);
        d2 = max(-(abs(gv.y)-0.08),d2);
        d = min(d,d2);
    } else if(n>=0.3 && n<0.6){
        gv*= Rot(radians(-iTime*60.));
        d2 = max(-(abs(gv.x)-0.08),d2);
        d2 = max(-(abs(gv.y)-0.08),d2);
        d = min(d,d2);
    } else if(n>=0.6 && n<1.){
        gv*= Rot(radians(iTime*60.)+n);
        d2 = abs(length(gv)-0.1)-0.025;
        d2 = max(-(abs(gv.x)-0.03),d2);
        d = min(d,abs(d2)-0.003);
    }
    
    p = prevP;
    p = mod(p,0.02)-0.01;
    d2 = B(p,vec2(0.001));
    d = min(d,d2);
    
    return d;
}

float numberWithCIrcleUI(vec2 p){
    
    vec2 prevP = p;
    
    p*=SkewX(radians(-15.0));
    int num = int(mod(iTime*6.0,10.0));
    float d = drawFont(p-vec2(-0.16,0.),num);
    num = int(mod(iTime*3.0,10.0));
    float d2 = drawFont(p-vec2(-0.08,0.),num);
    d = min(d,d2); 
    d2 = drawFont(p-vec2(-0.02,0.),seg_DP);
    d = min(d,d2); 
    
    p*=1.5;
    num = int(mod(iTime*10.0,10.0));
    d2 = drawFont(p-vec2(0.04,-0.03),num);
    d = min(d,d2); 
    num = int(mod(iTime*15.0,10.0));
    d2 = drawFont(p-vec2(0.12,-0.03),num);
    d = abs(min(d,d2))-0.002; 
    
    p = prevP;
    
    p.x-=0.07;
    p*=Rot(radians(-iTime*50.));
    p = DF(p,4.);
    p -= vec2(0.085);
    d2 = B(p*Rot(radians(45.0)),vec2(0.015,0.018));
    p = prevP;
    d2 = max(-B(p,vec2(0.13,0.07)),d2);
    d = min(d,abs(d2)-0.0005);
    
    return d;
}

float blockUI(vec2 p){
    vec2 prevP = p;
    p.x+=iTime*0.05;
    p.y = abs(p.y)-0.02;
    p.x = mod(p.x,0.04)-0.02;
    float d = B(p,vec2(0.0085));
    p = prevP;
    p.x+=iTime*0.05;
    p.x+=0.02;
    p.x = mod(p.x,0.04)-0.02;
    float d2 = B(p,vec2(0.0085));
    d = min(d,d2);
    p = prevP;
    d = max((abs(p.x)-0.2),d);
    return abs(d)-0.0002;
}

float smallCircleUI(vec2 p){
    p*=1.1;
    vec2 prevP = p;
    
    float deg = 20.;
    
    p*=Rot(radians(sin(iTime*3.)*50.));
    float d = abs(length(p)-0.1)-0.003;
    
    p = DF(p,0.75);
    p -= vec2(0.02);
    
    float a = radians(-deg);
    d = max(-dot(p,vec2(cos(a),sin(a))),d);
    a = radians(deg);
    d = max(-dot(p,vec2(cos(a),sin(a))),d);
    
    p = prevP;
    p*=Rot(radians(-sin(iTime*2.)*80.));
    float d2 = abs(length(p)-0.08)-0.001;
    d2 = max(-p.x,d2);
    d = min(d,d2);
    
    p = prevP;
    p*=Rot(radians(-iTime*50.));
    d2 = abs(length(p)-0.05)-0.015;
    deg = 170.;
    a = radians(deg);
    d2 = max(-dot(p,vec2(cos(a),sin(a))),d2);
    a = radians(-deg);
    d2 = max(-dot(p,vec2(cos(a),sin(a))),d2); 
    d = min(d,abs(d2)-0.0005);
    
    return d;
}

float smallCircleUI2(vec2 p){
    vec2 prevP = p;
    float d = abs(length(p)-0.04)-0.0001;
    float d2 = length(p)-0.03;
    
    p*=Rot(radians(iTime*30.));
    float deg = 140.;
    float a = radians(deg);
    d2 = max(-dot(p,vec2(cos(a),sin(a))),d2);
    a = radians(-deg);
    d2 = max(-dot(p,vec2(cos(a),sin(a))),d2); 
    d = min(d,d2);
    
    d2 = length(p)-0.03;
    a = radians(deg);
    d2 = max(dot(p,vec2(cos(a),sin(a))),d2);
    a = radians(-deg);
    d2 = max(dot(p,vec2(cos(a),sin(a))),d2); 
    d = min(d, d2);
    
    d = max(-(length(p)-0.02),d);
    
    return d;
}

float rectUI(vec2 p){
    p*= Rot(radians(45.));
    vec2 prevP = p;
    float d = abs(B(p,vec2(0.12)))-0.003;
    p*= Rot(radians(iTime*60.));
    d = max(-(abs(p.x)-0.05),d);
    d = max(-(abs(p.y)-0.05),d);
    p = prevP;
    float d2 = abs(B(p,vec2(0.12)))-0.0005;
    d = min(d,d2);
    
    d2 = abs(B(p,vec2(0.09)))-0.003;
    p*= Rot(radians(-iTime*50.));
    d2 = max(-(abs(p.x)-0.03),d2);
    d2 = max(-(abs(p.y)-0.03),d2);
    d = min(d,d2);
    p = prevP;
    d2 = abs(B(p,vec2(0.09)))-0.0005;
    d = min(d,d2);
    
    p*=Rot(radians(-45.));
    p.y = abs(p.y)-0.07-sin(iTime*3.)*0.01;
    d2 = Tri(p,vec2(0.02),radians(45.));
    d = min(d,d2);
    
    p = prevP;
    p*=Rot(radians(45.));
    p.y = abs(p.y)-0.07-sin(iTime*3.)*0.01;
    d2 = Tri(p,vec2(0.02),radians(45.));
    d = min(d,d2);
    
    p = prevP;
    p*=Rot(radians(45.));
    d2 = abs(B(p,vec2(0.025)))-0.0005;
    d2 = max(-(abs(p.x)-0.01),d2);
    d2 = max(-(abs(p.y)-0.01),d2);
    d = min(d,d2);
    
    return d;
}

float graphUI(vec2 p){
    vec2 prevP = p;
    p.x+=0.5;
    p.y-=iTime*0.25;
    p*=vec2(1.,100.);

    vec2 gv = fract(p)-0.5;
    vec2 id = floor(p);
    
    float n = Hash21(vec2(id.y))*2.;
    
    float w = (abs(sin(iTime*n)+0.25)*0.03)*n*0.5;
    float d = B(gv,vec2(w, 0.1));
    
    p = prevP;
    d = max((abs(p.x)-0.2),d);
    d = max((abs(p.y)-0.2),d);
    
    return d;
}

float staticUI(vec2 p){
    vec2 prevP = p;
    float d = B(p,vec2(0.005,0.13));
    p-=vec2(0.02,-0.147);
    p*=Rot(radians(-45.));
    float d2 = B(p,vec2(0.005,0.028));
    d = min(d,d2);
    p = prevP;
    d2 = B(p-vec2(0.04,-0.2135),vec2(0.005,0.049));
    d = min(d,d2);
    p-=vec2(0.02,-0.28);
    p*=Rot(radians(45.));
    d2 = B(p,vec2(0.005,0.03));
    d = min(d,d2);
    p = prevP;
    d2 = length(p-vec2(0.,0.13))-0.012;
    d = min(d,d2);
    d2 = length(p-vec2(0.,-0.3))-0.012;
    d = min(d,d2);
    return d;
}

float arrowUI(vec2 p){
    vec2 prevP = p;
    p.x*=-1.;
    p.x-=iTime*0.12;
    p.x = mod(p.x,0.07)-0.035;
    p.x-=0.0325;
    
    p*=vec2(0.9,1.5);
    p*=Rot(radians(90.));
    float d = Tri(p,vec2(0.05),radians(45.));
    d = max(-Tri(p-vec2(0.,-0.03),vec2(0.05),radians(45.)),d);
    d = abs(d)-0.0005;
    p = prevP;
    d = max(abs(p.x)-0.15,d);
    return d;
}

float sideLine(vec2 p) {
    p.x*=-1.0;
    vec2 prevP = p;
    p.y = abs(p.y)-0.17;
    p*=Rot(radians(45.));
    float d = B(p,vec2(0.035,0.01));
    p = prevP;
    float d2 = B(p-vec2(0.0217,0.),vec2(0.01,0.152));
    d = min(d,d2);
    return abs(d)-0.0005;
}

float sideUI(vec2 p){
    vec2 prevP = p;
    p.x*=-1.;
    p.x+=0.025;
    float d = sideLine(p);
    p = prevP;
    p.y= abs(p.y)-0.275;
    float d2 = sideLine(p);
    d = min(d,d2);
    return d;
}

float overlayUI(vec2 p){
    vec2 prevP = p;
    
    float d = numberWithCIrcleUI(p-vec2(0.56,-0.34));
    p.x = abs(p.x)-0.56;
    p.y-=0.45;
    float d2 = blockUI(p);
    d = min(d,d2);
    p = prevP;
    
    p.x = abs(p.x)-0.72;
    p.y-=0.35;
    d2 = smallCircleUI2(p);
    d = min(d,d2);
    p = prevP;
    d2 = smallCircleUI2(p-vec2(-0.39,-0.42));
    d = min(d,d2);
    
    p = prevP;
    p.x -= 0.58;
    p.y -= 0.07;
    p.y = abs(p.y)-0.12;
    d2 = smallCircleUI(p);
    d = min(d,d2);
    
    p = prevP;
    d2 = rectUI(p-vec2(-0.58,-0.3));
    d = min(d,d2);
    
    p-=vec2(-0.58,0.1);
    p.x = abs(p.x)-0.05;
    d2 = graphUI(p);
    d = min(d,d2);
    p = prevP;
    
    p.x = abs(p.x)-0.72;
    p.y-=0.13;
    d2 = staticUI(p);
    d = min(d,d2);
    p = prevP;
    
    p.x = abs(p.x)-0.51;
    p.y-=0.35;
    d2 = arrowUI(p);
    d = min(d,d2);
    p = prevP;
    
    p.x = abs(p.x)-0.82;
    d2 = sideUI(p);
    d = min(d,d2);
    
    return d;
}

float GetDist(vec3 p) {
    vec3 prevP = p;
    
    p.z+=0.7;
    float maxThick = 0.03;
    float minThick = 0.007;
    float thickness = maxThick;
    float frame = mod(iTime,30.);
    float time = frame;
    if(frame>=10.0 && frame<20.){
        time = getTime(time-10.0,1.5);
        thickness = (maxThick+minThick)-cubicInOut(time)*maxThick;
    } else if(frame>=20.){
        time = getTime(time-20.0,1.5);
        thickness = minThick+cubicInOut(time)*maxThick;
    }    
    
    float d = ring0(p.xy);
    d = max((abs(p.z)-thickness),d);
    
    p.z-=0.2;
    float d2 = ring1(p.xy);
    d2 = max((abs(p.z)-thickness),d2);
    d = min(d,d2);
    
    p.z-=0.2;
    d2 = ring2(p.xy);
    d2 = max((abs(p.z)-thickness),d2);
    d = min(d,d2);
        
    p.z-=0.2;
    d2 = ring3(p.xy);
    d2 = max((abs(p.z)-thickness),d2);
    d = min(d,d2);
            
    p.z-=0.2;
    d2 = ring4(p.xy);
    d2 = max((abs(p.z)-thickness),d2);
    d = min(d,d2);
    
    p.z-=0.2;
    d2 = ring5(p.xy);
    d2 = max((abs(p.z)-thickness),d2);
    d = min(d,d2);
    
    p.z-=0.2;
    d2 = ring6(p.xy);
    d2 = max((abs(p.z)-thickness),d2);
    d = min(d,d2);
    
    return d;
}

vec3 RayMarch(vec3 ro, vec3 rd, int stepnum) {
    vec3 res = vec3(0.0);
    float steps = 0.0;
    float alpha = 0.0;
    
    float tmax = 5.;
    float t = 0.0;
    
    float glowVal = 0.003;
    
    for(float i=0.; i<float(stepnum); i++) {
        steps = i;
        vec3 p = ro + rd*t;
        float d = GetDist(p);
        float absd = abs(d);

        if (t > tmax) break;

        alpha += 1.0 - smoothstep(0.0,glowVal, d);        
        t += max(0.0001, absd * 0.6);
    }
    alpha /= steps;
    
    res += alpha * vec3(1.5);
    return res;
}

vec3 R(vec2 uv, vec3 p, vec3 l, float z) {
    vec3 f = normalize(l-p),
        r = normalize(cross(vec3(0,1,0), f)),
        u = cross(f,r),
        c = p+f*z,
        i = c + uv.x*r + uv.y*u,
        d = normalize(i-p);
    return d;
}

void main( )
{
    vec2 uv = (xlv_TEXCOORD0-.5*iResolution.xy)/iResolution.y;
    vec2 m =  iMouse.xy/iResolution.xy;
    
    vec3 ro = vec3(0, 0, -2.1);
    if(iMouse.z>0.){
        ro.yz *= Rot(m.y*3.14+1.);
        ro.y = max(-0.9,ro.y);
        ro.xz *= Rot(-m.x*6.2831);
    } else {
    
        float YZ = 45.;
        float ogRXZ = 50.;
        float animRXZ = 20.;
        
        float frame = mod(iTime,30.);
        float time = frame;

        if(frame>=10.0 && frame<20.){
            time = getTime(time-10.0,1.5);
            
            YZ = 45.-cubicInOut(time)*45.0;
            ogRXZ = 50.-cubicInOut(time)*50.0;
            animRXZ = 20.-cubicInOut(time)*20.0;
        } else if(frame>=20.){
            time = getTime(time-20.0,1.5);
            
            YZ = cubicInOut(time)*45.0;
            ogRXZ = cubicInOut(time)*50.0;
            animRXZ = cubicInOut(time)*20.0;
        }
    
        ro.yz *= Rot(radians(YZ));
        ro.xz *= Rot(radians(sin(iTime*0.3)*animRXZ+ogRXZ));
    }
    
    vec3 rd = R(uv, ro, vec3(0,0.0,0), 1.0);
    vec3 d = RayMarch(ro, rd, MAX_STEPS);
    vec3 col = vec3(.0);
    float bd = bg(uv);
    col = mix(col,vec3(1.),S(bd,0.0));

    col=mix(col,d.xyz,0.7);
    // gamma correction
    col = pow( col, vec3(0.9545) );    

    float d2 = overlayUI(uv);
    col = mix(col,vec3(1.),S(d2,0.0));
    
    gl_FragColor = vec4(col,1.0);
}
