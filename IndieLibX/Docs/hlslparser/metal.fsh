#include <metal_stdlib>
using namespace metal;

inline float4 column(float4x4 m, int i) {
    return float4(m[0][i], m[1][i], m[2][i], m[3][i]);
}
inline float3 column(float3x4 m, int i) {
    return float3(m[0][i], m[1][i], m[2][i]);
}
inline float2 column(float2x4 m, int i) {
    return float2(m[0][i], m[1][i]);
}
inline float4 set_column(thread float4x4& m, int i, float4 v) {
        m[0][i] = v.x; m[1][i] = v.y; m[2][i] = v.z; m[3][i] = v.w; return v;
}
inline float3 set_column(thread float3x4& m, int i, float3 v) {
        m[0][i] = v.x; m[1][i] = v.y; m[2][i] = v.z; return v;
}
inline float2 set_column(thread float2x4& m, int i, float2 v) {
        m[0][i] = v.x; m[1][i] = v.y; return v;
}
inline float3x3 matrix_ctor(float4x4 m) {
        return float3x3(m[0].xyz, m[1].xyz, m[2].xyz);
}
struct Texture2DSampler {
    const thread texture2d<float>& t;
    const thread sampler& s;
    Texture2DSampler(thread const texture2d<float>& t, thread const sampler& s) : t(t), s(s) {};
};
inline float4 tex2D(Texture2DSampler ts, float2 texCoord) {
    return ts.t.sample(ts.s, texCoord);
}
struct Pixel_Shader {
#line 1 "./SimpleTexture.fsh"
    struct VsOut {
        float4 position [[position]];
        float2 texCoord;
    };
    thread texture2d<float>& Base_texture; thread sampler& Base_sampler;
    float4 main(VsOut IN) {
        return tex2D(Texture2DSampler(Base_texture, Base_sampler), IN.texCoord);
    };

    Pixel_Shader(thread texture2d<float> & Base_texture, thread sampler & Base_sampler) : Base_texture(Base_texture), Base_sampler(Base_sampler) {}
};

fragment float4 main(Pixel_Shader::VsOut IN [[stage_in]], texture2d<float> Base_texture [[texture(0)]], sampler Base_sampler [[sampler(0)]]) {
    Pixel_Shader main(Base_texture, Base_sampler);
    return main.main(IN);
}
