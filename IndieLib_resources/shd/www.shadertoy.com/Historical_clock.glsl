#if 0
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

#define SHADERTOY

#ifdef SHADERTOY
#define resolution iResolution
#define time iTime
#define data iDate
#endif

#ifndef SHADERTOY
uniform vec2 resolution;
uniform float time;
uniform vec4 date;
#define iResolution resolution
#define iTime time
#define iDate date
#endif
#endif

#define PI 3.141592
#define TWOPI 6.28318530717958648
#define HALFPI 1.5707963267948966

vec2 rotateUV(vec2 uv, vec2 pivot, float angle)
{
	float x = cos(angle) * (uv.x - pivot.x) + sin(angle) * (uv.y - pivot.y) + pivot.x;
	float y = cos(angle) * (uv.y - pivot.y) - sin(angle) * (uv.x - pivot.x) + pivot.y;
	return vec2(x,y);
}

// found at https://iquilezles.org/articles/distfunctions2d/
float sdLine(in vec2 p,in vec2 a,in vec2 b,in float w)
{
	vec2 pa = p-a, ba = b-a;
	float h = clamp(dot(pa,ba)/dot(ba,ba),0.0,1.0);
	return length(pa - ba*h)-w;
}

float sdCircle(vec2 uv,vec2 pos,float radius)
{
	return length(uv-pos)-(radius*0.5);
}

float sdRing(vec2 uv,vec2 pos,float radius,float width)
{
	return smoothstep(0.0, width, abs(length(uv-pos) - radius * 0.5));
}

float sdSquare(vec2 uv,vec2 offset,vec2 size)
{
	return max(abs(uv.x - offset.x) - size.x, abs(uv.y - offset.y) - size.y);
}

float sdParallelogram(in vec2 p,float wi,float he,float sk)
{
	vec2 e = vec2(sk,he);
	float e2 = sk*sk + he*he;

	p = (p.y<0.0)?-p:p;

	// horizontal edge
	vec2  w = p - e; w.x -= clamp(w.x,-wi,wi);
	vec2  d = vec2(dot(w,w), -w.y);

	// vertical edge
	float s = p.x*e.y - p.y*e.x;
	p = (s<0.0)?-p:p;
	vec2 v = p - vec2(wi,0);
	v -= e*clamp(dot(v,e)/e2,-1.0,1.0);
	d = min(d, vec2(dot(v,v), wi*he-abs(s)));

	return sqrt(d.x)*sign(-d.y);
}

// found at https://andorsaga.wordpress.com/2018/06/26/sdfs-rendering-a-rectangle/
float sdRect(vec2 st,vec2 pos,vec2 size,float bVal)
{
	return smoothstep(bVal+0.0001,0.0,length(max(abs(st-pos)-size,0.0)));
}

// found at https://www.shadertoy.com/view/7tj3Wz
float sdRhombus(vec2 st,vec2 pos,vec2 size,float bVal)
{
	st -= pos;

	st = abs(st);
	st.x -= size.x;

	float f = clamp( (st.y-st.x)/(size.y+size.x), 0.0, 1.0 );

	vec2 q = abs(st-f*vec2(-size.x,size.y));
	float result = max(q.x,q.y)*((size.y*st.x+size.x*st.y>0.0)?1.0:-1.0);

	return step(result, 0.0);
	//return smoothstep(bVal+0.0001,0.0,result);
}

float romanNumerals(vec2 uv,vec2 pos,vec2 size,float angle,int num)
{
	float n = 1.0;

	uv -= pos;
	uv /= size;

	uv = rotateUV(uv,vec2(0.0),-HALFPI+angle);

	if(num==1)
	{
		n = sdSquare(uv,vec2(0.0),vec2(0.35,2.0));
	}
	else if(num==2)
	{
		n = sdSquare(uv+vec2(0.6,0.0),vec2(0.0),vec2(0.35,2.0));
		n = min(n,sdSquare(uv-vec2(0.6,0.0),vec2(0.0),vec2(0.35,2.0)));
	}
	else if(num==3)
	{
		n = sdSquare(uv+vec2(1.2,0.0),vec2(0.0),vec2(0.35,2.0));
		n = min(n,sdSquare(uv,vec2(0.0),vec2(0.35,2.0)));
		n = min(n,sdSquare(uv-vec2(1.2,0.0),vec2(0.0),vec2(0.35,2.0)));
	}
	else if(num==4)
	{
		n = sdSquare(uv+vec2(1.8,0.0),vec2(0.0),vec2(0.35,2.0));
		n = min(n,sdParallelogram(uv-vec2(1.2,0.0),0.4,2.0,0.6));
		n = min(n,sdParallelogram(uv-vec2(0.0,0.0),0.4,2.0,-0.6));
	}
	else if(num==5)
	{
		n = sdParallelogram(uv-vec2(0.6,0.0),0.4,2.0,0.6);
		n = min(n,sdParallelogram(uv+vec2(0.6,0.0),0.4,2.0,-0.6));
	}
	else if(num==6)
	{
		n = sdSquare(uv-vec2(1.8,0.0),vec2(0.0),vec2(0.35,2.0));
		n = min(n,sdParallelogram(uv+vec2(0.0,0.0),0.4,2.0,0.6));
		n = min(n,sdParallelogram(uv+vec2(1.2,0.0),0.4,2.0,-0.6));
	}
	else if(num==7)
	{
		n = sdSquare(uv-vec2(1.2,0.0),vec2(0.0),vec2(0.35,2.0));
		n = min(n,sdSquare(uv-vec2(2.4,0.0),vec2(0.0),vec2(0.35,2.0)));
		n = min(n,sdParallelogram(uv+vec2(0.6,0.0),0.4,2.0,0.6));
		n = min(n,sdParallelogram(uv+vec2(1.8,0.0),0.4,2.0,-0.6));
	}
	else if(num==8)
	{
		n = sdSquare(uv-vec2(0.6,0.0),vec2(0.0),vec2(0.35,2.0));
		n = min(n,sdSquare(uv-vec2(1.8,0.0),vec2(0.0),vec2(0.35,2.0)));
		n = min(n,sdSquare(uv-vec2(3.0,0.0),vec2(0.0),vec2(0.35,2.0)));
		n = min(n,sdParallelogram(uv+vec2(1.2,0.0),0.4,2.0,0.6));
		n = min(n,sdParallelogram(uv+vec2(2.4,0.0),0.4,2.0,-0.6));
	}
	else if(num==9)
	{
		n = sdSquare(uv+vec2(1.6,0.0),vec2(0.0),vec2(0.35,2.0));
		n = min(n,sdParallelogram(uv-vec2(0.6,0.0),0.4,2.0,1.0));
		n = min(n,sdParallelogram(uv-vec2(0.6,0.0),0.4,2.0,-1.0));
	}
	else if(num==10)
	{
		n = sdParallelogram(uv,0.4,2.0,1.0 );
		n = min(n,sdParallelogram(uv,0.4,2.0,-1.0));
	}
	else if(num==11)
	{
		n = sdSquare(uv-vec2(1.6,0.0),vec2(0.0),vec2(0.35,2.0));
		n = min(n,sdParallelogram(uv+vec2(0.6,0.0),0.4,2.0,1.0));
		n = min(n,sdParallelogram(uv+vec2(0.6,0.0),0.4,2.0,-1.0));
	}
	else if(num==12)
	{
		n = sdSquare(uv-vec2(1.0,0.0),vec2(0.0),vec2(0.35,2.0));
		n = min(n,sdSquare(uv-vec2(2.2,0.0),vec2(0.0),vec2(0.35,2.0)));
		n = min(n,sdParallelogram(uv+vec2(1.2,0.0),0.4,2.0,1.0));
		n = min(n,sdParallelogram(uv+vec2(1.2,0.0),0.4,2.0,-1.0));
	}
	return step(0.1,n);
}

vec4 clockNumbers(vec2 uv)
{
	vec4 layer = vec4(0.0);
	float n = 0.0;

	if(length(uv) < 0.166 && (length(uv)) > 0.123)
	{
		uv = rotateUV(uv,vec2(0.0),-1.82);

		float num = 12.0;
		float angle = (atan(uv.y, uv.x) + PI) / TWOPI * num;
		float id = floor(angle);
		float baseAngle = id / num * TWOPI + PI;

		vec2 ruv = rotateUV(uv, vec2(0.0), (baseAngle + 0.245));

		float angleTmp = HALFPI;
		int numTmp = int(num-id);
		if(numTmp>3 && numTmp<9) angleTmp = -angleTmp;

		ruv = rotateUV(ruv,vec2(0.2),-HALFPI);

		n = romanNumerals(ruv,vec2(0.4,0.144),vec2(0.009), angleTmp, numTmp);

		vec4 numLayer = vec4(vec3(0.0),smoothstep(0.1,0.0,n));

		layer = mix(layer,numLayer,numLayer.a);

		// debug
		/*
		vec4 idLayer = vec4(vec3(id/num),1.0);
		idLayer = pow(idLayer,vec4(0.5));
		layer = mix(idLayer,layer,layer.a);
		*/
	}
	return layer;
}

vec4 clockFace(vec2 uv)
{
	vec4 layer = vec4(0.0);
	float r = 0.408;

	float d = sdCircle(uv,vec2(0.0),r);

	//layer = (d>0.0015) ? vec4(0.0) : vec4(vec3(1.0), smoothstep(0.005, -0.00125, d));
	layer = (d>0.0015) ? vec4(0.0) : vec4(3.0*vec3(r-distance(uv*1.5,vec2(0.0))), smoothstep(0.005, -0.00125, d));
	//layer = (d>0.0015) ? vec4(0.0) : vec4(1.1-vec3((length(uv*4.0)-0.01)), smoothstep(0.005, -0.00125, d));

	return layer;
}

vec4 clockLines(vec2 uv)
{
	vec4 layer = vec4(0.0);

	float line=1.0,angle,numLines=6.0;

	for(float i=0.0;i<numLines;i++)
	{
		angle = (PI / (numLines*2.0)) + (TWOPI * i / (numLines*2.0));
		line = min(line,sdLine(rotateUV(uv,vec2(0.0),angle),vec2(0.0,-0.2),vec2(0.0,0.2),0.0005));
	}

	layer = (line>0.0015) ? vec4(0.0) : vec4(vec3(0.0),smoothstep(0.001,0.0, line));

	return layer;
}

vec4 clockRings(vec2 uv)
{
	vec4 layer = vec4(0.0);
	float d = 0.0,rw = 0.1;

	d = sdRing(uv,vec2(0.0),0.4,rw);
	d = min(d,sdRing(uv,vec2(0.0),0.37,rw));
	d = min(d,sdRing(uv,vec2(0.0),0.2,rw));
	d = min(d,sdRing(uv,vec2(0.0),0.05,rw));

	layer = (d>0.0015) ? vec4(0.0) : vec4(vec3(0.0),smoothstep(0.001,0.0, d));

	return layer;
}

vec4 clockMarks(vec2 uv)
{
	vec4 layer = vec4(0.0);
	vec2 duv = uv;
	float mark = 1.0;

	float radius = 0.385;
	float numMarks = 60.0;
	float at = atan(uv.y, uv.x);
	float i = floor((at * numMarks / TWOPI) + 0.5);
	float angle = (TWOPI / numMarks) * i;

	if(mod(i,5.0)==0.0)
		mark = min(mark,sdCircle(uv,vec2(cos(angle),sin(angle))*radius*0.5, 0.006));
	else
		mark = min(mark,sdCircle(uv,vec2(cos(angle),sin(angle))*radius*0.5, 0.003));

	numMarks = 12.0;
	float r, width;
	float a = mod(atan(uv.x,uv.y),TWOPI/numMarks) - TWOPI / (numMarks * 2.0);
	uv = vec2(sin(a),cos(a)) * length(uv);
	r = 0.185; width = 0.003;
	mark = min(mark,length(uv-vec2(0.0,r)) - width);
	r = 0.1;

	mark = min(mark, length(uv-vec2(0.0,r)) - width);
	mark = min(mark, step(0.0,length(duv)-0.015));

	layer = (mark>0.0015) ? vec4(0.0) : vec4(vec3(0.0),smoothstep(0.001,0.0, mark));

	return layer;
}

vec4 clockHandShape(in vec2 st,vec2 pos,vec2 size,int type)
{
	vec4 layer=vec4(0.0);

	st -= pos;
	st /= size;

	float blurVal=0.008,shResult=0.0;

	float angle=0.0;
	float h = -(TWOPI*(iDate.w/3600.0/12.0));
	float m = mod(floor(iDate.w/60.0),60.0);
	float s = mod(floor(iDate.w),60.0);

	// Hour
	if(type==0)
	{
		angle = (iDate.w<=0.0)?sin(iTime*0.5)*PI+PI:h;
		vec2 huv = rotateUV(st,vec2(0.0),angle);

		if(abs(huv.x)*size.x>(size.x*0.11)||abs(huv.y-0.45)*size.y>(size.y*0.87)) return vec4(0.0);

		shResult = sdRhombus(huv,vec2(0.0,-0.2),vec2(0.05,0.2),blurVal);
		shResult = max(shResult,sdRhombus(huv,vec2(0.0,0.4),vec2(0.1,0.625),blurVal));
		shResult = max(shResult,sdRhombus(huv,vec2(0.0,1.0),vec2(0.085,0.3),blurVal));
	}
	// Minute
	if(type==1)
	{
		angle = (iDate.w<=0.0)?cos(iTime*0.25)*TWOPI:-(TWOPI*m/60.0);
		vec2 muv = rotateUV(st,vec2(0.0),angle);

		if(abs(muv.x)*size.x>(size.x*0.11)||abs(muv.y-0.55)*size.y>(size.y)) return vec4(0.0);

		shResult = sdRhombus(muv,vec2(0.0,-0.28),vec2(0.05,0.15),blurVal);
		shResult = max(shResult,sdRhombus(muv,vec2(0.0,0.55),vec2(0.09,0.9),blurVal));
		shResult = max(shResult,sdRhombus(muv,vec2(0.0,1.33),vec2(0.05,0.2),blurVal));
	}
	// Second hand
	if(type==2)
	{
		angle = (iDate.w <= 0.0) ? sin(iTime*0.75)*TWOPI : -(TWOPI*s/60.0);
		vec2 suv = rotateUV(st, vec2(0.0), angle);

		if(abs(suv.x)*size.x>(size.x*0.035)||abs(suv.y-0.55)*size.y>(size.y)) return vec4(0.0);

		shResult = sdRhombus(suv,vec2(0.0,-0.34),vec2(0.02,0.1),blurVal);
		shResult = max(shResult,sdRect(suv,vec2(0.0,0.6),vec2(0.01,0.9),blurVal));
		shResult = max(shResult,sdRhombus(suv,vec2(0.0,1.44),vec2(0.025,0.1),blurVal));
	}

	//layer = vec4(vec3(0.0),shResult);
	layer = (shResult>0.0) ? vec4(vec3(1.0),shResult) : vec4(0.0);

	return layer;
}

vec4 drawHistoricClock(vec2 uv, vec2 pos, float size)
{
	vec4 layer = vec4(0.0);

	vec2 huv = uv;

	uv -= pos;
	uv /= size;

	vec4 clockBackground = clockFace(uv);
	vec4 clockLinesLayer = clockLines(uv);
	vec4 clockRingsLayer = clockRings(uv);
	vec4 clockMarksLayer = clockMarks(uv);
	vec4 clockNumbersLayer = clockNumbers(uv);

	layer = mix(layer,clockBackground,clockBackground.a);
	layer = mix(layer,clockLinesLayer,clockLinesLayer.a);
	layer = mix(layer,clockRingsLayer,clockRingsLayer.a);
	layer = mix(layer,clockMarksLayer,clockMarksLayer.a);
	layer = mix(layer,clockNumbersLayer,clockNumbersLayer.a);

	layer = mix(layer, vec4(vec3(0.0),1.0), clockHandShape(huv,vec2(0.5),vec2(0.0925)*size,0).a);
	layer = mix(layer, vec4(vec3(0.0),1.0), clockHandShape(huv,vec2(0.5),vec2(0.115)*size,1).a);
	layer = mix(layer, vec4(vec3(0.0),1.0), clockHandShape(huv,vec2(0.5),vec2(0.122)*size,2).a);

	return layer;
}

void mainImage_(out vec4 fragColor,in vec2 fragCoord)
{
	vec2 uv = (fragCoord.xy-0.5*resolution.xy)/min(resolution.x,resolution.y)+0.5;

	float scaleFactor = (iResolution.x < iResolution.y) ? 2.25 : 2.0;

	vec4 historicClockLayer = drawHistoricClock(uv, vec2(0.5), scaleFactor);

	fragColor = mix(vec4(0.0),historicClockLayer,historicClockLayer.a);
}

#ifndef SHADERTOY
void main(void)
{
	mainImage_(gl_FragColor, xlv_TEXCOORD0.xy);
}
#endif

