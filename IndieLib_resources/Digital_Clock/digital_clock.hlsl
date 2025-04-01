/*
  Copyright (c) 2016 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
[Vertex shader]
#if 1
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
int bFS=0;

struct VS_INPUT
{
	float2 position  : POSITION;
};

struct VS_OUTPUT
{
	float4 hposition : POSITION;
	float2 tposition : TEXCOORD1;
	float2 texcoord0 : TEXCOORD0;
};

VS_OUTPUT main( VS_INPUT IN )
{
	VS_OUTPUT OUT=(VS_OUTPUT)0;

	float4 v = float4( IN.position.x,
		               IN.position.y,
					   0,
					   1.0f );
    if(bFS==0)
    	OUT.hposition = mul( worldViewProj, v );
    else
    {
    	//OUT.hposition = v.xyww;    	
   	v.xy = sign(v.xy);
   	OUT.hposition = v;
    }
    	
    OUT.texcoord0.x=(v.x+1.0)/2.0;
    OUT.texcoord0.y=(v.y-1.0)*-0.5;

    OUT.tposition=(sign(v.xy)+1.0)/2.0;
    OUT.tposition.x*=3.5;
    OUT.tposition.y*=1.2;

    return OUT;
}
#endif
[Fragment shader]
#if 1
struct Plane
{
    float2 normal;
    float2 point;
};

struct Segment
{
    // four planes that define a segment in the LED
    Plane p0;
    Plane p1;
    Plane p2;
    Plane p3;
};

struct Digit
{
    float4 bottom;
    float4 top;
};

#if 0
Digit zero;//  = Digit(float4(1.0, 1.0, 1.0, 0.0), float4(0.0, 1.0, 1.0, 1.0));
Digit one;//   = Digit(float4(0.0, 0.0, 1.0, 0.0), float4(0.0, 0.0, 1.0, 0.0));
Digit two;//   = Digit(float4(1.0, 1.0, 0.0, 1.0), float4(1.0, 0.0, 1.0, 1.0));
Digit three;// = Digit(float4(1.0, 0.0, 1.0, 1.0), float4(1.0, 0.0, 1.0, 1.0));
Digit four;//  = Digit(float4(0.0, 0.0, 1.0, 1.0), float4(1.0, 1.0, 1.0, 0.0));
Digit five;//  = Digit(float4(1.0, 0.0, 1.0, 1.0), float4(1.0, 1.0, 0.0, 1.0));
Digit six;//   = Digit(float4(1.0, 1.0, 1.0, 1.0), float4(1.0, 1.0, 0.0, 1.0));
Digit seven;// = Digit(float4(0.0, 0.0, 1.0, 0.0), float4(0.0, 0.0, 1.0, 1.0));
Digit eight;// = Digit(float4(1.0, 1.0, 1.0, 1.0), float4(1.0, 1.0, 1.0, 1.0));
Digit nine;//  = Digit(float4(0.0, 0.0, 1.0, 1.0), float4(1.0, 1.0, 1.0, 1.0));
Digit off;//   = Digit(float4(0.0, 0.0, 0.0, 0.0), float4(0.0, 0.0, 0.0, 0.0));

Segment ledA;/* = Segment(Plane(float2( 1.0,-1.0), float2(0.1, 0.0)),
                       Plane(float2( 0.0,-1.0), float2(1.0, 0.9)),
                       Plane(float2(-1.0,-1.0), float2(5.0, 0.9)),
                       Plane(float2( 0.0, 1.0), float2(5.9, 0.0)));
*/
Segment ledB;/* = Segment(Plane(float2( 1.0, 0.0), float2(0.0, 0.1)),
                       Plane(float2(-1.0,-1.0), float2(0.0, 5.9)),
                       Plane(float2(-1.0, 0.0), float2(0.9, 5.0)),
                       Plane(float2(-1.0, 1.0), float2(0.9, 1.0)));
*/
Segment ledC;/* = Segment(Plane(float2(-1.0, 0.0), float2(6.0, 0.1)),
                       Plane(float2( 1.0,-1.0), float2(6.0, 5.9)),
                       Plane(float2( 1.0, 0.0), float2(5.1, 5.0)),
                       Plane(float2( 1.0, 1.0), float2(5.1, 1.0)));
*/
Segment ledD;/* = Segment(Plane(float2( 0.0,-1.0), float2(0.1, 6.0)),
                       Plane(float2(-1.0, 1.0), float2(5.9, 6.0)),
                       Plane(float2( 0.0, 1.0), float2(5.0, 5.1)),
                       Plane(float2( 1.0, 1.0), float2(1.0, 5.1)));
*/
#endif
#define ZeroToOne 0
#define ZeroToFive 1
#define ZeroToNine 2

// get the set of on/off states that represent a given digit
Digit getDigit(float num, const int range)
{
    Digit d;
    if (range == ZeroToOne)
    {
        if (num == 0.0)
        {
		//Digit zero;
		d.bottom=float4(1.0, 1.0, 1.0, 0.0);
		d.top=float4(0.0, 1.0, 1.0, 1.0);        
         //d = zero;
         }
        if (num == 1.0)
        {
	//	Digit one;
		d.bottom=float4(0.0, 0.0, 1.0, 0.0);
		d.top=float4(0.0, 0.0, 1.0, 0.0);        
         //d = one;
         }
        if (num == -1.0)
        {
	//	Digit off;
		d.bottom=float4(0.0, 0.0, 0.0, 0.0);
		d.top=float4(0.0, 0.0, 0.0, 0.0);        
         //d = off;
         }   // turn all segments off
    }
    else if (range == ZeroToFive)
    {
        if (num == 0.0)
        {
	//	Digit zero;
		d.bottom=float4(1.0, 1.0, 1.0, 0.0);
		d.top=float4(0.0, 1.0, 1.0, 1.0);        
         //d = zero;
         }
        if (num == 1.0)
        {
	//	Digit one;
		d.bottom=float4(0.0, 0.0, 1.0, 0.0);
		d.top=float4(0.0, 0.0, 1.0, 0.0);        
         //d = one;
         }
        if (num == 2.0)
        {
	//	Digit two;
		d.bottom=float4(1.0, 1.0, 0.0, 1.0);
		d.top=float4(1.0, 0.0, 1.0, 1.0);        
         //d = two;
         }
        if (num == 3.0)
        {
	//	Digit three;
		d.bottom=float4(1.0, 0.0, 1.0, 1.0);
		d.top=float4(1.0, 0.0, 1.0, 1.0);        
         //d = three;
         }
        if (num == 4.0)
        {
	//	Digit four;
		d.bottom=float4(0.0, 0.0, 1.0, 1.0);
		d.top=float4(1.0, 1.0, 1.0, 0.0);        
         //d = four;
         }
        if (num == 5.0)
        {
	//	Digit five;
		d.bottom=float4(1.0, 0.0, 1.0, 1.0);
		d.top=float4(1.0, 1.0, 0.0, 1.0);        
         //d = five;
         }
    }
    else if (range == ZeroToNine)
    {
        if (num == 0.0)
        {
	//	Digit zero;
		d.bottom=float4(1.0, 1.0, 1.0, 0.0);
		d.top=float4(0.0, 1.0, 1.0, 1.0);        
         //d = zero;
         }
        if (num == 1.0)
        {
	//	Digit one;
		d.bottom=float4(0.0, 0.0, 1.0, 0.0);
		d.top=float4(0.0, 0.0, 1.0, 0.0);        
         //d = one;
         }
        if (num == 2.0)
        {
	//	Digit two;
		d.bottom=float4(1.0, 1.0, 0.0, 1.0);
		d.top=float4(1.0, 0.0, 1.0, 1.0);        
         //d = two;
         }
        if (num == 3.0)
        {
	//	Digit three;
		d.bottom=float4(1.0, 0.0, 1.0, 1.0);
		d.top=float4(1.0, 0.0, 1.0, 1.0);        
         //d = three;
         }
        if (num == 4.0)
        {
	//	Digit four;
		d.bottom=float4(0.0, 0.0, 1.0, 1.0);
		d.top=float4(1.0, 1.0, 1.0, 0.0);        
         //d = four;
         }
        if (num == 5.0)
        {
	//	Digit five;
		d.bottom=float4(1.0, 0.0, 1.0, 1.0);
		d.top=float4(1.0, 1.0, 0.0, 1.0);        
         //d = five;
         }
        if (num == 6.0)
        {
	//	Digit six;
		d.bottom=float4(1.0, 1.0, 1.0, 1.0);
		d.top=float4(1.0, 1.0, 0.0, 1.0);        
         //d = six;
         }
        if (num == 7.0)
        {
	//	Digit seven;
		d.bottom=float4(0.0, 0.0, 1.0, 0.0);
		d.top=float4(0.0, 0.0, 1.0, 1.0);        
         //d = seven;
         }
        if (num == 8.0)
        {
	//	Digit eight;
		d.bottom=float4(1.0, 1.0, 1.0, 1.0);
		d.top=float4(1.0, 1.0, 1.0, 1.0);        
         //d = eight;
         }
        if (num == 9.0)
        {
	//	Digit nine;
		d.bottom=float4(0.0, 0.0, 1.0, 1.0);
		d.top=float4(1.0, 1.0, 1.0, 1.0);        
         //d = nine;
         }
    }

    return d;
}

#define scale 10.0/120.0

/*
// returns true if any component of the two float4's is non zero
bool any(float4 a, float4 b)
{
    a = clamp(dot(a, float4(1.0)), 0.0, 1.0);
    b = clamp(dot(b, float4(1.0)), 0.0, 1.0);

    return bool(max(a, b));
}
*/
// returns true if any component of the two float4's is non zero
bool any(float4 a, float4 b)
{
    //return bool((length(a)>0)||(length(b)>0));//???   
    return (a.x!=0.0)||(a.y!=0.0)||(a.z!=0.0)||(a.w!=0.0)||(b.x!=0.0)||(b.y!=0.0)||(b.z!=0.0)||(b.w!=0.0);
}

float3 calcPlaneEq(Plane p, float2 offset)
{
    return float3(p.normal, dot(-p.normal, (p.point + offset)*scale));
}

float insideSegment(float2 pos, Segment seg, float2 offset)
{
    float3 p0 = calcPlaneEq(seg.p0, offset);
    float3 p1 = calcPlaneEq(seg.p1, offset);
    float3 p2 = calcPlaneEq(seg.p2, offset);
    float3 p3 = calcPlaneEq(seg.p3, offset);

    float4 tmp = pos.x * float4(p0.x, p1.x, p2.x, p3.x);
    tmp = (pos.y * float4(p0.y, p1.y, p2.y, p3.y)) + tmp;
    tmp = clamp(tmp + float4(p0.z, p1.z, p2.z, p3.z), 0.0, 1.0);

    tmp.xy = min(tmp.xy, tmp.zw);
    return min(tmp.x, tmp.y);
}

// returns true if point is inside any segment, and that segment is on
bool insideDigit(float2 point, float xOffset, Digit d)
{
Segment ledA;/* = Segment(Plane(float2( 1.0,-1.0), float2(0.1, 0.0)),
                       Plane(float2( 0.0,-1.0), float2(1.0, 0.9)),
                       Plane(float2(-1.0,-1.0), float2(5.0, 0.9)),
                       Plane(float2( 0.0, 1.0), float2(5.9, 0.0)));
*/
Segment ledB;/* = Segment(Plane(float2( 1.0, 0.0), float2(0.0, 0.1)),
                       Plane(float2(-1.0,-1.0), float2(0.0, 5.9)),
                       Plane(float2(-1.0, 0.0), float2(0.9, 5.0)),
                       Plane(float2(-1.0, 1.0), float2(0.9, 1.0)));
*/
Segment ledC;/* = Segment(Plane(float2(-1.0, 0.0), float2(6.0, 0.1)),
                       Plane(float2( 1.0,-1.0), float2(6.0, 5.9)),
                       Plane(float2( 1.0, 0.0), float2(5.1, 5.0)),
                       Plane(float2( 1.0, 1.0), float2(5.1, 1.0)));
*/
Segment ledD;/* = Segment(Plane(float2( 0.0,-1.0), float2(0.1, 6.0)),
                       Plane(float2(-1.0, 1.0), float2(5.9, 6.0)),
                       Plane(float2( 0.0, 1.0), float2(5.0, 5.1)),
                       Plane(float2( 1.0, 1.0), float2(1.0, 5.1)));
*/
//Segment ledA;
ledA.p0.normal=float2( 1.0,-1.0);
ledA.p0.point=float2(0.1, 0.0);
ledA.p1.normal=float2( 0.0,-1.0);
ledA.p1.point=float2(1.0, 0.9);
ledA.p2.normal=float2(-1.0,-1.0);
ledA.p2.point=float2(5.0, 0.9);
ledA.p3.normal=float2( 0.0, 1.0);
ledA.p3.point=float2(5.9, 0.0);

//Segment ledB;
ledB.p0.normal=float2( 1.0, 0.0);
ledB.p0.point=float2(0.0, 0.1);
ledB.p1.normal=float2(-1.0,-1.0);
ledB.p1.point=float2(0.0, 5.9);
ledB.p2.normal=float2(-1.0, 0.0);
ledB.p2.point=float2(0.9, 5.0);
ledB.p3.normal=float2(-1.0, 1.0);
ledB.p3.point=float2(0.9, 1.0);

//Segment ledC;
ledC.p0.normal=float2(-1.0, 0.0);
ledC.p0.point=float2(6.0, 0.1);
ledC.p1.normal=float2( 1.0,-1.0);
ledC.p1.point=float2(6.0, 5.9);
ledC.p2.normal=float2( 1.0, 0.0);
ledC.p2.point=float2(5.1, 5.0);
ledC.p3.normal=float2( 1.0, 1.0);
ledC.p3.point=float2(5.1, 1.0);

//Segment ledD;
ledD.p0.normal=float2( 0.0,-1.0);
ledD.p0.point=float2(0.1, 6.0);
ledD.p1.normal=float2(-1.0, 1.0);
ledD.p1.point=float2(5.9, 6.0);
ledD.p2.normal=float2( 0.0, 1.0);
ledD.p2.point=float2(5.0, 5.1);
ledD.p3.normal=float2( 1.0, 1.0);
ledD.p3.point=float2(1.0,5.1);

    float4 bottom;
    bottom.x = insideSegment(point, ledA, float2(xOffset, 1.0));
    bottom.y = insideSegment(point, ledB, float2(xOffset, 1.0));
    bottom.z = insideSegment(point, ledC, float2(xOffset, 1.0));
    bottom.w = insideSegment(point, ledD, float2(xOffset, 1.0));

    float4 top;
    top.x = insideSegment(point, ledA, float2(xOffset, 7.0));
    top.y = insideSegment(point, ledB, float2(xOffset, 7.0));
    top.z = insideSegment(point, ledC, float2(xOffset, 7.0));
    top.w = insideSegment(point, ledD, float2(xOffset, 7.0));

    return any(bottom*d.bottom, top*d.top);
}

// returns true if point is inside any segment, even if that segment is off
bool insideDigit(float2 point, float xOffset)
{
Segment ledA;/* = Segment(Plane(float2( 1.0,-1.0), float2(0.1, 0.0)),
                       Plane(float2( 0.0,-1.0), float2(1.0, 0.9)),
                       Plane(float2(-1.0,-1.0), float2(5.0, 0.9)),
                       Plane(float2( 0.0, 1.0), float2(5.9, 0.0)));
*/
Segment ledB;/* = Segment(Plane(float2( 1.0, 0.0), float2(0.0, 0.1)),
                       Plane(float2(-1.0,-1.0), float2(0.0, 5.9)),
                       Plane(float2(-1.0, 0.0), float2(0.9, 5.0)),
                       Plane(float2(-1.0, 1.0), float2(0.9, 1.0)));
*/
Segment ledC;/* = Segment(Plane(float2(-1.0, 0.0), float2(6.0, 0.1)),
                       Plane(float2( 1.0,-1.0), float2(6.0, 5.9)),
                       Plane(float2( 1.0, 0.0), float2(5.1, 5.0)),
                       Plane(float2( 1.0, 1.0), float2(5.1, 1.0)));
*/
Segment ledD;/* = Segment(Plane(float2( 0.0,-1.0), float2(0.1, 6.0)),
                       Plane(float2(-1.0, 1.0), float2(5.9, 6.0)),
                       Plane(float2( 0.0, 1.0), float2(5.0, 5.1)),
                       Plane(float2( 1.0, 1.0), float2(1.0, 5.1)));
*/
//Segment ledA;
ledA.p0.normal=float2( 1.0,-1.0);
ledA.p0.point=float2(0.1, 0.0);
ledA.p1.normal=float2( 0.0,-1.0);
ledA.p1.point=float2(1.0, 0.9);
ledA.p2.normal=float2(-1.0,-1.0);
ledA.p2.point=float2(5.0, 0.9);
ledA.p3.normal=float2( 0.0, 1.0);
ledA.p3.point=float2(5.9, 0.0);

//Segment ledB;
ledB.p0.normal=float2( 1.0, 0.0);
ledB.p0.point=float2(0.0, 0.1);
ledB.p1.normal=float2(-1.0,-1.0);
ledB.p1.point=float2(0.0, 5.9);
ledB.p2.normal=float2(-1.0, 0.0);
ledB.p2.point=float2(0.9, 5.0);
ledB.p3.normal=float2(-1.0, 1.0);
ledB.p3.point=float2(0.9, 1.0);

//Segment ledC;
ledC.p0.normal=float2(-1.0, 0.0);
ledC.p0.point=float2(6.0, 0.1);
ledC.p1.normal=float2( 1.0,-1.0);
ledC.p1.point=float2(6.0, 5.9);
ledC.p2.normal=float2( 1.0, 0.0);
ledC.p2.point=float2(5.1, 5.0);
ledC.p3.normal=float2( 1.0, 1.0);
ledC.p3.point=float2(5.1, 1.0);

//Segment ledD;
ledD.p0.normal=float2( 0.0,-1.0);
ledD.p0.point=float2(0.1, 6.0);
ledD.p1.normal=float2(-1.0, 1.0);
ledD.p1.point=float2(5.9, 6.0);
ledD.p2.normal=float2( 0.0, 1.0);
ledD.p2.point=float2(5.0, 5.1);
ledD.p3.normal=float2( 1.0, 1.0);
ledD.p3.point=float2(1.0,5.1);

    float4 bottom;
    bottom.x = insideSegment(point, ledA, float2(xOffset, 1.0));
    bottom.y = insideSegment(point, ledB, float2(xOffset, 1.0));
    bottom.z = insideSegment(point, ledC, float2(xOffset, 1.0));
    bottom.w = insideSegment(point, ledD, float2(xOffset, 1.0));

    float4 top;
    top.x = insideSegment(point, ledA, float2(xOffset, 7.0));
    top.y = insideSegment(point, ledB, float2(xOffset, 7.0));
    top.z = insideSegment(point, ledC, float2(xOffset, 7.0));
    top.w = insideSegment(point, ledD, float2(xOffset, 7.0));

    return any(bottom, top);
}

// returns true if point is inside the specified circle
bool insideCircle(float2 curPos, float2 circleCenter, float radius)
{
    return (distance(curPos, circleCenter * scale) <= radius*scale);
}
/*
// function prototypes
Digit getDigit(float num, const int range);
bool insideDigit(float2 point, float xOffset, Digit d);
bool insideDigit(float2 point, float xOffset);
bool insideCircle(float2 curPos, float2 circleCenter, float radius);

//extern int ZeroToOne, ZeroToFive, ZeroToNine;
*/
/*uniform*/float hour;
/*uniform*/float minute;
/*uniform*/float second;

struct PS_OUTPUT
{
	float4 color : COLOR;
};

PS_OUTPUT main(float2 hposition: TEXCOORD1)
{
	PS_OUTPUT OUT=(PS_OUTPUT)0;
	//init();
    // separate out the digits for hours/minutes/seconds
    float hourHigh = floor(hour/10.0);
    float hourLow = hour - (hourHigh*10.0);

    float minuteHigh = floor(minute/10.0);
    float minuteLow = minute - (minuteHigh*10.0);

    float secondHigh = floor(second/10.0);
    float secondLow = second - (secondHigh*10.0);

    // turn leading zero digit off for hours 
    if (hourHigh == 0.0) 
        hourHigh = -1.0; 

    Digit hrHigh    = getDigit(hourHigh, ZeroToOne);
    Digit hrLow     = getDigit(hourLow, ZeroToNine);
    Digit minHigh   = getDigit(minuteHigh, ZeroToFive);
    Digit minLow    = getDigit(minuteLow, ZeroToNine);
    Digit secHigh   = getDigit(secondHigh, ZeroToFive);
    Digit secLow    = getDigit(secondLow, ZeroToNine);

    if (insideDigit(hposition.xy,  1.0, hrHigh) ||
        insideDigit(hposition.xy,  7.1, hrLow) ||
        insideDigit(hposition.xy, 15.0, minHigh) ||
        insideDigit(hposition.xy, 21.1, minLow) ||
        insideDigit(hposition.xy, 29.0, secHigh) ||
        insideDigit(hposition.xy, 35.1, secLow) ||

        // four dots for digit separators
        insideCircle(hposition.xy, float2(14.0, 9.0), 0.7) ||
        insideCircle(hposition.xy, float2(14.0, 5.0), 0.7) ||
        insideCircle(hposition.xy, float2(28.0, 9.0), 0.7) ||
        insideCircle(hposition.xy, float2(28.0, 5.0), 0.7))
    {
        // lit segments are bright green
        OUT.color = float4(0.0, 1.0, 0.0, 1.0);
    }
    else if (insideDigit(hposition.xy, 1.0) ||
             insideDigit(hposition.xy, 7.1) ||
             insideDigit(hposition.xy, 15.0) ||
             insideDigit(hposition.xy, 21.1) ||
             insideDigit(hposition.xy, 29.0) ||
             insideDigit(hposition.xy, 35.1))
    {
        // unlit segments are dark green
        OUT.color = float4(0.0, 0.2, 0.0, 1.0);
    }
    else
    {
        // fragments outside of any digit are black
        OUT.color = float4(0.0, 0.0, 0.0, 1.0);
    }
    return OUT;
}
#endif

