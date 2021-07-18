#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct main0_in
{
    float2 aPosition [[attribute(0)]];
    float2 aTexCoord [[attribute(1)]];
};

struct main0_out
{
    float2 vTexCoord [[user(locn0)]];
    float4 gl_Position [[position]];
    float gl_PointSize [[point_size]];
    float gl_ClipDistance[1] /* [[clip_distance]] built-in not yet supported under Metal. */;
};

vertex main0_out main0(main0_in in [[stage_in]])
{
    main0_out out = {};
    out.gl_Position = worldViewProj * float4(in.aPosition.x, in.aPosition.y, 0.0, 1.0);
    out.vTexCoord = in.aTexCoord;
    return out;
}

