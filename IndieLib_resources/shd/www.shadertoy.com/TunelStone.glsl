void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 p = (-iResolution.xy + 2.0*fragCoord)/iResolution.y;

    float a = atan(p.y,p.x);
    vec2 t = abs(p);
    float r = max(t.x, t.y);
    vec2 uv = vec2( 0.3/r + 0.2*iTime, a/3.1415927 );

    vec2 uv2 = vec2( uv.x+0.5, atan(p.y,abs(p.x))/3.1415927-0.5 );
    vec3 col = textureGrad( iChannel0, uv, dFdx(uv2), dFdy(uv2) ).xyz;
    
    col = col*r;
    
    
    fragColor = vec4( col, 1.0 );
}
