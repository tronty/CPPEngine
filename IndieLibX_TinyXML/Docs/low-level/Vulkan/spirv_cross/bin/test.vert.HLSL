uniform float4 gl_HalfPixel;

static float4 gl_Position;
static float4x4 worldViewProj;
static float2 aPosition;
static float2 vTexCoord;
static float2 aTexCoord;

struct SPIRV_Cross_Input
{
    float4 worldViewProj_0 : TEXCOORD0;
    float4 worldViewProj_1 : TEXCOORD1;
    float4 worldViewProj_2 : TEXCOORD2;
    float4 worldViewProj_3 : TEXCOORD3;
    float2 aPosition : TEXCOORD4;
    float2 aTexCoord : TEXCOORD5;
};

struct SPIRV_Cross_Output
{
    float2 vTexCoord : TEXCOORD0;
    float4 gl_Position : POSITION;
};

void vert_main()
{
    gl_Position = mul(float4(aPosition.x, aPosition.y, 0.0f, 1.0f), worldViewProj);
    vTexCoord = aTexCoord;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    worldViewProj[0] = stage_input.worldViewProj_0;
    worldViewProj[1] = stage_input.worldViewProj_1;
    worldViewProj[2] = stage_input.worldViewProj_2;
    worldViewProj[3] = stage_input.worldViewProj_3;
    aPosition = stage_input.aPosition;
    aTexCoord = stage_input.aTexCoord;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.vTexCoord = vTexCoord;
    stage_output.gl_Position.x = stage_output.gl_Position.x - gl_HalfPixel.x * stage_output.gl_Position.w;
    stage_output.gl_Position.y = stage_output.gl_Position.y + gl_HalfPixel.y * stage_output.gl_Position.w;
    return stage_output;
}
