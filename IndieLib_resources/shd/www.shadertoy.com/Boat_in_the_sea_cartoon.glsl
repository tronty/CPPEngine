float random(vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}
bool DrawCube(vec2 uv,vec2 pos, vec2 size)
{
    float xyRatio = iResolution.y/iResolution.x;
    if(uv.x>pos.x-size.x && uv.x<pos.x+size.x
    && uv.y*xyRatio>pos.y*xyRatio-size.y && uv.y*xyRatio<pos.y*xyRatio+size.y)
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool DrawCircle(vec2 uv,vec2 pos, float radius)
{
    radius = iResolution.x * radius;
    pos = vec2(pos.x*iResolution.x,pos.y*iResolution.y);
    uv= vec2(uv.x*iResolution.x,uv.y*iResolution.y);
    return distance(pos,uv)<radius;
}
bool DrawWave(vec2 uv,float frequency, float size, float height, float timeDilation)
{
    float pos = (sin(frequency*iTime+timeDilation+uv.x*3.0)*size)*0.5;
    
    pos = height + pos;
    if(uv.y<pos)
    {
        return true;
    }
    else
    {
        return false;
    }
}
void main( )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = xlv_TEXCOORD0/iResolution.xy;
    float fs=sin(iTime-uv.y*4.0)*5.0;
    float r= (fs+10.0)/200.0;
    
    gl_FragColor = vec4(0.2*abs(sin(iTime/3.0)),0.85*abs(sin(iTime/3.0)),0.4+0.4*abs(sin(iTime/3.0)),1);
    
    //Sun
    if(DrawCircle(uv,vec2(1.2-abs(sin(iTime/3.0))/4.0,0.95), 0.1))
    {
        gl_FragColor = vec4(1,0.6+abs(sin(iTime/3.0))*0.2,0,1);
    }
    //Moon
    if(DrawCircle(uv,vec2(-0.2+abs(sin((iTime-4.25)/3.0))/4.0,0.95), 0.1))
    {
        gl_FragColor = vec4(1,1,1,1);
    }
    //Island
    float deltaPosIsland = fract(-iTime/30.0)*1.5-0.3;
    //Trees
    if(DrawCube(uv,vec2(deltaPosIsland*2.0-0.05,0.35), vec2(0.015,0.05)))
    {
        gl_FragColor = vec4(0.56,0.23,0.0,1);
    }
    if(DrawCircle(uv,vec2(deltaPosIsland*2.0-0.05,0.45), 0.05))
    {
        gl_FragColor = vec4(0,0.5+ sin((iTime+0.75)/3.0)*0.1,0,1);
    }
    
    if(DrawCube(uv,vec2(deltaPosIsland*2.0+0.05,0.3), vec2(0.015,0.03)))
    {
        gl_FragColor = vec4(0.56,0.23,0.0,1);
    }
    if(DrawCircle(uv,vec2(deltaPosIsland*2.0+0.05,0.4), 0.033))
    {
        gl_FragColor = vec4(0,0.5+ sin((iTime+0.5)/3.0)*0.1,0,1);
    }
    
    if(DrawCube(uv,vec2(deltaPosIsland*2.0+0.125,0.35), vec2(0.015,0.055)))
    {
        gl_FragColor = vec4(0.56,0.23,0.0,1);
    }
    if(DrawCircle(uv,vec2(deltaPosIsland*2.0+0.125,0.5), 0.05))
    {
        gl_FragColor = vec4(0,0.5+ sin((iTime+0.0)/0.25)*0.1,0,1);
    }
    //Sand
    if(DrawCircle(vec2(uv.x/2.0,uv.y),vec2(deltaPosIsland,0.1), 0.11))
    {
        gl_FragColor = vec4(0.8+ 0.1*random(uv),0.8+0.1*random(uv),0.4+0.1*random(uv),1);
    }
    //WaveBack
    if(DrawWave(uv,2.0, 0.1, 0.2,0.0))
    {
        gl_FragColor = vec4(0.1,0.45+abs(sin((iTime)/3.0))*0.2,0.9,1);
    }
    //Wind
    if(DrawCube(uv,vec2(fract(-iTime)*1.5-0.3,0.35), vec2(0.1,0.001+r*0.001)))
    {
        gl_FragColor = vec4(1,1,1,1);
    }
    if(DrawCube(uv,vec2(fract(-iTime+0.3)*1.5-0.3,0.55), vec2(0.1,0.001+r*0.001)))
    {
        gl_FragColor = vec4(1,1,1,1);
    }
    if(DrawCube(uv,vec2(fract(-iTime+1.2)*1.5-0.3,0.75), vec2(0.1,0.001+r*0.001)))
    {
        gl_FragColor = vec4(1,1,1,1);
    }
    if(DrawCube(uv,vec2(fract(-iTime+0.8)*1.5-0.3,0.85), vec2(0.1,0.001+r*0.001)))
    {
        gl_FragColor = vec4(1,1,1,1);
    }
    //Barco
    float boatXDilation = sin(iTime*0.5)*0.01;
    float boatYDilation = sin(iTime*1.25)*0.05;
    //Mastro
    if(DrawCube(uv,vec2(0.4+boatXDilation,0.35+boatYDilation), vec2(0.02,0.15)))
    {
        gl_FragColor = vec4(0.61,0.28,0.04,1);
    }
    //Bandeira
    if(DrawCube(uv,vec2(0.35+boatXDilation+r,0.45+boatYDilation), vec2(0.15,0.1)))
    {
        gl_FragColor = vec4(1);
    }
    //Boat body
    if(DrawCircle(vec2(uv.x/2.0,uv.y),vec2(0.2 + boatXDilation,0.2 + boatYDilation), 0.11))
    {
        if(!DrawCircle(vec2(uv.x/2.0,uv.y),vec2(0.2 + boatXDilation,0.275 + boatYDilation), 0.105))
        {
            gl_FragColor = vec4(0.66,0.33,0.09,1);
        }
    }
    if(DrawCircle(vec2(uv.x/2.0,uv.y),vec2(0.2 + boatXDilation,0.2 + boatYDilation), 0.1))
    {
        if(uv.y < 0.2 + boatYDilation)
        {
            gl_FragColor = vec4(0.66,0.33,0.09,1);
            for(float i=0.0;i<4.0;i++)
            {
                if(DrawCube(uv,vec2(0.275+i*0.08+boatXDilation,0.125+boatYDilation), vec2(0.02,0.02)))
                {
                    gl_FragColor = vec4(0.56,0.23,0.0,1);
                    if(DrawCube(uv,vec2(0.275+i*0.08+boatXDilation,0.125+boatYDilation), vec2(0.015,0.015)))
                    {
                        gl_FragColor = vec4(0.76,0.43,0.19,1);
                    }
                }  
            }
        }
    }
    float fsb=sin(iTime-uv.y*1.0)*1.5;
    float rb= (fsb+10.0)/200.0;
    for(float i=0.0;i<4.0;i++)
    {
        if(DrawCube(uv,vec2(0.23+i*0.08+boatXDilation+rb,0.05+boatYDilation), vec2(0.01,0.05)))
        {
            gl_FragColor = vec4(0.56,0.23,0.0,1);
        }
    }
    //WaveFront
    if(DrawWave(uv,2.0, 0.05, 0.1,2.0))
    {
        gl_FragColor = vec4(0.1,0.5+abs(sin((iTime+10.0)/3.0))*0.2,1,1);
    }
}

