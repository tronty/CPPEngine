// Created by Arrangemonk 2021, arrangemonk@gmail.com
// where i have a clear source i added shadertoy link


vec3 rampBiome(float x){
    vec3 col = biome[0];
    float step_size = 1./(float(biome.length())-1.);
    for(int i = 0; i < biome.length()-1; i++){
        col =mix(col, biome[i+1], linearstep(float(i)*step_size, (float(i)+1.)*step_size, x));
    }
    return col;
}
 
 vec3 rampSky(float x){
    vec3 col = sky[0];
    float step_size = 1./(float(sky.length())-1.);
    for(int i = 0; i < sky.length()-1; i++){
             col =mix(col, sky[i+1], linearstep(float(i)*step_size, (float(i)+1.)*step_size, x));
    }
    return col;
}

float h(in vec2 uv,bool water)
    {
    uv = uv * 0.005;
    float result = texture(iChannel0,uv).x - texture(iChannel3,uv*45.).x *0.025;
    if(!water)
        result = max(waterline + waterripple(uv*5.,iChannel3,iTime).z *0.03,result);
    return result ;
    }

//https://www.shadertoy.com/view/3sSSW1
vec3 texNormalMap(in vec2 uv,float scale)
{ 
    float p  = h(uv,true);
    float h1 = h(uv + vec2(scale,0),true);
    float v1 = h(uv +  vec2(0,scale),true);
       
   	vec2 normal = (p - vec2(h1, v1));
    
    normal *= normalStrength;
    normal += 0.5;
    
    return normalize(vec3(normal, 1.));
}



vec3 color( in vec2 uv,float height)
{ 
    return mix(rampBiome(height*0.25),mix(fbm(uv,iChannel1,octaves).rgb,rampBiome(height*0.25),0.75),height/6. +0.1);
}

vec3 target(in vec3 origin,in vec3 direction,in float distance)
    {
        return origin + (direction * distance);
    }

//lost link, most likely iqs doing
float RayMarch(vec3 origin,vec3 direction,in bool water, out bool result)
{
    float t = 0.;
    for (int i = 0; i < 72; i++)
    {
        vec3 rp = target(origin,direction,t);
        float h = rp.y - (h(rp.xz,water));
        if (abs(h) < (.0015 * t) || t > DrawDistance) break;
        t += h;
    }
    result = t < DrawDistance;
    return t;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    time = iTime;
    vec2 uv = fragCoord/iResolution.xy;
    vec3 sunDir = normalize(vec3(6.,-3., 2.));
    
    //low resolution 
   //  uv = floor(uv*vec2(320.,240.))/vec2(320.,240.);
    float aspect = iResolution.x / iResolution.y;
    
    vec3 rd = normalize(vec3((1.- 2. * uv.x) * aspect + sin(time * 0.1)*0.2+sin(time *3.)*.01,-1. + 2. * uv.y-0.6+sin(time *3.)*.01, -3.0));
    vec3 ro = vec3(sin(time * 0.1)*3.+sin(time *3.)*.01,5.+sin(time *5.)*.009, -time+sin(time *4.)*.011);

    bool result;
    float distance = RayMarch(ro,rd,false,result);
    float distfract = clamp(distance*DrawDistanceFract,0.,1.);
    
     vec3 c = rampSky((uv.y-0.4)*1.1);
     if(result)
     {
         vec3 point = target(ro,rd,distance);
         float height =  h(point.xz,false);
         if(h(point.xz,true) < height) // why water isnt at 0 u ask? i winged it, thats why
         {
            vec3 waternormal = normalize(waterripple(point.xz * 0.02 + distance * 0.002,iChannel3,iTime) + vec3(0,0,2.+ (distfract * distfract) * 10.));
            vec3 wd = refract(rd,waternormal, .97);
            vec3 wr = normalize(reflect(rd,waternormal));
             float wdistance = RayMarch(point,wd,true,result);
             float wdistfract = clamp(wdistance*DrawDistanceFract,0.,1.);
             vec3 wp = target(point,wd,wdistance);
             float wheight =  h(wp.xz,true)-0.1;
             
             float hilight = clamp((dot(wr,waternormal) * 0.5 + 0.5)* distance *0.035,0.,1.);
             //vec3 skyreflect = rampSky(1.1 -wr.z * 0.6);
             vec3 skyreflect = rampSky(1.1 - texture(iChannel2,-wr).r* 0.6);
             skyreflect = mix(skyreflect,texture(iChannel2,wr).rgb,.5);
             
             float wsun = dot( texNormalMap(wp.xz,1.), sunDir);
             wsun += dot( texNormalMap(wp.xz,0.25), sunDir )*2.;
             wsun += dot( texNormalMap(wp.xz,0.02), sunDir )*4.,
             wsun *= 0.143;
             wsun = clamp( wsun, 0.0, 1.0 )-0.6;
             float cst = caustics(wp.zx * 5. + wp.y,time*2.)*0.4 * clamp(wdistance,0.,1.)*clamp(wp.y/8.,0.,1.);
             
             vec3 c1 = mix(color(wp.xz,mix(3.-wdistfract*10.,wheight,.2)) + wsun,skyreflect,hilight) +cst;
             //vec3 c1 = vec3(cst,cst,cst);
       
             c = mix(c1,c,distfract);
             c = foam(point.xz + waternormal.xy *0.15 - vec2(0.2,0.2)*time,iChannel3,iChannel1,c,wheight);
         }
         else{
             float sun = dot( texNormalMap(point.xz,1.), sunDir);
             sun += dot( texNormalMap(point.xz,0.25), sunDir )*2.;
             sun += dot( texNormalMap(point.xz,0.02), sunDir )*4.,
             sun *= 0.143;
             sun = clamp( sun, 0.0, 1.0 )-0.6;
             c = mix(color(point.xz,height) + sun,c,distfract);
             c += foamborder(point.xz - vec2(0.1,0.1)*time,iChannel3,height*height -5.45);
         }
     }
    fragColor = vec4(c,1.0);

}

