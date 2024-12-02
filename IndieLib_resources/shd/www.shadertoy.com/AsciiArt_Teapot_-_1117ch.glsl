/* Thanks Coyote !!! */                                  #define A(i)vec2(4*(T[i]&63)-120,3*(T[i]>>6)-40)-q
/*Created by Sebastien Durand-2014*/                     #define G(a)for(int i=0;i<a;i++)
/*License Creative Commons        */                      #define U(a,b)(a.x*b.y-b.x*a.y)
/*Attribution - Non Commercial    */                                  #define N normalize
/*ShareAlike3.0 UnportedLicense.  */                                       #define W vec3

                                    float K,a,t
                                      ,z=0.;W
                                        P,O; 
                                      int[] T=
                              int[](30,46,110,242,690,
                         1074,1966,2029, 1964,2092,2209,2270,              2466,2595
                       ,2590,1743,1793,1345,896, 652,2107,1976            , 1591,
         1077,944);float B(int i){ vec2 q=P.xy,m=A(i),n=A(i+1),          o=A(i+
       2);float x=.5*U(o,m),y=U(n,o),z=U(m,n);q=m*(x+y)+n*(z-y)-        o*(x+z
      );q.x          =-q.x;m-=n+=n-o;K=clamp(.5*U((o+(y*z-x*x)*q       .yx/dot
      (q,q          )),m)/(x+x+y+z),0.,1.);return sqrt(dot(q=o+K*     (n-o+K
      *m),q        )+P*P).z;}float M(W p){P=p+O;a=min(min(B(15),B(   17))-6.
       ,max(       P.y-50.,min(abs(B(20)-7.)-1., B(22)*(.25+.75*K)-8.)));P.
        xz/=N     (P.xz); P.z = 0.; G(7) a = min ( a, (B( i+i ) -2.)* .7);
         return   a;}void mainImage(out vec4 o,vec2 p){vec2 r=iResolution
           .xy;o= o-o+.8-.3*length(p=(p+p-r)/r.y);W Y=W(0,1,0),u=cross(O
             =N(W(cos(t=iTime), .6-iMouse.y/r.y, sin(t))),Y),d=N(p.x*u+
                 p.y*cross(u,O)-O-O),E=Y/1e3, L=N(3.*Y+O);O*=3e2;G(99
                   &&z<5e2)z+=t=M(d*z);if(t<E.y){O+=z*d; u=N(W(M(E.
                    yxx),M(E),M(E.xxy))-M(E-E));z=.3;G(30)z=min(z
                     ,M(L*(t+=2.))/t);o=mix(o,.4*(Y*max(z+z,0.)
                         +Y).grbb+pow(max (dot(reflect(L,u),d
                              ),0.),1e3),-dot(u,d));}}
