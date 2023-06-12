#define texture(a1,a2) texture2D(a1,a2)
#define PI 3.14159265358979323844

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float scale = 0.85;
    vec2 res = iResolution.xy;
    vec2 xy = (2. * fragCoord.xy - res ) / res.y / scale;
    
    float r = sqrt(dot(xy,xy));
    if( r > 1.0 )
    {
    	fragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
        return;
    }
    
    vec3 p = vec3( xy, sqrt( 1.0 - r*r ));
    float angle = iTime * 0.5;
    vec3 axis = vec3(0.2, 1, 0.6);
    
    // rotational matrix
    float ca = cos(angle);
    float sa = sin(angle);
    axis = normalize(axis);
    
    mat3 rot = mat3(
    ca + axis.x*axis.x*(1.-ca), axis.x*axis.y*(1.-ca) - axis.z*sa, axis.x*axis.z*(1.-ca) + axis.y*sa,
    axis.y*axis.x*(1.-ca) + axis.z*sa, ca + axis.y*axis.y*(1.-ca), axis.y*axis.z*(1.-ca) - axis.x*sa,
    axis.z*axis.x*(1.-ca) - axis.y*sa, axis.y*axis.z*(1.-ca) + axis.x*sa, ca + axis.z*axis.z*(1.-ca));
	
    vec3 rp = rot * p;

    float u = 0.5 + atan(rp.z,rp.x) / PI;
    float v = 0.5 - asin(rp.y) / PI;
    fragColor = texture( iChannel0, vec2( u, v ));

    // directional light
    vec3 lightDir = normalize( vec3( -1.1, -0.3, 1.0 )); 
    float mflight = max(dot( p, lightDir), 0.0);
    fragColor.rgb *= mflight;
    

    // point light
    vec3 light1pos = vec3( 0.6, 0.5, 0.6 ); 
    vec3 light1col = vec3( 1.0, 2.0, 3.0 ); 
    float dis = 1.0 - length( p - light1pos ) / 2.0;
    fragColor.rgb += light1col * pow( dis, 8.0);
}

