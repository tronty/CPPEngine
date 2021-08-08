#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct main0_in
{
    float2 vTexCoord [[user(locn0)]];
};

struct main0_out
{
    float4 color [[color(0)]];
};

fragment main0_out main0(main0_in in [[stage_in]], texture2d<float> Base [[texture(0)]], sampler BaseSmplr [[sampler(0)]])
{
    main0_out out = {};
    out.color = Base.sample(BaseSmplr, in.vTexCoord);
    return out;
}

