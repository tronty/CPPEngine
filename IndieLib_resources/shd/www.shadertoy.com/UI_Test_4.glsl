#define Rot(a) mat2(cos(a),-sin(a),sin(a),cos(a))
#define antialiasing(n) (1.0*n)/min(iResolution.y,iResolution.x)
#define S(d,b) smoothstep(antialiasing(1.0),b,d)
#define B(p,s) max(abs(p).x-s.x,abs(p).y-s.y)
#define Tri(p,s,a) max(-dot(p,vec2(cos(-a),sin(-a))),max(dot(p,vec2(cos(a),sin(a))),max(abs(p).x-s.x,abs(p).y-s.y)))
#define DF(a,b) length(a) * cos( mod( atan(a.y,a.x)+6.28/(b*8.0), 6.28/((b*8.0)*0.5))+(b-1.)*6.28/(b*8.0) + vec2(0,11) )
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

float Hash21(vec2 p) {
    p = fract(p*vec2(234.56,789.34));
    p+=dot(p,p+34.56);
    return fract(p.x+p.y);
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
    }
    
    return d;
}
float barCode(vec2 p){
    p*=1.1;
    vec2 prevP = p;
    p.x+=iTime*0.5;
    p*=15.0;
    vec2 gv = fract(p)-0.5;
    vec2 id = floor(p);

    float n = Hash21(vec2(id.x))*5.;
    
    p.x = mod(p.x,0.2)-0.1;
    float d = abs(p.x)-((0.01*n)+0.01);
    
    p = prevP;
    d = max(abs(p.x)-0.15,d);
    d = max(abs(p.y)-0.1,d);

    float d2 = abs(B(p,vec2(0.16,0.11)))-0.001;
    d2 = max(-(abs(p.x)-0.14),d2);
    d2 = max(-(abs(p.y)-0.09),d2);

    return min(d,d2);
}

float circleUI(vec2 p){
    vec2 prevP = p;
    float speed = 3.;
    mat2 animRot = Rot(radians(iTime*speed)*30.0);
    p*=animRot;
    
    p = DF(p,32.0);
    p -= vec2(0.28);
    
    float d = B(p*Rot(radians(45.0)), vec2(0.002,0.02));
    
    p = prevP;
    p*=animRot;
    
    float a = radians(130.);
    d = max(dot(p,vec2(cos(a),sin(a))),d);
    a = radians(-130.);
    d = max(dot(p,vec2(cos(a),sin(a))),d);
    
    p = prevP;
    animRot = Rot(radians(iTime)*20.0);
    p*=animRot;
    
    p = DF(p,24.0);
    p -= vec2(0.19);
    
    float d2 = B(p*Rot(radians(45.0)), vec2(0.003,0.015));
    
    p = prevP;
    p*=animRot;
    
    a = radians(137.5);
    d2 = max(dot(p,vec2(cos(a),sin(a))),d2);
    a = radians(-137.5);
    d2 = max(dot(p,vec2(cos(a),sin(a))),d2);
    
    d = min(d,d2);
    

    p = prevP;
    animRot = Rot(-radians(iTime*speed)*25.0);
    p*=animRot;
    
    p = DF(p,16.0);
    p -= vec2(0.16);
    
    d2 = B(p*Rot(radians(45.0)), vec2(0.003,0.01));
    
    p = prevP;
    p*=animRot;
    
    a = radians(25.5);
    d2 = max(dot(p,vec2(cos(a),sin(a))),d2);
    a = radians(-25.5);
    d2 = max(dot(p,vec2(cos(a),sin(a))),d2);
    
    d = min(d,d2);
    
    
    p = prevP;
    animRot = Rot(radians(iTime*speed)*35.0);
    p*=animRot;
    
    p = DF(p,8.0);
    p -= vec2(0.23);
    
    d2 = B(p*Rot(radians(45.0)), vec2(0.02,0.02));
    
    p = prevP;
    p*=animRot;
    
    a = radians(40.0);
    d2 = max(dot(p,vec2(cos(a),sin(a))),d2);
    a = radians(-40.0);
    d2 = max(dot(p,vec2(cos(a),sin(a))),d2);
    
    d = min(d,d2);    
    
    
    p = prevP;
    
    animRot = Rot(radians(iTime*speed)*15.0);
    p*=animRot;
    
    d2 = abs(length(p)-0.36)-0.002;
    d2 = max(abs(p.x)-0.2,d2);
    d = min(d,d2);    
    
    p = prevP;
    
    animRot = Rot(radians(90.)+radians(iTime*speed)*38.0);
    p*=animRot;
    
    d2 = abs(length(p)-0.245)-0.002;
    d2 = max(abs(p.x)-0.1,d2);
    d = min(d,d2);    
    
    p = prevP;
    d2 = abs(length(p)-0.18)-0.001;
    d = min(d,d2);       
    
    p = prevP;
    animRot = Rot(radians(145.)+radians(iTime*speed)*32.0);
    p*=animRot;
    d2 = abs(length(p)-0.18)-0.008;
    
    a = radians(30.0);
    d2 = max(dot(p,vec2(cos(a),sin(a))),d2);
    a = radians(-30.0);
    d2 = max(dot(p,vec2(cos(a),sin(a))),d2);    
    
    d = min(d,d2);  
    
    p = prevP;
    
    a = radians(iTime*speed)*30.0;
    p.x+=cos(a)*0.45;
    p.y+=sin(a)*0.45;
    
    d2 = Tri(p*Rot(-a)*Rot(radians(90.0)),vec2(0.02),radians(45.));
    d = min(d,d2);  
    
    p = prevP;
    
    a = radians(-sin(iTime*speed*0.5))*120.0;
    a+=radians(-70.);
    p.x+=cos(a)*0.45;
    p.y+=sin(a)*0.45;
    
    d2 = abs(Tri(p*Rot(-a)*Rot(radians(90.0)),vec2(0.02),radians(45.)))-0.001;
    d = min(d,d2);      
    
    p = prevP;
    animRot = Rot(-radians(iTime*speed)*27.0);
    p*=animRot;
    
    d2 = abs(length(p)-0.43)-0.0001;
    d2 = max(abs(p.x)-0.3,d2);
    d = min(d,d2);    
    
    p = prevP;
    animRot = Rot(-radians(iTime*speed)*12.0);
    p*=animRot;
    
    p = DF(p,8.0);
    p -= vec2(0.103);
    
    d2 = B(p*Rot(radians(45.0)), vec2(0.001,0.007));    
    d = min(d,d2);  
    
    p = prevP;
    animRot = Rot(radians(16.8)-radians(iTime*speed)*12.0);
    p*=animRot;    
    
    p = DF(p,8.0);
    p -= vec2(0.098);
    
    d2 = B(p*Rot(radians(45.0)), vec2(0.001,0.013));    
    d = min(d,d2);      
    
    
    p = prevP;
    animRot = Rot(radians(iTime*speed)*30.0);
    p*=animRot;    
    
    p = DF(p,10.0);
    p -= vec2(0.28);
    
    d2 = abs(B(p*Rot(radians(45.0)), vec2(0.02,0.02)))-0.001;
    
    p = prevP;
    p*=animRot;
    
    a = radians(50.);
    d2 = max(dot(p,vec2(cos(a),sin(a))),d2);
    a = radians(-50.);
    d2 = max(dot(p,vec2(cos(a),sin(a))),d2);    
    d = min(d,d2);   
    
    p = prevP;
    int num = int(mod(iTime*10.0,10.0));
    d2 = drawFont(p-vec2(0.038,0.),num);
    d = min(d,abs(d2)-0.001); 
    num = int(mod(iTime*3.0,10.0));
    d2 = drawFont(p-vec2(-0.038,0.),num);
    d = min(d,d2); 
    
    return d;
}

float smallCircleUI(vec2 p){
    p*=1.3;
    vec2 prevP = p;
    float speed = 3.;
    
    mat2 animRot = Rot(radians(iTime*speed)*35.0);
    p*=animRot;  
    
    float d = abs(length(p)-0.2)-0.005;
    
    float a = radians(50.);
    d = max(dot(p,vec2(cos(a),sin(a))),d);
    a = radians(-50.);
    d = max(dot(p,vec2(cos(a),sin(a))),d);   
    
    p*=Rot(radians(10.));
    float d2 = abs(length(p)-0.19)-0.006;
    
    a = radians(60.);
    d2 = max(dot(p,vec2(cos(a),sin(a))),d2);
    a = radians(-60.);
    d2 = max(dot(p,vec2(cos(a),sin(a))),d2);   
    
    d = min(d,d2);
    
    p = prevP;
    
    d2 = abs(length(p)-0.195)-0.0001;
    d = min(d,d2);
    
    
    p = prevP;
    animRot = Rot(-radians(iTime*speed)*30.0);
    p*=animRot;      
    
    p = DF(p,12.0);
    p -= vec2(0.11);
    
    d2 = B(p*Rot(radians(45.0)), vec2(0.003,0.015));      
    

    
    d = min(d,d2);  
    
    p = prevP;
    animRot = Rot(radians(iTime*speed)*23.0);
    p*=animRot;  
    p = DF(p,2.5);
    p -= vec2(0.05);
    
    d2 = B(p*Rot(radians(45.0)), vec2(0.01));      
    d = min(d,d2); 
    
    p = prevP;
    animRot = Rot(-radians(iTime*speed)*26.0);
    p*=animRot;  
    d2 = abs(length(p)-0.11)-0.005;
    
    d2 = max(abs(p.x)-0.05,d2);
    
    d = min(d,d2);
    
    return d;
}

float smallCircleUI2(vec2 p){
    p.x = abs(p.x)-0.4;
    p.y = abs(p.y)-0.34;
    vec2 prevP = p;
    float speed = 3.;
    mat2 animRot = Rot(radians(iTime*speed)*28.0);
    p*=animRot;  
    
    float d = abs(length(p)-0.028)-0.0005;
    d = max(B(p,vec2(0.015,0.1)),d);
    
    p = prevP;
    animRot = Rot(-radians(iTime*speed)*31.0);
    p*=animRot;  
    float d2 = abs(length(p)-0.027)-0.004;
    
    float a = radians(50.);
    d2 = max(dot(p,vec2(cos(a),sin(a))),d2);
    a = radians(-50.);
    d2 = max(dot(p,vec2(cos(a),sin(a))),d2);    
    
    d = min(max(-d2,d),abs(d2)-0.001);
    
    p = prevP;
    animRot = Rot(-radians(iTime*speed)*30.0);
    p*=animRot;      
    
    p = DF(p,2.0);
    p -= vec2(0.008);
    
    d2 = B(p*Rot(radians(45.0)), vec2(0.0005,0.002));       
    d = min(d,d2);
    
    return d;
}

float mainWave(vec2 p){
    p*=1.5;
    
    float thickness = 0.003;
    
    vec2 prevP = p;

    float t = fract(sin(iTime*100.0))*0.5;

    p.x+=iTime*1.0;
    p.y+=sin(p.x*8.)*(0.05+abs(sin(t*10.0)*0.12));
    float d = abs(p.y)-thickness;

    p = prevP;
    
    p.x-=iTime*0.5;
    p.y+=sin(p.x*3.)*(0.1+abs(sin(t*9.0)*0.13));
    float d2 = abs(p.y)-thickness;

    d = min(d,d2);

    p = prevP;
    
    p.x+=iTime*0.7;
    p.y+=sin(p.x*5.)*(0.1+abs(sin(t*9.3)*0.15));
    d2 = abs(p.y)-thickness;

    d = min(d,d2);
    
    p = prevP;
    
    p.x-=iTime*0.6;
    p.y+=sin(p.x*10.)*(0.1+abs(sin(t*9.5)*0.08));
    d2 = abs(p.y)-thickness;

    d = min(d,d2);
        
    p = prevP;
    
    p.x+=iTime*1.2;
    p.y+=cos(-p.x*15.)*(0.1+abs(sin(t*10.0)*0.1));
    d2 = abs(p.y)-thickness;

    d = min(d,d2);
    
    return d;
}

float graph(vec2 p){
    vec2 prevP = p;
    float d = 10.;
    float t = iTime+Hash21(vec2(floor(p.y-0.5),0.0));
    p.y = abs(p.y);
    p.y+=0.127;
    for(float i = 1.0; i<=20.0; i+=1.0) {
        float x = 0.0;
        float y = i*-0.015;
        float w = abs(sin(Hash21(vec2(i,0.0))*t*3.0)*0.1);
        float d2 = B(p+vec2(0.1-w,y),vec2(w,0.003));
        d = min(d,d2);
    }
    p = prevP;
    
    return max(abs(p.y)-0.2,d);
}

float scifiUI(vec2 p){
    p*=1.1;
    vec2 prevP = p;
    float d = B(p,vec2(0.15,0.06));
    float a = radians(45.);
    p.x = abs(p.x)-0.195;
    p.y = abs(p.y);
    float m = dot(p,vec2(cos(a),sin(a)));
    d = max(m,d);
    
    p = prevP;
    
    p.x+=0.16;
    p.y+=0.008;
    float d2 = B(p,vec2(0.06,0.052));
    a = radians(45.);
    p.x = abs(p.x)-0.095;
    p.y = abs(p.y);
    m = dot(p,vec2(cos(a),sin(a)));
    d2 = max(m,d2);
    
    p = prevP;
    d2 = min(d,d2);
    d2 = max(-B(p-vec2(-0.03,-0.05),vec2(0.2,0.05)),abs(d2)-0.003);
    
    return abs(d2)-0.001;
}

float triAnimatin(vec2 p){
    p.x = abs(p.x)-0.458;
    p.y = abs(p.y)-0.45;
    vec2 prevP = p;
    p.x+=iTime*0.1;
    p.x=mod(p.x,0.04)-0.02;
    p.x+=0.01;
    float d = abs(Tri(p*Rot(radians(-90.)),vec2(0.012),radians(45.)))-0.0001;
    p = prevP;
    return max(abs(p.x)-0.125,d);
}

float randomDotLine(vec2 p){
    vec2 prevP = p;
    p.x+=iTime*0.08;
    vec2 gv = fract(p*17.0)-0.5;
    vec2 id = floor(p*17.0);
    
    float n = Hash21(id);
    float d = B(gv,vec2(0.25*(n*2.0),0.2));
    p = prevP;
    p.y+= 0.012;
    d = max(abs(p.y)-0.01,max(abs(p.x)-0.27,d));
    return d;
}

float scifiUI2(vec2 p){
    vec2 prevP = p;

    p*=1.2;
    p.x= abs(p.x)-0.72;
    p.y= abs(p.y)-0.53;
    
    float d = B(p,vec2(0.03));
    float a = radians(-45.);
    
    float m = -dot(p-vec2(-0.005,0.0),vec2(cos(a),sin(a)));
    d = max(m,d);
    m = dot(p-vec2(0.005,0.0),vec2(cos(a),sin(a)));
    d = max(m,d);
    
    float d2 = B(p-vec2(0.175,0.0256),vec2(0.15,0.004));
    d = min(d,d2);
    d2 = B(p-vec2(-0.175,-0.0256),vec2(0.15,0.004));
    d = abs(min(d,d2))-0.0005;
    
    p.y-=0.003;
    p.x+=iTime*0.05;
    p.x = mod(p.x,0.03)-0.015;
    p.x-=0.01;
    d2 = B(p,vec2(0.026));
    
    m = -dot(p-vec2(-0.005,0.0),vec2(cos(a),sin(a)));
    d2 = max(m,d2);
    m = dot(p-vec2(0.005,0.0),vec2(cos(a),sin(a)));
    d2 = max(m,d2);
    
    p = prevP;
    p*=1.2;
    p.x= abs(p.x)-0.72;
    p.y= abs(p.y)-0.53;
    m = -dot(p-vec2(0.02,0.0),vec2(cos(a),sin(a)));
    d2 = max(m,d2);
    m = dot(p-vec2(0.32,0.0),vec2(cos(a),sin(a)));
    d2 = max(m,d2);
    
    d = min(d,d2);
    
    p = prevP;
    
    d2 = triAnimatin(p);
    d = min(d,d2);
    
    
    p = prevP;
    p.x= abs(p.x)-0.6;
    p.y= abs(p.y)-0.418;
    
    d2 = randomDotLine(p);
    d = min(d,d2);
    
    return d;
}

float scifiUI3Base(vec2 p){
    float d = abs(length(p)-0.03)-0.01;
    p.x=abs(p.x)-0.1;
    float d2 = abs(length(p)-0.03)-0.01;
    d = min(d,d2);
    return d;
}

float scifiUI3(vec2 p){
    vec2 prevP = p;
    float speed = 3.;
    float d = abs(length(p)-0.03)-0.01;
    
    mat2 animRot = Rot(radians(iTime*speed)*40.0);
    p*=animRot;  
    
    float a = radians(50.);
    d = max(dot(p,vec2(cos(a),sin(a))),d);
    a = radians(-50.);
    d = max(dot(p,vec2(cos(a),sin(a))),d);   
    
    p = prevP;
    p.x=abs(p.x)-0.1;
    animRot = Rot(radians(iTime*speed)*45.0);
    p*=animRot;  
    
    
    float d2 = abs(length(p)-0.03)-0.01;
    
    a = radians(170.);
    d2 = max(dot(p,vec2(cos(a),sin(a))),d2);
    a = radians(-170.);
    d2 = max(dot(p,vec2(cos(a),sin(a))),d2);   
    
    return min(d,d2);
}

float slider(vec2 p){
    vec2 prevP = p;
    
    float d = abs(B(p,vec2(0.15,0.015)))-0.001;
    float d2 = B(p-vec2(sin(iTime*1.5)*0.13,0),vec2(0.02,0.013));
    d = min(d,d2);
    
    p.y = abs(p.y)-0.045;
    d2 = abs(B(p,vec2(0.15,0.015)))-0.001;
    d = min(d,d2);
    d2 = B(p-vec2(sin(iTime*2.0)*-0.13,0),vec2(0.02,0.013));
    d = min(d,d2);
    
    p = prevP;
    p.y=abs(p.y);
    d2 = scifiUI(p-vec2(0.032,0.045));
    d = min(d,d2);
    
    return d;
}

float bg(vec2 p){
    p = mod(p,0.3)-0.15;
    float d = B(p,vec2(0.001,0.01));
    float d2 = B(p,vec2(0.01,0.001));
    d = min(d,d2);
    return d;
}

void main( )
{
    vec2 uv = (2.0*xlv_TEXCOORD0.xy-iResolution.xy)/min(iResolution.y,iResolution.x);
    uv*=1.6;
    //vec3 col =vec3(0.0);
    vec3 col = vec3(0.9,0.9,0.9);
    
    float d = bg(uv);
    col = mix(col, vec3(0.3),S(d,0.0));
    
    // bg color
    vec3 bcol = vec3(0.3,0.75,0.7);
    
	float r3 = length( uv+vec2(-0.5,0.75) )*3.0;
    float r4 = length( uv+vec2(-0.5,0.75) )*3.5;
    
    col = mix( col, vec3(0.2,0.2,0.2), 1.0-smoothstep(0.98,1.0,r3) );
    col = mix( col, bcol, 1.0-smoothstep(0.98,1.0,r4) );
#if 0
    d = mainWave(uv);
    col = mix(col, vec3(1.),S(d,-0.005));
#endif
#if 0
    d = scifiUI2(uv);
    
    float d2 = circleUI(uv);
    d = min(d,d2);

    d2 = smallCircleUI(uv-vec2(-0.62,-0.22));
    d = min(d,d2);
    
    d2 = smallCircleUI2(uv);
    d = min(d,d2);
    
    d2 = graph(uv-vec2(-0.67,0.19));
    d = min(d,d2);
    
    d2 = barCode(uv-vec2(0.63,-0.27));
    d = min(d,d2);
    
    d2 = slider(uv-vec2(0.62,0.26));
    d = min(d,d2);
    col = mix(col, vec3(1.),S(d,0.0));
#endif
#if 1
    d = scifiUI3Base(uv-vec2(0.65,0.));
    col = mix(col, col+vec3(0.5),S(d,0.0));
#endif
#if 1
    d = scifiUI3(uv-vec2(0.65,0.));
    col = mix(col, vec3(1.),S(d,0.0));
#endif
    gl_FragColor = vec4(col,1.0);
}
