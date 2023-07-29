//TODO : add chromatic abberation and dirty lens
/*
    Feel free to use this code for your own shader. 
*/
void main( )
{
    vec2 uv = xlv_TEXCOORD0/iResolution.xy;
    uv-=.5;
    uv.x*=iResolution.x/iResolution.y ;
    
    float time = iTime*0.5 ;
    
    vec3 rd = normalize(vec3(uv.x+sin(time-90.)*0.5,uv.y-0.2, 1.)) ;
    vec3 focus = normalize(vec3(0.+sin(time-90.)*0.5 ,-0.2,1.)) ;
    
    vec3 sundir= normalize(vec3(-0.,0.,1.));
    vec3 col = vec3(0.);
    
    //sun
    col +=0.05/length(rd-sundir)*vec3(1.) ;
    //sun halo
    col+=0.06/length(sundir-rd)*0.1*vec3(1.,0.5,0.) ;
    
    //focus/sundir vector
    vec3 dir = sundir-focus;
    float size = length(dir) ;
    
    //define Lens size and step length
    float lenses = 3. ;
    float stepL = size*3./lenses ;
    
    //firts Lens 
    vec3 pos = sundir+dir*stepL*0.5;
    vec3 roP = normalize(pos) ;
    
    float halo = smoothstep(0.5,0.49,length(rd-roP)/0.5);
    halo-=smoothstep(0.5,0.4,length(rd-roP)/0.49);
    col+=max(halo,0.)*0.2*vec3(0.4,0.8,1.);
    
    pos +=dir*stepL;
    roP = normalize(pos) ;
    col+=smoothstep(0.999,1.,dot(rd,roP))*vec3(1.,0.6,0.)*0.6;
    
    pos+=dir*stepL*2. ;
    roP = normalize(pos);
    col+=smoothstep(0.993,1.,dot(rd,roP))*vec3(0.4,0.8,1.)*0.3;
    
    //go to other side of the sun
    pos = sundir ;
    pos+=-dir*stepL ;
    roP = normalize(pos);
    col+=smoothstep(0.6,0.4,length(rd-roP)/0.07) *vec3(1.,0.6,0.0)*0.6;
    
    pos+=-dir*stepL*0.25 ;
    roP = normalize(pos);
    col+=0.02/length(rd-roP)*vec3(0.2,0.6,1.)*0.6 ;
    
    pos+=-dir*stepL*0.5;
    roP = normalize(pos);
    col+=0.015/length(rd-roP)*vec3(0.4,0.8,1.)*0.6 ;
    
    pos+=-dir*stepL*0.25 ;
    roP = normalize(pos);
    col+=smoothstep(0.6,0.4,length(rd-roP)/0.1)*vec3(1.,0.6,0.0)*0.4;
    
    pos+=-dir*stepL*0.25 ;
    roP = normalize(pos) ;
    col+=0.007/length(rd-roP)*vec3(0.4,1.,0.8)*0.6;
    
    pos+=-dir*stepL*0.25 ;
    roP = normalize(pos);
    col+=smoothstep(0.5,0.4,length(rd-roP)/0.3)*vec3(0.4,0.8,1.)*0.3 ;
    col-=smoothstep(0.9,.0,length(rd-roP)/0.2)*vec3(0.4,0.8,1.)*0.2 ;
    
    pos+=-dir*stepL ;
    roP = normalize(pos);
    col+=smoothstep(0.6,0.4,length(rd-roP)/0.2)*vec3(1.,0.6,0.0)*0.6;
    
    pos+=-dir*stepL*0.5 ;

    roP = normalize(pos);
    col+=0.003/length(rd-roP)*vec3(0.4,1.,0.8)*0.6;
    
    pos+=-dir*stepL*1.5 ;
    roP = normalize(pos);
    float haloF = smoothstep(0.5,0.49,length(rd-roP)/0.7);
    haloF-=smoothstep(0.5,0.4,length(rd-roP)/0.69);
    col+=max(haloF,0.)*0.2*vec3(0.4,0.8,1.);
    
    //post processing
    col=clamp(col, 0.,1.);
    col=pow(col,vec3(0.8)) ; //gamma correction
    
    // Output to screen
    gl_FragColor = vec4(col,1.0);
}

