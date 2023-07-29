

// 2023-7-24 22:45
// thanks to FabriceNeyret2
// Newton Flower
// z = z - (z ^ 3 - 1)/(3 * z ^ 2)
// 320 chars
void main( ){

    float a,r,s,i;

    vec2  g,h,q, R = iResolution.xy,

          v = 2.1*(xlv_TEXCOORD0+xlv_TEXCOORD0-R)/R.y + vec2(5,7.7);

    

    for (;i++<12.;) 

        a = atan(v.y, v.x) + .7,

        r = length(v)* .5,

        s = .1 / r / r,  

        g = q, q = v,  // v, q, g = memory of v over 3 steps

        v =  vec2(r * cos(a) - s * cos(a + a),  

                  r * sin(a) + s * sin(a + a)),

        i<2. ? h=v : h;

    g = abs(q - g);

    h = abs(v - h);

    s = g.x * g.y;  

    gl_FragColor = log(1.+vec4(s, h.x*h.y, s*s, 0 )) * vec4(4,.2,.1,1); gl_FragColor.r -= 2.8;//O-=.1;

}



/*

//2023-7-23 13:55     332 chars
void main( ){

    float a,r,s;

    vec2 g,h,q, R=iResolution.xy,

    v=4.2*(xlv_TEXCOORD0-.5*R)/R.y + vec2(5.,7.7);
    for (int i;i<12;i++) 

    {  

        a = atan(v.y, v.x) + .7
        ;
        //a += .01 * sin(iTime); //摇动                                         //

        r = length(v)* .5;

        s = .1 / r / r;   

        g = q; q = v; v =

        vec2(r * cos(a) - s * cos(a + a),  

             r * sin(a) + s * sin(a + a)); 

        if(i<1) h=v;

    }

    g = abs(q - g);h = abs(v - h);

                      s = g.x * g.y;
                      
    gl_FragColor =vec4(4. * log(1. + s) - 2.8      ,
    
            .2 * log(1. + h.x * h.y)     , 

            .1 * log(1. + s * s) ,          

            1.);

}

*/

