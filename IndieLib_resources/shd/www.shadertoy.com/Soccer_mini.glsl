//CLICK MOUSE AND MOVE


#define W 5
#define rot(a) mat2(cos(a),sin(a),-sin(a),cos(a))
#define m(v2,deg) mod((v2)*rot(radians(deg)),60.0)-30.0
#define hex() o1=m(xlv_TEXCOORD0,0.);o2=m(xlv_TEXCOORD0,60.);o3=m(xlv_TEXCOORD0,120.);rgba+=clamp(vec4(abs(o1.y)+abs(o2.y)+abs(o3.y)-54.),0.1,1.0);

void main(){
    gl_FragColor=vec4(0.33,1.0,0.5,1);
    vec2 R=iResolution.xy,iM=((length(iMouse.z)>=1.0)?vec2(iMouse.x,iMouse.y)*1.0:R/2.0)*min(R.x,R.y)/360.0;
    xlv_TEXCOORD0-=R/2.;vec2 a=xlv_TEXCOORD0,M=xlv_TEXCOORD0,field=xlv_TEXCOORD0;xlv_TEXCOORD0/=min(R.x,R.y)/360.0;;field/=min(R.x,R.y)/360.0;
    vec2 o1,o2,o3;float iT=iTime;
    xlv_TEXCOORD0-=(-iM/min(R.x,R.y)*360.0+R/2.0)/min(R.x,R.y)*360.0*(vec2(sign(cos(iT/3.14))*cos(iT/3.14)*cos(iT/3.14),sin(iT/1.57)));
    xlv_TEXCOORD0/=(abs(cos(radians(iTime*180.0)))+0.5)/1.5;
    float scale=7.0,x=xlv_TEXCOORD0.x/scale,y=xlv_TEXCOORD0.y/scale,
    z=sqrt(x*x+y*y)/scale;xlv_TEXCOORD0/=cos(z);
    //field
    ivec4 f=ivec4(int(abs(field.x))/W,int(abs(field.y))/W,int((abs(field.x)-0.499)/float(W)*2.0),int(length(field)/float(W)));
    if(((f.x-10*W)*(f.y-5*W)==0||f.x>=8*W&&f.y==2*W||f.x==8*W&&f.y<=2*W||(f.z)==0)&&(f.y<=5*W&&f.x<=10*W)||f.w==2*W)
      gl_FragColor=vec4(2.0,2.2,2.0,1);
    if(z<1.0){//ball
      gl_FragColor=vec4(0,0,0,1);
      xlv_TEXCOORD0+=vec2(cos(iTime/1.0)*90.0,-sin(iTime/1.0)*90.0);hex();xlv_TEXCOORD0.y+=40.;hex();
      gl_FragColor*=(pow(1.0-cos(z),2.0)+3./4.0);
      if(gl_FragColor.x<=0.5)gl_FragColor*=pow(cos(z),-2.0)/2.0;
    }else{//grass
      gl_FragColor*=(cos(length(mod(a,R/10.5))*length(mod(a,R/10.5))*5.0)+2.0)/3.0;
      if(-length(M+R/2.-iM/min(R.x,R.y)*360.0)>=-16.*min(R.x,R.y)/360.0){//Player
       gl_FragColor=clamp(gl_FragColor,0.0,1.0)*0.5;
      }if(z<2.){//shadow
       gl_FragColor=clamp(gl_FragColor,0.0,1.0)+clamp((z-1.1)/((abs(cos(radians(iTime*180.0)))+0.5)/1.5),-1.0,0.0);
}   } }

