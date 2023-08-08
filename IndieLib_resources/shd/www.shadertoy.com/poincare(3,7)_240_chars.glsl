// 2023 -8 -8
// 庞加莱圆盘
// poincaredisk(3, 7) 240 chars



// thank s to FabriceNeyret2
// modify 
//

void main()
{
    vec2 R = iResolution.xy,

         u = ( xlv_TEXCOORD0.xy+xlv_TEXCOORD0.xy - R ) / R.y,

         p; p.x = 3.63;         // 3.625845        

    float r = 3.49, m = 9., d, l;  // 3.485219

    

    vec4 O = vec4(.2); 

    

    for (int j; j++ < 99; )    

            p *= mat2(-.5, -.866, .866, -.5),  // rot 2 * PI / 3

            d = length(p - u),
            
            l = .01*exp(length(u)),

            m = d < m 

                  ? R = p ,d   // m = min(m,d), save R=p

                  : j % 4 < 1      // every 4 steps:

                      ? u -= abs(m-r) < l    // dist to p ~ radius: draw, reset u

                                ? O += .9*smoothstep(l,0.,abs(m-r)), u 

                                : R,           // else translate to saved p(m)

                        u = u * r * r / dot(u, u) + R, 9.  // inversion and reset m

                      : m;
	gl_FragColor=O;

}

/*

void main()
{

    vec2 p,R = iResolution.xy,u = (xlv_TEXCOORD0.xy + xlv_TEXCOORD0.xy -R) / R.y;

    p.x = 3.63;       // 3.625845        

    float r = 3.49,m = 9.,d;    // 3.485219
    
   vec4 O = vec4(.2);//// 232 chars 
    
    for(int j;j++ < 99;){    

            p *= mat2(-.5, -.866, .866, -.5),  //2 * PI / 3
            
            m = (d = length(p - u)) < m ? R = p ,d : 
        
        j % 5 < 1 ?
        
        u -=  abs(m-r) < .01 ? O += .9, u : R,
        
        u = u * r * r / dot(u, u) + R, 9. : m;
    }
	gl_FragColor=O;
}



*/

