struct TextureSampler2D {
    Texture2D    t;
    SamplerState s;
};
struct TextureSampler2DShadow {
    Texture2D                t;
    SamplerComparisonState   s;
};
struct TextureSampler3D {
    Texture3D    t;
    SamplerState s;
};
struct TextureSamplerCube {
    TextureCube  t;
    SamplerState s;
};
TextureSampler2D CreateTextureSampler2D(Texture2D t, SamplerState s) {
    TextureSampler2D ts;
    ts.t = t; ts.s = s;
    return ts;
}
TextureSampler2DShadow CreateTextureSampler2DShadow(Texture2D t, SamplerComparisonState s) {
    TextureSampler2DShadow ts;
    ts.t = t; ts.s = s;
    return ts;
}
TextureSampler3D CreateTextureSampler3D(Texture3D t, SamplerState s) {
    TextureSampler3D ts;
    ts.t = t; ts.s = s;
    return ts;
}
TextureSamplerCube CreateTextureSamplerCube(TextureCube t, SamplerState s) {
    TextureSamplerCube ts;
    ts.t = t; ts.s = s;
    return ts;
}
#line 1 "./SimpleTexture.vsh"
struct VsIn {
    float2 position : POSITION;
    float2 texCoord : TEXCOORD0;
};
struct VsOut {
#line 7
    float2 texCoord : TEXCOORD0;
#line 6
    float4 position : SV_Position;
};
#line 9
float4x4 worldViewProj;
VsOut main(VsIn In) {
    VsOut Out = (VsOut )(0);
    ((Out).position = mul(worldViewProj, float4(((In).position).x, ((In).position).y, 0.000000, 1.000000)));
    ((Out).texCoord = (In).texCoord);
    return Out;
};
