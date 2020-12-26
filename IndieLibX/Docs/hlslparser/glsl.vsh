#version 140
#pragma optionNV(fastmath on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)
vec3 matrix_row0(mat3 m, int i) { return vec3( m[0][i], m[1][i], m[2][i] ); }
vec4 matrix_row0(mat4 m, int i) { return vec4( m[0][i], m[1][i], m[2][i], m[3][i] ); }
vec2  m_scalar_swizzle20(float x) { return  vec2(x, x); }
ivec2 m_scalar_swizzle20(int   x) { return ivec2(x, x); }
vec3  m_scalar_swizzle30(float x) { return  vec3(x, x, x); }
ivec3 m_scalar_swizzle30(int   x) { return ivec3(x, x, x); }
vec4  m_scalar_swizzle40(float x) { return  vec4(x, x, x, x); }
ivec4 m_scalar_swizzle40(int   x) { return ivec4(x, x, x, x); }
uvec2 m_scalar_swizzle20(uint  x) { return uvec2(x, x); }
uvec3 m_scalar_swizzle30(uint  x) { return uvec3(x, x, x); }
uvec4 m_scalar_swizzle40(uint  x) { return uvec4(x, x, x, x); }
vec2 bvecTernary0(bvec2 cond, vec2 trueExpr, vec2 falseExpr) { vec2 ret; ret.x = cond.x ? trueExpr.x : falseExpr.x; ret.y = cond.y ? trueExpr.y : falseExpr.y; return ret; }
vec3 bvecTernary0(bvec3 cond, vec3 trueExpr, vec3 falseExpr) { vec3 ret; ret.x = cond.x ? trueExpr.x : falseExpr.x; ret.y = cond.y ? trueExpr.y : falseExpr.y; ret.z = cond.z ? trueExpr.z : falseExpr.z; return ret; }
vec4 bvecTernary0(bvec4 cond, vec4 trueExpr, vec4 falseExpr) { vec4 ret; ret.x = cond.x ? trueExpr.x : falseExpr.x; ret.y = cond.y ? trueExpr.y : falseExpr.y; ret.z = cond.z ? trueExpr.z : falseExpr.z; ret.w = cond.w ? trueExpr.w : falseExpr.w; return ret; }
in vec2 POSITION;
in vec2 TEXCOORD0;
out vec2 frag_TEXCOORD0;
struct VsIn {
#line 2
    vec2 position;
    vec2 texCoord;
};
struct VsOut {
    vec4 position;
    vec2 texCoord;
};
uniform mat4 worldViewProj;
VsOut main(VsIn In) {
    VsOut Out = VsOut (0);
    ((Out).position = ((worldViewProj)*(vec4(((In).position).x, ((In).position).y, 0.000000, 1.000000))));
    ((Out).texCoord = (In).texCoord);
    return Out;
}
void main() {
    VsIn In;
    In.position = POSITION;
    In.texCoord = TEXCOORD0;
    VsOut result = main(In);
    gl_Position = result.position;
    frag_TEXCOORD0 = result.texCoord;
}
