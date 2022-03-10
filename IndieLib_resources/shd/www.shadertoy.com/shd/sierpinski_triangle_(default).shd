// https://www.shadertoy.com/view/fd3SDM
void mainImage(out vec4 color, in vec2 o) {
    
    float SIZE=iResolution.y-120.0;        // FULL HEIGHT;
    float size=(iResolution.y-120.0)/16.0; // ELEMENTS HEIGHT
    
    o.x=iResolution.x/2.0-o.x;
    o.y=iResolution.y-SIZE/4.0-o.y;
    
    float                   A,B,C,         a,b,c;  
    A = o.y;                       a=mod(A,size);
    B = o.y*0.5+o.x*sqrt(3.0)/2.0; b=mod(B,size);
    C = o.y*0.5-o.x*sqrt(3.0)/2.0; c=mod(C,size);
    
    if(a>b&&C>0.0&&B>0.0&&o.y<SIZE) {
        if((int(B/size)&int(C/size))==0)color=vec4(0,0,0,1);
        else color=vec4(1,1,0,1);
    }else if(a<b&&C>0.0&&B>0.0&&o.y<SIZE) {
        color=vec4(0,1,0.5,1);
    }else color=vec4(1); 
}

