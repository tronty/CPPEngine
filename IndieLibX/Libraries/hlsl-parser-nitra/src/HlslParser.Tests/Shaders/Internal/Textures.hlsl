struct VertexShaderInput
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

float Scalar1;
int Scalar2;

float2 Vector1;
int4 Vector2;
vector<bool, 3> Vector3;

matrix Matrix1;
float1x2 Matrix2;
int2x3 Matrix3;
bool4x2 Matrix4;
matrix<uint, 3, 2> Matrix5;

float4x4 WorldViewProjection;

Texture2D Picture;
Texture2D<bool> PictureTyped;
SamplerState PictureSampler;
SamplerComparisonState PictureSamplerComparison;

Texture2DMS<float4, 32> PictureMS;

PixelShaderInput VS(VertexShaderInput input)
{
	PixelShaderInput output = (PixelShaderInput) 0;
	
	output.pos = mul(input.pos, WorldViewProjection);
	output.tex = input.tex;
	
	return output;
}

float4 PS(PixelShaderInput input) : SV_Target
{
	float lod = Picture.CalculateLevelOfDetail(PictureSampler, input.tex);
	float lodUnclamped = Picture.CalculateLevelOfDetailUnclamped(PictureSampler, input.tex);
	float4 gathered = Picture.Gather(PictureSampler, input.tex, int2(0, 1));
	
	int width, height, numLevels;
	Picture.GetDimensions(1, width, height, numLevels);

	float2 samplePos = PictureMS.GetSamplePosition(0);

	float4 loaded = PictureMS.Load(int2(25, 10), 1, int2(0, 1));

	float4 sampled = Picture.Sample(PictureSampler, input.tex);
	float4 sampleBias = Picture.SampleBias(PictureSampler, input.tex, 0.5);
	float4 sampleCmp = Picture.SampleCmp(PictureSamplerComparison, input.tex, 0.4);
	float4 sampleCmpLevelZero = Picture.SampleCmpLevelZero(PictureSamplerComparison, input.tex, 0.6);
	float4 sampleGrad = Picture.SampleGrad(PictureSampler, input.tex, 0.1, 0.2);
	float4 sampleLevel = Picture.SampleLevel(PictureSampler, input.tex, 1.5);

	return float4(lod, lodUnclamped, width, height)
		+ gathered
		+ float4(samplePos, 0, 0)
		+ sampled
		+ sampleBias
		+ sampleCmp
		+ sampleCmpLevelZero
		+ sampleGrad
		+ sampleLevel;
}