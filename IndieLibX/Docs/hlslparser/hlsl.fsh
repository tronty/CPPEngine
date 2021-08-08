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
float4 tex2D1(TextureSampler2D ts, float2 texCoord) {
    return ts.t.Sample(ts.s, texCoord);
}
#line 1 "./SimpleTexture.fsh"
struct VsOut {
#line 3
    float2 texCoord : TEXCOORD0;
};
Texture2D Base_texture; SamplerState Base_sampler;
float4 main(VsOut IN) : SV_Target {
    return tex2D1(CreateTextureSampler2D(Base_texture, Base_sampler), (IN).texCoord);
};
