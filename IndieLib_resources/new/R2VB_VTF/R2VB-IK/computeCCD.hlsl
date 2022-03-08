[Vertex shader]

struct VsOut {
	float4 Pos: POSITION;
	float2 tc0: TEXCOORD0;
};

VsOut main(float4 pos: POSITION, float2 t0 : TEXCOORD0){
	VsOut Out;

	Out.Pos = pos;
	Out.tc0 = t0;

	return Out;
}


[Fragment shader]

sampler PivotM;
sampler LinkTM;	
sampler EffectLinkTM;	
		
float4 EndPos;
static float R2D = (180.0f/3.14159f);
static float D2R = (3.14159f/180.0f);
static float XAxisLimit = 3.0f;
static float YAxisLimit = 0.0f;
static float ZAxisLimit = 3.0f;

float3 float4x4ToEuler(float4x4 m)
{
	float cx,sx,x;
	float cy,sy,y,yr;
	float cz,sz,z;
	float3 Euler;
	
	sy = -m[2][0];
	cy = sqrt(1 - (sy * sy));
	yr = (float)atan2(sy,cy);
	Euler.y = yr * R2D;

	// AVOID DIVIDE BY ZERO ERROR ONLY WHERE Y= +-90 or +-270 
	// NOT CHECKING cy BECAUSE OF PRECISION ERRORS.
	if (sy != 1.0f && sy != -1.0f)	
	{
		cx = m[2][2] / cy;
		sx = m[2][1] / cy;
		Euler.x = ((float)atan2(sx,cx) * R2D);

		cz = m[0][0] / cy;
		sz = m[1][0] / cy;
		Euler.z = ((float)atan2(sz,cz) * R2D);
	}
	else
	{
		cx = m[1][1];
		sx = -m[1][2];
		Euler.x = ((float)atan2(sx,cx) * R2D);

		cz = 1.0f;
		sz = 0.0f;
		Euler.z = ((float)atan2(sz,cz) * R2D);
	}
	return Euler;
}

float3 QuaternionToEuler(float4 q)
{
	float m[3][3];
	float cx,sx,x;
	float cy,sy,y,yr;
	float cz,sz,z;
	float3 Euler;
	
	// CONVERT QUATERNION TO MATRIX - I DON'T REALLY NEED ALL OF IT.
	m[0][0] = 1.0f - (2.0f * q.y * q.y) - (2.0f * q.z * q.z);
	//m[0][1] = (2.0f * q.x * q.y) - (2.0f * q.w * q.z);
	//m[0][2] = (2.0f * q.x * q.z) + (2.0f * q.w * q.y);
	m[1][0] = (2.0f * q.x * q.y) + (2.0f * q.w * q.z);
	//m[1][1] = 1.0f - (2.0f * q.x * q.x) - (2.0f * q.z * q.z);
	//m[1][2] = (2.0f * q.y * q.z) - (2.0f * q.w * q.x);
	m[2][0] = (2.0f * q.x * q.z) - (2.0f * q.w * q.y);
	m[2][1] = (2.0f * q.y * q.z) + (2.0f * q.w * q.x);
	m[2][2] = 1.0f - (2.0f * q.x * q.x) - (2.0f * q.y * q.y);

	sy = -m[2][0];
	cy = sqrt(1 - (sy * sy));
	yr = (float)atan2(sy,cy);
	Euler.y = (yr * R2D);

	// AVOID DIVIDE BY ZERO ERROR ONLY WHERE Y= +-90 or +-270 
	// NOT CHECKING cy BECAUSE OF PRECISION ERRORS.
	if (sy != 1.0f && sy != -1.0f)	
	{
		cx = m[2][2] / cy;
		sx = m[2][1] / cy;
		Euler.x = ((float)atan2(sx,cx) * R2D);

		cz = m[0][0] / cy;
		sz = m[1][0] / cy;
		Euler.z = ((float)atan2(sz,cz) * R2D);
	}
	else
	{
		// SINCE Cos(Y) IS 0, ADOPT THE STANDARD Z = 0.
		// NEED SOME MORE OF THE MATRIX TERMS NOW.
		m[1][1] = 1.0f - (2.0f * q.x * q.x) - (2.0f * q.z * q.z);
		m[1][2] = (2.0f * q.y * q.z) - (2.0f * q.w * q.x);
		cx = m[1][1];
		sx = -m[1][2];
		Euler.x = ((float)atan2(sx,cx) * R2D);

		cz = 1.0f;
		sz = 0.0f;
		Euler.z = ((float)atan2(sz,cz) * R2D);
	}
	return Euler;
}

float4x4 Euler2float4x4(float3 e)
{
	float				fCos, fSin;
	float4x4			tm, m;

	fCos = cos(e.x*3.14159f/180.0f);
	fSin = sin(e.x*3.14159f/180.0f);
	float4x4 MX = float4x4(1.0f, 0.0f, 0.0f, 0.0f,
							0.0f, fCos, -fSin, 0.0f,
							0.0f, fSin, fCos, 0.0f,
							0.0f, 0.0f, 0.0f, 1.0f);

	fCos = cos(e.y*3.14159f/180.0f);
	fSin = sin(e.y*3.14159f/180.0f);
	float4x4 MY = float4x4(fCos, 0.0f, -fSin, 0.0f,
							0.0f, 1.0f, 0.0f, 0.0f,
							fSin, 0.0f, fCos, 0.0f,
							0.0f, 0.0f, 0.0f, 1.0f);

	fCos = cos(e.z*3.14159f/180.0f);
	fSin = sin(e.z*3.14159f/180.0f);
	float4x4 MZ = float4x4(fCos,  -fSin, 0.0f, 0.0f,
							fSin, fCos, 0.0f, 0.0f,
							0.0f,  0.0f, 1.0f, 0.0f,
							0.0f,  0.0f, 0.0f, 1.0f);   
	tm = mul(MX, MY); 
	m = mul(tm, MZ);
	
	return m;
}

float4 main(float2 t0: TEXCOORD0) : COLOR {
	float4x4 rm, pm, tpm, newpm;
	float4 LinkPos, EffectPos;
	float3 f, t, d;
	float Rad;
	float3 Axis;
	float4 Quat;
	float3 Euler;
	
	EffectPos = tex2D(EffectLinkTM, float2(0.875f, 0.5f));
	d = EffectPos.xyz - EndPos.xyz;
	if (length(d) > 0.5f) //It's close enough or not?
	{	
		LinkPos = tex2D(LinkTM, float2(0.875f, 0.5f));		
	
		f = EndPos.xyz - LinkPos.xyz;
		f = normalize(f);
		
		t = EffectPos.xyz - LinkPos.xyz;
		t = normalize(t);
		
		Axis = cross(t, f);
		Axis = normalize(Axis);		
		
		Rad = acos(dot(t, f));
		Rad *= 0.1f;
		
		// get the quaternion of this link.
		Quat.x = Axis.x*sin(Rad);
		Quat.y = Axis.y*sin(Rad);
		Quat.z = Axis.z*sin(Rad);
		Quat.w = cos(Rad);
		// convert quaternion to Euler degree.
		Euler = QuaternionToEuler(Quat);
		
		//limit rotation degree to the maximum rotation degree of each axis.
		Euler.x = min(Euler.x, XAxisLimit);
		Euler.x = max(Euler.x, -XAxisLimit);
		
		Euler.y = min(Euler.y, YAxisLimit);
		Euler.y = max(Euler.y, -YAxisLimit);
		
		Euler.z = min(Euler.z, ZAxisLimit);
		Euler.z = max(Euler.z, -ZAxisLimit);
		
		// conver Euler degree to Matrix4x4.
		rm = Euler2float4x4(Euler);
		// get current pivot matrix.
		pm[0] = tex2D(PivotM, float2(0.125f, 0.5f));
		pm[1] = tex2D(PivotM, float2(0.275f, 0.5f));
		pm[2] = tex2D(PivotM, float2(0.625f, 0.5f));
		pm[3] = tex2D(PivotM, float2(0.875f, 0.5f));
		
		// new PM = rm * PM
		newpm = mul(rm, pm);			
	
		return newpm[t0.x*4.0f];		
	}
	else //close enough, don't need to rotate any more.
	{
		return tex2D(PivotM, t0);
	}			
}
