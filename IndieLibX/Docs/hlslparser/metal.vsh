#include <metal_stdlib>
using namespace metal;

inline float2 mul(float2 a, float2x2 m) {
    return a * m;
}
inline float3 mul(float3 a, float3x3 m) {
    return a * m;
}
inline float4 mul(float4 a, float4x4 m) {
    return a * m;
}
inline float2 mul(float2x2 m, float2 a) {
    return m * a;
}
inline float3 mul(float3x3 m, float3 a) {
    return m * a;
}
inline float4 mul(float4x4 m, float4 a) {
    return m * a;
}
inline float3 mul(float4 a, float3x4 m) {
    return a * m;
}
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
struct Vertex_Shader {
#line 1 "./SimpleTexture.vsh"
    struct VsIn {
        float2 position;
        float2 texCoord;
    };
    struct VsOut {
        float4 position [[position]];
        float2 texCoord;
    };
    float4x4 worldViewProj;
    VsOut main(VsIn In) {
        VsOut Out = (VsOut)(0);
        Out.position = mul(worldViewProj, float4(In.position.x, In.position.y, 0.000000, 1.000000));
        Out.texCoord = In.texCoord;
        return Out;
    };

    Vertex_Shader() {}
};

vertex Vertex_Shader::VsOut main(Vertex_Shader::VsIn In [[stage_in]]) {
    Vertex_Shader main;
    return main.main(In);
}
