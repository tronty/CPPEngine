uniform sampler2D iChannel0;
#define texture(a1,a2) texture2D(a1,a2)
/*
The MIT License (MIT)

Copyright (c) 2016 Huw Bowles, Daniel Zimmermann, Beibei Wang

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

//
// For volume render shader:
//
// https://www.shadertoy.com/view/Mt3GWs
//
// We are in the process of writing up this technique. The following github repos
// is the home of this research.
//
// https://github.com/huwb/volsample
//

#define PERIOD (30.)
#define SAMPLE_CNT 15
#define RESOLUTIONS 1
#define GRID_CONTRAST 1.5
#define PI 3.141592654
bool STRATIFIED;

float line( vec2 p, vec2 n, bool repeat )
{
    //n /= dot(n,n);
    
    float d = abs(dot(p, n));
    
    if( !repeat )
    {
	    d = d / PERIOD;
	    return smoothstep( 1.5/PERIOD, 0., abs(d) );
    }
    
    float res = 0.;
    for( int i = 0; i < RESOLUTIONS; i++ )
    {
        float period = PERIOD*pow(2.,float(RESOLUTIONS-i-1));
        
        float fr = abs(fract(d/period));
        // fix fract boundary
        fr = min( fr, 1.-fr );
        float col = mix( 0., .3, pow(GRID_CONTRAST,-float(i)) );
        res = max( res, smoothstep( 2./period, 0., fr ) * col );
    }
    return res;
}

float lines( vec2 p )
{
    float l = 0.;
    
    l = max( l, line( p, vec2(1.,0.), true ) );
    
    if( bool( 10 ) )
    {
        l = max( l, line( p, vec2(1.,1.)/sqrt(2.), true ) );
        l = max( l, line( p, vec2(1.,-1.)/sqrt(2.), true ) );
        l = max( l, line( p, vec2(0.,1.), true ) );
    }
    
    return l;
}

void main( )
{
	STRATIFIED = fract(iTime/(4.*PI))<0.5;
    
    vec2 p = xlv_TEXCOORD0.xy - iResolution.xy / 2.;
    
    float l = lines( p );
    
    gl_FragColor = vec4(l);

    gl_FragColor += texture( iChannel0, xlv_TEXCOORD0/iResolution.xy );
}
