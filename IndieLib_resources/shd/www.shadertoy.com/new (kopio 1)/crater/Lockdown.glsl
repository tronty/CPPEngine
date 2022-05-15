// Author: 		ocb
// Title: 		Lockdown

// Ray-marching + FBM is twice a slow process.
// Moreover, computing hard shadows needs the same process again (twice slow again)
// To maintain acceptable speed perfos:
// 1- The crater shape function is ray-marched first (with no fbm)
// 	  and when close to the final surface the FBM is included in the ray-march process.
// 2- The hard shadows are pre-computed in buffer A.
//    Using the 4 channels (4 squared sectors around origin) to increase resolution.
// 	  Hard shadows are just added at the end of the color process.
//    A trick is used to detect screen size (resolution) changes.
//
// More comments are included in Common to manage some parameters.



/*****************************    Image   *****************************/

vec3 skyCol(vec3 ray){
    float lit = dot(ray,LIGHT);
    return vec3(1,.5,0)*(.6-max(0.,ray.y))*(.7+.3*lit) + .002/(1.002-lit);
}
  
/************************************************************************/
/*							Ground fct									*/
/************************************************************************/

// mapping global crater shape
float shape(vec2 g){
	float a = .000001*(g.x*g.x + g.y*g.y);
    float h = smin(a*a,abs(1./a),.2)+.8/(50.*a+1.);
    return h;
}

// mapping accurate ground, adding shape + fbm + texture
float ground(vec2 g, float t){
	float len = max(abs(g.x),abs(g.y));
    if(len>5000.) return .5; 
    float h = shape(g);
    float m = fbm(vec3(g.x,maxHill*h,g.y));
    m *= 1.-linstep(3000.,5000.,len);
    
    // scaling texture
    float tex,trans,dump;
    if(t>300.){
    	float dump = 1.-linstep(300.,2000.,t);
        tex = dump*1.5*texture(iChannel0,.00125*g.xy+.5).r;
    }
    else if(t>100.){
    	trans = linstep(100.,300.,t);
        tex = trans*1.5*texture(iChannel0,.00125*g.xy+.5).r+(1.-trans)*.3*texture(iChannel0,.01*g.xy+.5).r;
    }
    else if(t>50.){
        tex = .3*texture(iChannel0,.01*g.xy+.5).r;
    }
    else{
    	trans = linstep(20.,50.,t);
        tex = trans*.3*texture(iChannel0,.01*g.xy+.5).r+(1.-trans)*.05*texture(iChannel0,.05*g.xy+.5).r;
    }
        
    return maxHill*(.4*h*m+.2*m+.4*h)+ tex;
}


vec3 getGndNormal(vec3 p, float t){
    vec2 e = vec2(.1-.075*step(t,50.),0.);	// step() to adjust accuracy to the scale
	float dx = ground(p.xz+e.xy,t) - p.y;
    float dy = ground(p.xz+e.yx,t) - p.y;
    return normalize(vec3(dx,-e.x,dy));    
}


// Fast marching to the global shape
float traceShape(in vec3 pos, in vec3 ray){
    float t = 0., dh;
    vec3 p = pos;
    for(int i=0;i<60;i++){
        dh = p.y-maxHill*shape(p.xz)*1.15;
        if(dh<10.)break;
        t += dh;
        p = pos+t*ray;
        if(p.y > 1.5*maxHill && ray.y>=0.){
            t = INFINI;
            break;
        }
    }
    return t;
}

// Marching:
// step 1 = fast marching to the shape
// step 2 = fine tunning on the ground
float traceGround(in vec3 pos, in vec3 ray){
    float t = 0.;

    float dh = pos.y-ground(pos.xz,t);
    if(dh<.1) return .0001;
    
    if(pos.y > maxHill*shape(pos.xz)*1.15) t = traceShape(pos, ray);    
    if(t == INFINI) return INFINI;
    
    vec3 p = pos + t*ray;
    for(int i=0; i<70;i++){
        dh = p.y-ground(p.xz,t);
        if(abs(dh)<.1-.075*step(t,50.)) break;
        t += .9*dh;
        p = pos+t*ray;
        if(p.y > 1.5*maxHill && ray.y>=0.){
            t = INFINI;
            break;
        }
    }
    return t;
}



vec3 PaintCrater(vec3 p, vec3 ray, float tg, vec3 norm){
    
    // scaling texture
    float trans;
    vec3 tex;
    if(max(abs(p.x),abs(p.z))>3000.){
        tex = texture(iChannel0,.0000625*p.xz+.5).rgb;
    }
 	else if(tg>100.){
    	trans = linstep(100.,300.,tg);
        tex = trans*texture(iChannel0,.00125*p.xz+.5).rgb+(1.-trans)*texture(iChannel0,.01*p.xz+.5).rgb;
    }
    else if(tg>50.){
        tex = texture(iChannel0,.01*p.xz+.5).rgb;
    }
    else{
    	trans = linstep(20.,50.,tg);
        tex = trans*texture(iChannel0,.01*p.xz+.5).rgb+(1.-trans)*texture(iChannel0,.05*p.xz+.5).rgb;
    }
    
    // setting color and shade
    vec3 col = vec3(.5,.25,.0)*(.8+max(0.,p.y)/maxHill)+ tex;
    col *= dot(-norm,LIGHT);

    // fog
    float tt = 3000./tg;
    col *= min(1.,tt);
    col = mix(skyCol(ray), col, min(.7,tt));

    return col;
}

float groundShadows(vec2 p){
    float txg;
    vec2 e = vec2(.001,0.), f = vec2(-.001,.001);
    vec2 g = abs(p.xy)/2000.;
    if(sign(p.x)>0.){
        if(sign(p.y)>0.){
            txg = texture(iChannel1,g+e.xx).x	// Simple triangular interpolation
                + texture(iChannel1,g+f.xy).x	// is enough and reduce workload
                + texture(iChannel1,g-e.yx).x
                - 3.*iResolution.x;				// removing resolution info (stored in buffer A)
        }
        else{
            txg = texture(iChannel1,g+e.xx).y
                + texture(iChannel1,g+f.xy).y
                + texture(iChannel1,g-e.yx).y;
        }
    }
    else{
        if(sign(p.y)>0.){
            txg = texture(iChannel1,g+e.xx).w
                + texture(iChannel1,g+f.xy).w
                + texture(iChannel1,g-e.yx).w;
        }
        else{
            txg = texture(iChannel1,g+e.xx).z
                + texture(iChannel1,g+f.xy).z
                + texture(iChannel1,g-e.yx).z;
        }
    }
    return txg *= .333333;
}

/************************************************************************/
/*							Dome fct									*/
/************************************************************************/

// coord. change
vec2 polar(vec3 p){ return vec2(atan(p.z/p.x)+PI*step(p.x,0.),atan(length(p.xz)/p.y)); }

vec3 cartesian(vec2 i){float s = sin(i.y); return vec3(s*cos(i.x),cos(i.y),s*sin(i.x));}

// Raycasting
vec2 sphere(in vec3 p, in vec3 O, in float r, in vec3 ray){
    vec2 t = vec2(INFINI);
    vec3 d = O - p;
    float b = dot(d, ray);
    
    if (b >= 0.){
        float c = dot(d,d) - r*r;
    	float D = b*b - c;
    	if (D >= 0.){
        	float VD = sqrt(D);
            t.x= b - VD;
            t.y= b + VD;
        }
    }
    return t;
}

// Black structure on sphere, depending on scale
float structure(vec3 p, float scale){
    vec2 a = polar(p)/PIdiv2;
    a.y *= 3.;
    float iy = floor(a.y);
    a.x *= 2.*scale;
    a.x += 1./pow(scale,scale);
    a.x += mod(iy,2.)*.5;
    a = fract(a)-vec2(.5,0.);
    float d = 2.*abs(a.x);
    /*if(iy<scale-6.) d = 1.;
    else*/ if(iy<scale-5.) d -= smoothstep(.0,.1,a.y)-.05;
    else d -= smoothstep(-0.12,.5,a.y)-.05;

    return d;
}

// Black structure on sphere.
// Call twice at 2 differents scales (thick and thin) 
float frame(vec3 p){
        
    float d = structure(p,2.);
    float c = smoothstep(-.04,-.02,-abs(d));

    d = structure(p,4.);
    c = max(c,smoothstep(-.03,-.01,-abs(d)));
    
    return c;
}

vec3 DrawStruct(vec3 p, vec3 ray, vec3 C){
	vec3 col = vec3(.0);
    p -= C;		// origin of the coord set on the center of sphere
    
    col -= frame(p);	// blacken the frame
    
    vec2 tile = WINTILE/PIdiv2;		// number of window glass
    vec2 a = tile*polar(p);			// tiling polar coordinates
    vec2 i = floor(a.xy)/tile;
    vec2 f = abs(fract(a.xy)-.5);
    
    float d = smoothstep(.48,.5,max(f.x,f.y));			// draw thin contour of each window
    col -= d;
        
    vec3 norm = cartesian(i);		// back to cartesian. Same normal for the whole window (i = floor)
    vec3 refl = reflect(ray,norm);	
    
    float spec = .2/(1.1-dot(LIGHT,refl));		// calculating sky reflection
    col += spec*skyCol(refl);					// and specular light
    col += spec*d;								// sky reflection is used on the whole surface to set the golden color of the dome
    
    p += C;
    float t = traceGround(p,refl);				// Adding ground reflection using the
    if(t<INFINI){								// window normal
        p += t*refl;								
        p.y = ground(p.xz,t);								
    	vec3 n = getGndNormal(p,t);
    	col += .5*PaintCrater(p,refl,t,n)*groundShadows(p.xz);
    }
    else col += skyCol(refl);
    
    return col;
}


/************************************************************************/
/*							Camera										*/
/************************************************************************/


vec3 getCamPos(in vec3 camTarget){
    
 #ifdef FREE_CAM
    float 	rau = RAU,
            alpha = iMouse.x/iResolution.x*4.*PI,
            theta = (iMouse.y+.001)/iResolution.y*PI+ (PI/2.0001);	// +0.001 to avoid black horizontal line
 #else
    float ti = TIME_SET;
    float cs = (1.-cos(ti*.09-PI));
    //float s = smoothstep(25.,120.,abs(ti-60.));
    float rau = 25. + 2500.*smoothstep(25.,120.,abs(ti-90.))*(1.-smoothstep(180.,280.,ti)),
    	  alpha = ti*.12-1.,
    	  theta = .1*cs*cs*cs+.05;
 #endif
            
    return rau*vec3(-cos(theta)*sin(alpha),sin(theta),cos(theta)*cos(alpha))+camTarget;
}

vec3 getRay(in vec2 st, in vec3 pos, in vec3 camTarget){
    float 	focal = 1.;
    vec3 ww = normalize( camTarget - pos);
    vec3 uu = normalize( cross(ww,vec3(0.0,1.0,0.0)) ) ;
    vec3 vv = cross(uu,ww);
	return normalize( st.x*uu + st.y*vv + focal*ww );
}


/************************************************************************/
/*								Main									*/
/************************************************************************/

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 st = (fragCoord.xy-.5*iResolution.xy)/iResolution.y;
    
    vec3 C = vec3(1.);
    C.y = ground(C.xz,1.);
    
    //vec3 camTarget = TARGET;
    vec3 camTarget = C + vec3(0.,3.,0.);
    vec3 pos = getCamPos(camTarget);
    pos.y = max(pos.y, 1.05*ground(pos.xz,1.));
    vec3 ray = getRay(st, pos,camTarget);
		
    vec3 col = vec3(.0);
    vec3 coldom = vec3(.0);
    vec3 norm;
    
    float tg = INFINI;
    vec2 ts;
        
    tg = traceGround(pos,ray);
    
    if(tg<INFINI){						// Ground is touched
        
        vec3 p = pos + tg*ray;
        p.y = ground(p.xz,tg);			// recalculating p.y (vertical) to be more accurate on position
										// needed due to the fine details
        norm = getGndNormal(p,tg);

        col += PaintCrater(p,ray,tg, norm);
        
        float txg=1., txs=1.;							// finally, adding shadows pre-calculated on the buffer A
        if(max(abs(p.x),abs(p.z))<2000.){				 
            
            txg = groundShadows(p.xz);
            
            vec2 tss = sphere(p+.2*norm,C,R,LIGHT);		// And finally to finally, adding the dome
            if(tss.x<INFINI){							// shadow, only the frame, as windows are 
                p += tss.x*LIGHT-C;						// supposed to be partially transparent.
                txs = .1+.8*dot(normalize(p),-LIGHT);	// dot() is to simulate more thickness on the edges
                txs *= .6-.6*frame(p);
                //txs *= 1.-.8*frame(p+(tss.y-tss.x)*LIGHT);
            }
            
            col *= min(txs,txg);

    	}
    }
 
    else col += skyCol(ray);						// no hit = sky

    
    ts = sphere(pos,C,R,ray);
    
	if(ts.x < tg){						// Dome is touched
        
        vec3 p = pos + ts.x*ray;
        coldom += DrawStruct(p,ray,C);	// draw frame and color
        
        if(ts.y<tg){					// draw the other side of the dome
            p = pos + ts.y*ray;			// visible by transparency
            p-=C;
            coldom = mix(coldom,vec3(-frame(p)),.2);
            col*=.1;
        }
        
    if(tg < ts.y) col = vec3(0.);	// center of the dome is black
    col = mix(col,coldom,0.6);		// mixing sphere and color behind for the partial transparency
    }
    
    fragColor = vec4(1.5*col,1.0);
}

