#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct frag_out
{
    float4 ResultColor [[color(0)]];
};

struct frag_in
{
    float4 Color [[user(locn0)]];
    float4 TRBRTLBL_BorderRadius [[user(locn1)]];
    float2 UV [[user(locn2)]];
    float2 Position [[user(locn3)]];
    float2 Size [[user(locn4)]];
    float2 VertPos [[user(locn5)]];
};

static inline __attribute__((always_inline))
float2 select(thread const bool& selector, thread const float2& a, thread const float2& b)
{
    return (a * float(selector)) + (b * float(!selector));
}

static inline __attribute__((always_inline))
float select(thread const bool& selector, thread const float& a, thread const float& b)
{
    return (float(selector) * a) + (float(!selector) * b);
}

static inline __attribute__((always_inline))
float sdRoundedBox(thread const float2& p, thread const float2& b, thread float4& r)
{
    bool param = p.x > 0.0;
    float2 param_1 = r.xy;
    float2 param_2 = r.zw;
    float2 _70 = select(param, param_1, param_2);
    r.x = _70.x;
    r.y = _70.y;
    bool param_3 = p.y > 0.0;
    float param_4 = r.x;
    float param_5 = r.y;
    r.x = select(param_3, param_4, param_5);
    float2 q = (abs(p) - b) + float2(r.x);
    return (fast::min(fast::max(q.x, q.y), 0.0) + length(fast::max(q, float2(0.0)))) - r.x;
}

fragment frag_out frag(frag_in in [[stage_in]], texture2d<float> Tex [[texture(0)]], sampler TexSmplr [[sampler(0)]])
{
    frag_out out = {};
    float4 texColor = Tex.sample(TexSmplr, in.UV);
    float2 extents = in.Size * 0.5;
    float2 param = (in.VertPos - in.Position) - extents;
    float2 param_1 = extents;
    float4 param_2 = in.TRBRTLBL_BorderRadius;
    float _143 = sdRoundedBox(param, param_1, param_2);
    float sdf = -_143;
    out.ResultColor = texColor * in.Color;
    out.ResultColor.w *= fast::clamp(sdf, 0.0, 1.0);
    return out;
}

