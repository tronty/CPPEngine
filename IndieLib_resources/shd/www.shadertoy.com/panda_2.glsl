//

//

#define M  i++;if(s > 0.)c = i;s = fc

#define E  i++;if(s > 0.)c = i;s = ffc



float sml;







void rotate(in float sita,inout vec2 u)

{

	sita = sita * 3.1415926535/180.;
	u *= mat2(cos(sita), -sin(sita), sin(sita), cos(sita));

}



float fc(vec2 u, float ox,float oy, float sc1x,float sc1y, float sita1,float a,float b,

	float sita2,float sc2x,float sc2y,float r1,float r2,float hx,float hy,float fx,float fy)

{

	u -= vec2(ox, oy); 

	u *= vec2(sc1x, sc1y);

	rotate(sita1, u);

	u *= vec2(fx, fy);

	if(dot(u, u) < r1)

	{

		u += vec2(hx, hy) * pow(abs(u), vec2(a, b));

		u *= vec2(sc2x, sc2y);

		rotate(sita2, u);

		u = pow(abs(u), vec2(b, a));   ////////////////////// 反写的

		if(abs(u.x + u.y) < r2) return 1.; // 

	}

	return 0.;

}





float ffc(vec2 u, float ox,float oy, float sc1x,float sc1y, float sita1,  float fx,float fy)

{

	return fc(u, ox, oy, sc1x, sc1y, sita1, 1.6, 1.6, 0., 3.2, 3.,.3, .3, 1., 1., fx, fy);



}











// 

float mth(vec2 u)

{

	u = (u - vec2(-.40, .14)) * 20.;

	rotate(199., u);

	u.x *= 1.5;

	float d =sqrt( u.x * u.x  +pow((u.y + 10.),2.))-10.;

	if(d > 0.)

	{

		if(sml > .5)u.x /= 1.6;

		float r=length(u);

		if(r < 3.)

		{

			if(d < .8 )return 1.;

			else if(r < 2.5 && r > 1.5)return 1.;

		}

	}

	return 0.;

}

float lft(vec2 u)

{

	u -= vec2(-.059, -1.3);

	u *= 32.;

	rotate(328., u); /// 

	if (.4 * dot(u, u)*dot(u, u) -8. * u.x * u.x * u.x - 9. * u.y * u.y * u.y < 5.) return 1.;

	return 0.;

}







float bmb(vec2 u ,int i)

{

	float e = -.84, d, r;

	if(u.x > .4 || u.y < -1.25)return 0.;

	if(i == 0)

	{

		u -= vec2(.46, .3);

		rotate( 135., u);

		u *= vec2(.5, 1.1);

	}else{

		u -= vec2(-.74, -1);

		rotate (130., u);

		u.x *= .8;

	}

	u *= 2.5; u.x *= .5;///

	u.y = abs(u.y) + e;



	d = pow(abs(u.y), 5.) + pow(abs(u.x), 5.);

	r = length(u -vec2(0, e));

	if(d > .25){

		if(r < .7) return 1.;

		if(r < .7*1.05) return 2.; // 白

	}

	return 0.;

}











float el(vec2 u ,float ox, float oy, float sc)

{

	u = (u - vec2(ox, oy)) * sc ;

	rotate(303., u);

	if((u.x * u.x / 51.84 + u.y * u.y / 27.04) < .5)return 1.;

	return 0.;

}











// 

float  panda(in vec2 u)

{

	float s,c,i,clr;



	clr = .5; ///

	M(u,   -0.12  , -0.5   ,  0.6   ,  0.6   , 25.    ,  1.47  ,  1.6   ,  0.    ,  1.    ,  1.    ,  0.3   ,  0.23  ,  0.    ,  0.    ,  1.    ,  1.    );//2

	M(u,   -0.12  , -0.5   ,  0.6   ,  0.6   , 25.    ,  1.47  ,  1.6   ,  0.    ,  1.    ,  1.    ,  0.3   ,  0.2   ,  0.    ,  0.    ,  1.    ,  1.    );//3 w

	M(u,   -0.83  , -0.33  ,  0.8   ,  0.8   , 129.   ,  2.    ,  3.23  , 41.8   ,  3.    ,  3.    ,  0.3   ,  0.3   ,  1.    ,  1.    ,  1.    ,  1.    );//4

	M(u,    0.23  ,  0.03  ,  0.8   ,  0.8   , 307.   ,  2.    ,  3.23  , 560.   ,  3.    ,  3.    ,  0.3   ,  0.3   ,  1.    ,  1.    ,  1.    , -1.14  );//5

	M(u,   -0.64  ,  1.098 ,  1.3   ,  1.3   , 28.    ,  2.18  ,  2.05  ,  0.    ,  1.    ,  1.    ,  0.3   ,  0.1   ,  0.    ,  0.    ,  1.    ,  1.    );//6

	M(u,    0.15  ,  0.78  ,  1.2   ,  1.2   , 572.   ,  3.2   ,  2.7   ,  0.    ,  2.2   ,  2.2   ,  0.3   ,  0.3   ,  1.    ,  1.    ,  1.    ,  1.    );//7

	M(u,   -0.35  ,  0.48  ,  0.6   ,  0.6   , 28.    ,  2.08  ,  2.05  ,  0.    ,  1.    ,  1.    ,  0.3   ,  0.12  ,  0.    ,  0.    ,  1.    ,  1.    );//8 

	M(u,   -0.35  ,  0.48  ,  0.6   ,  0.6   , 28.    ,  2.08  ,  2.05  ,  0.    ,  1.    ,  1.    ,  0.3   ,  0.1   ,  0.    ,  0.    ,  1.    ,  1.    );//9 w

	M(u,   -0.6   ,  0.4   ,  2.5   ,  2.5   , 144.6  ,  2.77  ,  3.94  , 341.   ,  2.2   ,  2.2   ,  0.3   ,  0.3   ,  1.    ,  1.    ,  1.    ,  1.    );//10

	M(u,   -0.125 ,  0.3   ,  2.5   ,  2.5   , 395.   ,  2.7   ,  3.5   , 58.    ,  2.2   ,  2.2   ,  0.3   ,  0.3   ,  1.    ,  1.    ,  1.    ,  1.    );//11

	M(u,   -0.375 ,  0.27  ,  5.5   ,  5.5   , 572.   ,  3.9   ,  3.5   ,  0.    ,  2.2   ,  2.2   ,  0.3   ,  0.3   ,  1.    ,  1.    ,  1.    ,  1.    );//12

E(u,   -0.2   , -0.472 ,  8.    ,  8.    , 222.   ,  0.3   , -1.    );//13

	E(u,    0.494 ,  0.277 ,  4.    ,  4.    , 262.   ,  0.15  , -1.    );//14

	E(u,    0.55  ,  0.161 ,  4.    ,  4.    , 233.   ,  0.15  ,  1.    );//15

	E(u,    0.631 ,  0.281 ,  4.    ,  4.    , 255.   ,  0.15  ,  1.    );//16

	E(u,    0.744 ,  0.267 ,  4.    ,  4.    , 190.   ,  0.15  ,  1.    );//17

	E(u,    0.844 ,  0.507 ,  3.    ,  3.    , 175.   ,  0.15  ,  1.    );//18

	E(u,    1.164 ,  0.477 ,  4.5   ,  4.5   , 245.   ,  0.15  ,  1.    );//19

	E(u,    1.014 ,  0.667 ,  5.    ,  5.    , 155.   ,  0.15  , -1.    );//20

	E(u,    0.664 ,  0.647 ,  8.    ,  8.    , 155.   ,  0.3   , -1.    );//21

	E(u,   -1.066 , -0.767 ,  4.    ,  4.    , 335.   ,  0.15  , -1.    );//23

	E(u,   -0.846 , -0.732 ,  5.    ,  5.    , 300.   ,  0.15  , -1.    );//24

	E(u,   -0.776 , -0.682 ,  5.5   ,  5.5   , 200.   ,  0.35  ,  1.    );//25



	if(s > 0.)c = i;



	if (c > 0.)

	{

		if ((c==3.)||(c==9.) )

			clr = 1.;

		else

			clr = 0.;

	}





        s = bmb(u, 0);if(s > 0.)clr=(s == 1. ? 0. : 1.); 

        s = bmb(u, 1);if(s > 0.)clr=(s == 1. ? 0. : 1.);    



        s = el(u, -0.525, 0.44, 88.);if(s > 0.)clr = 1.;//eye w

        s = el(u, -0.175, 0.34, 88.);if(s > 0.)clr = 1.;

        s = el(u, 0.47, -0.52, 15.);if(s > 0. )clr = 0.; //h rt foot 

        s = el(u, 0.57, -0.4, 88.) ;if(s > 0. )clr = 1.; 

        s = el(u, 0.45, -0.37, 88.);if(s > 0. )clr = 1.;

        s = el(u, 0.61, -0.54, 88.);if(s > 0. )clr = 1.;

	s = lft(u)  ;if(s > 0.)                clr = 0.;

	s = mth(u)  ;if(s > 0.)                clr = 0.;



	return clr;



}






vec3 poincare(in vec2 u,int nk)

{
    //if(length(u)>1.)return vec3(.5);
	
	float d = 1.73205080756888, r = 1.41421356237309, m, clr, s;

	int i, j;

	vec2 p, q;
    
    
    s= 1. / (.4+.6 * dot(u, u))*.45;

  if(length(u)>1.)return vec3(.5);
   

	p.x = d;
    for(float k;k<12.;k++)
    {
	for(j = 0;j < 32;j++)

	{

		m = d + d;

		for(i = 0;i < 4;i++)

		{

			p.xy = vec2(-p.y, p.x);

			if(length(p - u) < m){m = length(p - u);q = p;}

		}

		if(length(u - q) > r)break;// and 

		u -= q;

		u = u * r * r / dot(u , u) + q;

	}

	if(abs(length(u - q) - r) < .012*pow(2.,k*3. ))clr= 0.25;
    else clr=panda( u * 4.5 - 0.);
    
    if(clr != .5  || j== 0)break;
      
      
      u*=8.;
    
    
    
    
    
    
    
    }
    
  // if(clr== .5)
   clr *= s;
    return vec3(clr);

}








// 

void main( )
{
	vec2 u =xlv_TEXCOORD0.xy;

	vec2 R = iResolution.xy,

	     v = .8 * (u + u -R) / (R.x > R.y ? R.y : R.x),

	     m = .8 * (iMouse.xy * 2. -R) / (R.x > R.y ? R.y : R.x);



	
    vec3 p = poincare( v , 0);

	
   // if(p == .5)p /= (.5 + .5 * dot(v, v))*1.5;

	gl_FragColor = vec4( p,1. );

}

