//Compressed version: https://www.shadertoy.com/view/sstXRn

//Voxel map (approximate SDF)
float map(vec3 pos)
{
    return length(mod(pos,18.0)-9.0) - 9.5+sin(pos.z*0.3-iTime*0.1);
}

void main()
{
    vec2 res = iResolution.xy;   //View resolution
    
    vec3 cam = vec3(sin(iTime*0.2+res), iTime),         //Camera position
    pos = cam,                                          //Pixel position
    ray = normalize(vec3(xlv_TEXCOORD0*2.0 - res, res.y)),  //Ray direction
    cell = vec3(0,0,0);                                 //Voxel cell
    
    //Step up to 100 voxels.
    for(int i = 0; i<100; i++)
    {
        //Axis distance to nearest cell (with a small bias).
        vec3 dist = fract(-pos * sign(ray)) + 1e-4,
        //Alternative version (produces artifacts after a while)
        //vec3 dist = 1.0-fract(pos * sign(ray)),
        //Raytraced distance to each axis.
        leng = dist / abs(ray),
        //Nearest axis' raytrace distance (as a vec3).
        near = min(leng.xxx, min(leng.yyy, leng.zzz));
        
        //Step to the nearest voxel cell.
        pos += ray * near;
        //Get the cell position (center of the voxel).
        cell = ceil(pos) - 0.5;
        //Stop if we hit a voxel.
        if (map(cell) < 0.0) break;
    }

    //Rainbow color based off the voxel cell position.
    vec3 color = sin(cell.z+vec3(0,2,4)) * 0.5 + 0.5;
    //Square for gamma encoding.
    color *= color;
    
    //Compute cheap ambient occlusion from the SDF.
    float ao = smoothstep(-1.0, 1.0, map(pos)),
    //Fade out to black using the distance.
    fog = min(1.0, exp(1.0 - length(pos-cam)/8.0));
    
    //Output final color with ao and fog (sqrt for gamma correction).
    gl_FragColor = vec4(sqrt(color * ao * fog),1);
}

