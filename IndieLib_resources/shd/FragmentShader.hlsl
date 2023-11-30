struct VsOut
{
	float4 position;
	float2 uv;
};

struct VsIn
{
	float2 position;
	float2 uv;
};

float4 main( VsOut ) : COLOR0
{
  return float4(1.0, 1.0, 1.0, 1.0);
}

