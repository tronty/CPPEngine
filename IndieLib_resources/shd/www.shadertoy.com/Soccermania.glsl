// Soccermania by Kastorp
//-------------------------------
// controls:
//   use mouse to control  player and ball direction
//   A= shot
//   S= pass
// toggles:
//   E= practice on/off 
//   C= auto shot on/off
//--------------------------------

float pitch(vec2 uv)
{
    float d =1e5;
    uv=abs(uv);
    d=min(d,abs(length(uv) - .18));
    d=min(d,max(uv.x- PC.x +.34, abs(length(uv-vec2(PC.x-.27,0)) - .18)));
    d=min(d,abs(sdBox(uv-vec2(PC.x/2.,0),vec2(PC.x/2.,PC.y))));
    d=min(d,abs(sdBox(uv-vec2(PC.x-.17,0),vec2(.17,.4))));
    d=min(d,abs(sdBox(uv-vec2(PC.x-.05,0),vec2(.05,.15))));
    d=min(d,length(uv-vec2(PC.x-.25,0.)));
    return d;
}


void main()
{
    vec2 uv = position(xlv_TEXCOORD0);
    vec3 col = vec3(0.2,0.5,0.1)*(.9+.1*step(0.1,mod(uv.x,.2)))*(.9+.1*step(0.1,mod(uv.y,.2)));         
    col=mix(col,vec3(1), smoothstep(tk,.0,pitch(uv)));    
    col=mix(col,vec3(.7), smoothstep(tk,.0,sdBox(abs(uv)-vec2(PC.x+.02,0),vec2(.02,.07))));
    
    ivec4 mode= ivec4( texelFetch(iChannel0,ivec2(24,0),0));
    ivec4 sc= ivec4( texelFetch(iChannel0,ivec2(25,0),0));
    drawChar(iChannel2, col, vec3(0.,.7,0), uv, vec2(-0.05,.45), vec2(.1), 48+sc.x);
    drawChar(iChannel2,col, vec3(0.,.7,0), uv, vec2(0.05,.45), vec2(.1), 48+sc.y);


    vec4 ball=  texelFetch(iChannel0,ivec2(0),0);         
    vec4 zBall=texelFetch(iChannel1,ivec2(coord(ball.xy)),0);
    
    for(int i=0;i<=22;i++)
    {
        vec4 pl=  texelFetch(iChannel0,ivec2(i,0),0);
        vec2 pd=(uv-pl.xy)*mat2(cos(pl.z),sin(pl.z),-sin(pl.z),cos(pl.z));
        
        float fl= abs(mod(pl.w*.5,0.04)-0.02);
        if(i>0 && sdBox(uv-pl.xy,vec2(.03))<0. ) {
            vec3 c=((int(mod(float(i),11.0)))==1?vec3(0,1,0): (i<12? vec3(1,0,0):vec3(0,.7,1)));
            col=mix(col, vec3(0), smoothstep(tk,.0,sdBox(pd +vec2(-fl+.005,.01),vec2(.005,.002))  ));
            col=mix(col, vec3(0), smoothstep(tk,.0,sdBox(pd +vec2(-.015+fl,-.01),vec2(.005,.002))  ));
            col=mix(col, c, smoothstep(tk,.0,sdBox(pd ,vec2(.006,.012))  ));
            col=mix(col, c, smoothstep(tk,.0,sdBox(pd +vec2(-.006+fl*.5,.014),vec2(.005,.002))  ));
            col=mix(col, c, smoothstep(tk,.0,sdBox(pd +vec2(-fl*.5+.002,-.014),vec2(.005,.002))  ));
            
            vec3 head=vec3(0.4,0.2,0.2);
            if(iMouse.z>0. && zBall.x<11.5  && int(zBall.x)==i)  head=vec3(1,1,0);
            if(iMouse.z>0. && zBall.z<11.5  && int(zBall.z)==i)  head=vec3(1,1,0);
            col=mix(col, head,smoothstep(tk,.0,-.007+length(uv-pl.xy)));
            
        }else if(i==0 && length(uv-pl.xy)<.01){
        //from https://www.shadertoy.com/view/fsBfz3 "soccer ball 3d" by 4eckme  
          #define o(a,i)  abs( mod( dot( i, vec2(-sin(a),cos(a)) ) , 60.) - 30. ) 
          vec2 i = (uv-pl.xy)*100.; 
          float z=length(i);                          
          i = i/cos(z)*50.-pl.xy*1500.;
          col*=0.;
          col +=  smoothstep(-5.,5.,o(0.,i)+o(1.05,i)+o(2.1,i) -54.);
          i.y += 40.;
          col += smoothstep(-5.,5.,o(0.,i)+o(1.05,i)+o(2.1,i) -54.);
          col *= cos(z);
        }
    }


    if(iMouse.z>0.) {
        col=mix(col, vec3(1,1,0), smoothstep(.001,.0,-.005*R.y+length(xlv_TEXCOORD0-iMouse.xy)));         
    }
    
#ifdef DEBUG
    vec4 d=texelFetch(iChannel1,ivec2(xlv_TEXCOORD0),0);
    int j = int(d.x);
    vec4 zBallt=texelFetch(iChannel1,ivec2(coord(ball.xy+ball.zw*1.)),0);
    //team zones
    col=mix(col, (int(zBall.x)==j || int(zBall.z)==j?3.:1. )*  (int(mod(float(j),11.0))==1?vec3(1,0,1): (j<12? vec3(1,0,0):vec3(0,0,1))), smoothstep(0.1,.0,d.w-d.y)*.1);
    vec4 b=texelFetch(iChannel0,ivec2(0,0),0);
    col=mix(col, vec3(1,1,0),smoothstep(tk,.0,-.01+length(uv-b.xy-b.zw*1.))); //ball target 
    vec4 pl=texelFetch(iChannel0,ivec2(zBall.x,0),0);
    col=mix(col, vec3(1,0,1),smoothstep(tk,.0,-.01+length(uv-pl.xy))); //closest player
    vec4 plt=texelFetch(iChannel0,ivec2(zBallt.x,0),0);
    col=mix(col, vec3(0,1,1),smoothstep(tk,.0,-.01+length(uv-plt.xy))); //target player   
#endif
   
    gl_FragColor = vec4(col,1.0);    
}

